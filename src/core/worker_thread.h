#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace imp {
  class worker_thread {
  public:
    worker_thread();

    void wait();
    void join();

    template<typename... Args>
    void emplace(Args &&...args) {
      {
        auto lock = std::scoped_lock{mutex_};
        queue_.emplace(std::forward<Args>(args)...);
      }
      condvar_.notify_all();
    }

  private:
    bool joining_;
    std::queue<std::function<void()>> queue_;
    std::mutex mutex_;
    std::condition_variable condvar_;
    std::thread thread_;
  };
} // namespace imp