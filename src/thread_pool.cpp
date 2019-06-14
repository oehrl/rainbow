#include "rainbow/thread_pool.hpp"

namespace rainbow {

ThreadPool default_thread_pool;

ThreadPool::ThreadPool(unsigned int worker_count) : close_(false) {
  workers_.reserve(worker_count);
  for (unsigned int i = 0; i < worker_count; ++i) {
    workers_.emplace_back(&ThreadPool::WorkerThread, this);
  }
}

ThreadPool::~ThreadPool() {
  close_ = true;
  tasks_cv_.notify_all();
  for (auto &worker : workers_) {
    worker.join();
  }
}

void ThreadPool::WorkerThread() {
  while (!close_.load(std::memory_order_acquire)) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock{tasks_mutex_};
      tasks_cv_.wait(lock, [this]() {
        return tasks_.size() > 0 || close_.load(std::memory_order_acquire);
      });
      if (close_.load(std::memory_order_relaxed)) {
        break;
      }
      task = tasks_.front();
      tasks_.pop();
    }
    task();
  }
}

} // namespace rainbow
