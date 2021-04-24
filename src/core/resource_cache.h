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
  private:
    struct node {
      std::aligned_storage_t<sizeof(Resource), alignof(Resource)> storage;
      ResourceInfo info;
      std::atomic<std::uint32_t> references;
      std::atomic<resource_state> state;

      node(ResourceInfo const &info):
          info{info}, references{0}, state{resource_state::unloaded} {}
    };

  public:
    using loader = resource_loader<Resource, ResourceInfo>;

    class handle {
    public:
      explicit handle(node &n, worker_thread &wt): node_{&n}, worker_{&wt} {}

      ~handle() {
        if (node_ && --node_->references == 0) {
          auto n = node_;
          worker_->emplace([=]() {
            if (n->references == 0 && n->state == resource_state::loaded) {
              n->state = resource_state::unloading;
              reinterpret_cast<Resource *>(&n->storage)->~Resource();
              n->state = resource_state::unloaded;
            }
          });
        }
      }

      handle(handle const &rhs) noexcept:
          node_{rhs.node_}, worker_{rhs.worker_} {
        if (node_) {
          ++node_->references;
        }
      }

      handle &operator=(handle const &rhs) noexcept {
        if (&rhs != this) {
          if (node_ && --node_->references == 0) {
            auto n = node_;
            worker_->emplace([=]() {
              if (n->references == 0 && n->state == resource_state::loaded) {
                n->state = resource_state::unloading;
                reinterpret_cast<Resource *>(&n->storage)->~Resource();
                n->state = resource_state::unloaded;
              }
            });
          }
          node_ = rhs.node_;
          worker_ = rhs.worker_;
          ++node_->references;
        }
        return *this;
      }

      handle(handle &&rhs) noexcept: node_{rhs.node_}, worker_{rhs.worker_} {
        rhs.node_ = nullptr;
      }

      handle &operator=(handle &&rhs) noexcept {
        if (&rhs != this) {
          if (node_ && --node_->references == 0) {
            auto n = node_;
            worker_->emplace([=]() {
              if (n->references == 0 && n->state == resource_state::loaded) {
                n->state = resource_state::unloading;
                reinterpret_cast<Resource *>(&n->storage)->~Resource();
                n->state = resource_state::unloaded;
              }
            });
          }
          node_ = rhs.node_;
          worker_ = rhs.worker_;
          rhs.node_ = nullptr;
        }
        return *this;
      }

      resource_state state() const noexcept {
        return node_->state;
      }

      Resource &operator*() const noexcept {
        return *reinterpret_cast<Resource *>(&node_->storage);
      }

      Resource *operator->() const noexcept {
        return reinterpret_cast<Resource *>(&node_->storage);
      }

    private:
      node *node_;
      worker_thread *worker_;
    };

    explicit resource_cache(worker_thread *worker, loader *loader) noexcept:
        worker_{worker}, loader_{loader} {}

    ~resource_cache() {
      worker_->wait();
    }

    template<typename... Args>
    bool try_emplace(std::string_view name, Args &&...args) {
      return nodes_
          .try_emplace(
              name, std::make_unique<node>(std::forward<Args>(args)...))
          .second;
    }

    handle at(std::string_view name) const {
      auto n = nodes_.at(name).get();
      if (++n->references == 1) {
        auto l = loader_;
        worker_->emplace([=]() {
          if (n->references > 0 && n->state == resource_state::unloaded) {
            n->state = resource_state::loading;
            auto result = l->load(n->info);
            if (result) {
              new (&n->storage) Resource(std::move(*result));
              n->state = resource_state::loaded;
            } else {
              n->state = resource_state::error;
            }
          }
        });
      }
      return handle{*n, *worker_};
    }

  private:
    worker_thread *worker_;
    loader *loader_;
    absl::flat_hash_map<std::string, std::unique_ptr<node>> nodes_;

    // resource_state state(std::uint32_t index) const noexcept {
    //  return nodes_[index]->state;
    //}

    // resource_state state(std::string_view name) const {
    //  return state(indices_.at(name));
    //}

    // template<typename... Args>
    // bool try_emplace(std::string_view name, Args &&...args) {
    //  return nodes_.try_emplace(name, std::forward<Args>(args)...).second;
    //}

    // void create_reference(std::uint32_t index) noexcept {
    //  auto &n = *nodes_[index];
    //  if (n.references++ == 0) {
    //    auto loader = loader_;
    //    worker_->emplace([&n, loader]() {
    //      if (n.references > 0 && n.state == resource_state::unloaded) {
    //        n.state = resource_state::loading;
    //        auto result = loader->load(n.info);
    //        if (result) {
    //          new (&n.storage) Resource(std::move(*result));
    //          n.state = resource_state::loaded;
    //        } else {
    //          n.state = resource_state::error;
    //        }
    //      }
    //    });
    //  }
    //}

    // void create_reference(std::string_view name) {
    //  create_reference(indices_.at(name));
    //}

    // void destroy_reference(std::uint32_t index) noexcept {
    //  auto &n = *nodes_[index];
    //  if (--n.references == 0) {
    //    worker_->emplace([&n]() {
    //      if (n.references <= 0 && n.state == resource_state::loaded) {
    //        n.state = resource_state::unloading;
    //        reinterpret_cast<Resource *>(&n.storage)->~Resource();
    //        n.state = resource_state::unloaded;
    //      }
    //    });
    //  }
    //}

    // void destroy_reference(std::string_view name) {
    //  destroy_reference(indices_.at(name));
    //}

    // void refresh(std::uint32_t index) noexcept {
    //  auto &n = *nodes_[index];
    //  auto loader = loader_;
    //  worker_->emplace([&n, loader]() {
    //    switch (n.state) {
    //    case resource_state::loaded:
    //      n.state = resource_state::unloading;
    //      reinterpret_cast<Resource *>(&n.storage)->~Resource();
    //      if (n.references > 0) {
    //        n.state = resource_state::loading;
    //        auto result = loader->load(n.info);
    //        if (result) {
    //          new (&n.storage) Resource(std::move(*result));
    //          n.state = resource_state::loaded;
    //        } else {
    //          n.state = resource_state::error;
    //        }
    //      }
    //      break;
    //    case resource_state::error:
    //      if (n.references > 0) {
    //        n.state = resource_state::loading;
    //        auto result = loader->load(n.info);
    //        if (result) {
    //          new (&n.storage) Resource(std::move(*result));
    //          n.state = resource_state::loaded;
    //        } else {
    //          n.state = resource_state::error;
    //        }
    //      } else {
    //        n.state = resource_state::unloaded;
    //      }
    //      break;
    //    }
    //  });
    //}

    // void refresh(std::string_view name) {
    //  refresh(indices_.at(name));
    //}

    // Resource *acquire(std::uint32_t index) const noexcept {
    //  auto &n = nodes_[index];
    //  return n.state == resource_state::loaded
    //             ? reinterpret_cast<Resource *>(&n.storage)
    //             : nullptr;
    //}

    // Resource *acquire(std::string_view name) const {
    //  return acquire(indices_.at(name));
    //}
  };
} // namespace imp