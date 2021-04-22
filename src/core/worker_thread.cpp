#include "worker_thread.h"

namespace imp {
  worker_thread::worker_thread():
      joining_{false}, thread_{[this]() {
        for (;;) {
          auto lock = std::unique_lock{mutex_};
          condvar_.wait(lock, [this]() { return joining_ || !queue_.empty(); });
          if (!queue_.empty()) {
            auto work = std::move(queue_.front());
            queue_.pop();
            lock.unlock();
            work();
            condvar_.notify_all();
          } else {
            return;
          }
        }
      }} {}

  void worker_thread::wait() {
    auto lock = std::unique_lock{mutex_};
    condvar_.wait(lock, [this]() { return queue_.empty(); });
  }

  void worker_thread::join() {
    {
      auto lock = std::scoped_lock{mutex_};
      joining_ = true;
    }
    condvar_.notify_one();
    thread_.join();
  }
} // namespace imp