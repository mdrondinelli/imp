#pragma once

#include <algorithm>
#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

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
        absl::flat_hash_map<std::string, ResourceInfo> const &resources):
        loader_{loader}, worker_{worker} {
      auto names = std::vector<std::string>{};
      names.reserve(resources.size());
      for (auto &resource : resources) {
        names.emplace_back(resource.first);
      }
      std::sort(names.begin(), names.end());
      indices_.reserve(resources.size());
      nodes_.reserve(resources.size());
      for (auto &name : names) {
        auto index = static_cast<std::uint32_t>(indices_.size());
        indices_.emplace(name, index);
        nodes_.emplace_back(std::make_unique<node>(resources.at(name)));
      }
    }

    resource_state state(std::uint32_t index) const noexcept {
      return nodes_[index]->state;
    }

    resource_state state(std::string_view name) const {
      return state(indices_.at(name));
    }

    void create_reference(std::uint32_t index) noexcept {
      auto &n = *nodes_[index];
      if (n.references++ == 0) {
        auto loader = loader_;
        worker_->emplace([&n, loader]() {
          if (n.references > 0 && n.state == resource_state::unloaded) {
            n.state = resource_state::loading;
            auto result = loader->load(n.info);
            if (result) {
              new (&n.storage) Resource(std::move(*result));
              n.state = resource_state::loaded;
            } else {
              n.state = resource_state::error;
            }
          }
        });
      }
    }

    void create_reference(std::string_view name) {
      create_reference(indices_.at(name));
    }

    void destroy_reference(std::uint32_t index) noexcept {
      auto &n = *nodes_[index];
      if (--n.references == 0) {
        worker_->emplace([&n]() {
          if (n.references <= 0 && n.state == resource_state::loaded) {
            n.state = resource_state::unloading;
            reinterpret_cast<Resource *>(&n.storage)->~Resource();
            n.state = resource_state::unloaded;
          }
        });
      }
    }

    void destroy_reference(std::string_view name) {
      destroy_reference(indices_.at(name));
    }

    void refresh(std::uint32_t index) noexcept {
      auto &n = *nodes_[index];
      auto loader = loader_;
      worker_->emplace([&n, loader]() {
        switch (n.state) {
        case resource_state::loaded:
          n.state = resource_state::unloading;
          reinterpret_cast<Resource *>(&n.storage)->~Resource();
          if (n.references > 0) {
            n.state = resource_state::loading;
            auto result = loader->load(n.info);
            if (result) {
              new (&n.storage) Resource(std::move(*result));
              n.state = resource_state::loaded;
            } else {
              n.state = resource_state::error;
            }
          }
          break;
        case resource_state::error:
          if (n.references > 0) {
            n.state = resource_state::loading;
            auto result = loader->load(n.info);
            if (result) {
              new (&n.storage) Resource(std::move(*result));
              n.state = resource_state::loaded;
            } else {
              n.state = resource_state::error;
            }
          } else {
            n.state = resource_state::unloaded;
          }
          break;
        }
      });
    }

    void refresh(std::string_view name) {
      refresh(indices_.at(name));
    }

    Resource *operator[](std::uint32_t index) const noexcept {
      auto &n = nodes_[index];
      return n.state == resource_state::loaded
                 ? reinterpret_cast<Resource *>(&n.storage)
                 : nullptr;
    }

    Resource *operator[](std::string_view name) const {
      return operator[](indices_.at(name));
    }

  private:
    resource_loader<Resource, ResourceInfo> *loader_;
    worker_thread *worker_;
    absl::flat_hash_map<std::string, std::uint32_t> indices_;
    std::vector<std::unique_ptr<node>> nodes_;
  };
} // namespace imp