#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "ResourceLoader.h"
#include "ResourceState.h"
#include "WorkerThread.h"

namespace imp {
  template<typename Resource, typename ResourceInfo>
  class ResourceCache {
  public:
    struct Node {
      std::aligned_storage_t<sizeof(Resource), alignof(Resource)> storage;
      ResourceInfo info;
      std::atomic<std::uint32_t> references;
      std::atomic<ResourceState> state;

      Node(ResourceInfo const &info):
          info{info}, references{0}, state{ResourceState::UNLOADED} {}
    };

    class Handle {
    public:
      explicit Handle(Node &node, WorkerThread &thread):
          node_{&node}, thread_{&thread} {}

      ~Handle() {
        if (node_ && --node_->references == 0) {
          auto n = node_;
          thread_->emplace([=]() {
            if (n->references == 0 && n->state == ResourceState::LOADED) {
              n->state = ResourceState::UNLOADING;
              reinterpret_cast<Resource *>(&n->storage)->~Resource();
              n->state = ResourceState::UNLOADED;
            }
          });
        }
      }

      Handle(Handle const &rhs) noexcept:
          node_{rhs.node_}, thread_{rhs.thread_} {
        if (node_) {
          ++node_->references;
        }
      }

      Handle &operator=(Handle const &rhs) noexcept {
        if (&rhs != this) {
          if (node_ && --node_->references == 0) {
            auto n = node_;
            thread_->emplace([=]() {
              if (n->references == 0 && n->state == ResourceState::LOADED) {
                n->state = ResourceState::UNLOADING;
                reinterpret_cast<Resource *>(&n->storage)->~Resource();
                n->state = ResourceState::UNLOADED;
              }
            });
          }
          node_ = rhs.node_;
          thread_ = rhs.thread_;
          ++node_->references;
        }
        return *this;
      }

      Handle(Handle &&rhs) noexcept: node_{rhs.node_}, thread_{rhs.thread_} {
        rhs.node_ = nullptr;
      }

      Handle &operator=(Handle &&rhs) noexcept {
        if (&rhs != this) {
          if (node_ && --node_->references == 0) {
            auto n = node_;
            thread_->emplace([=]() {
              if (n->references == 0 && n->state == ResourceState::LOADED) {
                n->state = ResourceState::UNLOADING;
                reinterpret_cast<Resource *>(&n->storage)->~Resource();
                n->state = ResourceState::UNLOADED;
              }
            });
          }
          node_ = rhs.node_;
          thread_ = rhs.thread_;
          rhs.node_ = nullptr;
        }
        return *this;
      }

      ResourceState state() const noexcept {
        return node_->state;
      }

      Resource &operator*() const noexcept {
        return *reinterpret_cast<Resource *>(&node_->storage);
      }

      Resource *operator->() const noexcept {
        return reinterpret_cast<Resource *>(&node_->storage);
      }

    private:
      Node *node_;
      WorkerThread *thread_;
    };

    explicit ResourceCache(
        WorkerThread &thread,
        ResourceLoader<Resource, ResourceInfo> &loader) noexcept:
        thread_{&thread}, loader_{&loader} {}

    ~ResourceCache() {
      thread_->wait();
    }

    bool insert(std::string_view name, ResourceInfo const &info) {
      return nodes_.try_emplace(name, std::make_unique<Node>(info)).second;
    }

    Handle at(std::string_view name) const {
      auto n = nodes_.at(name).get();
      if (++n->references == 1) {
        auto l = loader_;
        thread_->emplace([=]() {
          if (n->references > 0 && n->state == ResourceState::UNLOADED) {
            n->state = ResourceState::LOADING;
            auto result = l->load(n->info);
            if (result) {
              new (&n->storage) Resource(std::move(*result));
              n->state = ResourceState::LOADED;
            } else {
              n->state = ResourceState::ERROR;
            }
          }
        });
      }
      return Handle{*n, *thread_};
    }

  private:
    WorkerThread *thread_;
    ResourceLoader<Resource, ResourceInfo> *loader_;
    absl::flat_hash_map<std::string, std::unique_ptr<Node>> nodes_;
  };
} // namespace imp