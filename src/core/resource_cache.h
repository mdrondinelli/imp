#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

#include <absl/container/flat_hash_map.h>

#include "resource_loader.h"
#include "resource_state.h"
#include "worker_thread.h"

namespace imp {
  template<typename Resource, typename ResourceInfo>
  class resource_cache {
  public:
    struct node {
      ResourceInfo info;
      std::atomic<int> references;
      std::atomic<resource_state> state;
      std::aligned_storage_t<sizeof(Resource), alignof(Resource)> storage;

      node(ResourceInfo const &info):
          info{info}, references{0}, state{resource_state::unloaded} {}
    };

    explicit resource_cache(
        resource_loader<Resource, ResourceInfo> *loader,
        worker_thread *worker,
        absl::flat_hash_map<std::uint32_t, ResourceInfo> const &infos):
        loader_{loader}, worker_{worker} {
      for (auto &[id, info] : infos) {
        nodes_.emplace(id, std::make_unique<node>(info));
      }
    }

    resource_state state(std::uint32_t id) const noexcept {
      auto it = nodes_.find(id);
      return it != nodes_.end() ? it->second.state : resource_state::error;
    }

    void create_reference(std::uint32_t id) noexcept {
      auto it = nodes_.find(id);
      if (it != nodes_.end()) {
        auto n = it->second.get();
        if (n->references++ == 0) {
          auto loader = loader_;
          worker_->emplace([=]() {
            if (n->references > 0 && n->state == resource_state::unloaded) {
              n->state = resource_state::loading;
              auto result = loader->load(n->info);
              if (result) {
                new (&n->storage) Resource(std::move(*result));
                n->state = resource_state::loaded;
              } else {
                n->state = resource_state::error;
              }
            }
          });
        }
      }
    }

    void destroy_reference(std::uint32_t id) noexcept {
      auto it = nodes_.find(id);
      if (it != nodes_.end()) {
        auto n = it->second.get();
        if (--n->references == 0) {
          auto loader = loader_;
          worker_->emplace([=]() {
            if (n->references <= 0 && n->state == resource_state::loaded) {
              n->state = resource_state::unloading;
              reinterpret_cast<Resource *>(&n->storage)->~Resource();
              n->state = resource_state::unloaded;
            }
          });
        }
      }
    }

    void refresh(std::uint32_t id) noexcept {
      auto it = nodes_.find(id);
      if (it != nodes_.end()) {
        auto n = it->second.get();
        auto loader = loader_;
        worker_->emplace([=]() {
          switch (n->state) {
          case resource_state::loaded:
            n->state = resource_state::unloading;
            reinterpret_cast<Resource *>(&n->storage)->~Resource();
            if (n->references > 0) {
              n->state = resource_state::loading;
              auto result = loader->load(n->info);
              if (result) {
                new (&n->storage) Resource(std::move(*result));
                n->state = resource_state::loaded;
              } else {
                n->state = resource_state::error;
              }
            }
            break;
          case resource_state::error:
            if (n->references > 0) {
              n->state = resource_state::loading;
              auto result = loader->load(n->info);
              if (result) {
                new (&n->storage) Resource(std::move(*result));
                n->state = resource_state::loaded;
              } else {
                n->state = resource_state::error;
              }
            } else {
              n->state = resource_state::unloaded;
            }
            break;
          }
        });
        /*if (--n->references == 0) {
          auto loader = loader_;
          worker_->emplace([=]() {
            if (n->references <= 0 && resource_state::loaded) {
              n->state = resource_state::unloading;
              reinterpret_cast<Resource *>(&n->storage)->~Resource();
              n->state = resource_state::unloaded;
            }
          });
        }*/
      }
    }

    Resource *operator[](std::uint32_t id) const noexcept {
      auto it = nodes_.find(id);
      return it != nodes_.end() && it->second->state == resource_state::loaded
                 ? reinterpret_cast<Resource *>(&it->second->storage)
                 : nullptr;
    }

  private:
    resource_loader<Resource, ResourceInfo> *loader_;
    worker_thread *worker_;
    absl::flat_hash_map<std::uint32_t, std::unique_ptr<node>> nodes_;
  };
} // namespace imp