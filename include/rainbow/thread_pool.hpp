#pragma once

#include <array>
#include <condition_variable>
#include <future>
#include <iterator>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

namespace rainbow {

// class Task final {
//  public:
//   template <typename F>
//   Task(F&& function, std::future<typename std::invoke_result_t<F>>* future) {
//     using ResultType = std::invoke_result_t<F>;
//     static_assert(sizeof(PackagedTaskInvoker) ==
//                   sizeof(PackagedTaskInvokerImpl<ResultType()>));
//     new (packaged_task_invoker_.data())
//         PackagedTaskInvokerImpl<ResultType()>(std::move(function), future);
//   }

//   Task(const Task&) = delete;
//   Task(Task&&) = default;

//   ~Task() {
//     reinterpret_cast<PackagedTaskInvoker*>(packaged_task_invoker_.data())
//         ->~PackagedTaskInvoker();
//   }

//   Task& operator=(const Task&) = delete;
//   Task& operator=(Task&&) = default;

//  private:
//   class PackagedTaskInvoker {
//    public:
//     virtual ~PackagedTaskInvoker() = default;
//     virtual void Invoke(void* packaged_task) = 0;

//    protected:
//     std::array<char, sizeof(std::packaged_task<void()>)> packaged_task_;
//   };

//   template <typename PackagedTaskType>
//   class PackagedTaskInvokerImpl final : public PackagedTaskInvoker {
//    public:
//     template <typename F>
//     PackagedTaskInvokerImpl(
//         F&& function, std::future<typename std::invoke_result_t<F>>* future)
//         {
//       static_assert(sizeof(PackagedTaskType) < packaged_task_.size());
//       new (packaged_task_.data()) PackagedTaskType(std::move(function));
//       if (future != nullptr) {
//         future = GetPackagedTask()->get_future();
//       }
//     }

//     void Invoke(void* packaged_task) { (*GetPackagedTask())(); }

//     PackagedTaskType* GetPackagedTask() {
//       return reinterpret_cast<PackagedTaskType*>(packaged_task_.data());
//     }
//   };

//   std::array<char, sizeof(PackagedTaskInvoker)> packaged_task_invoker_;
// };

class ThreadPool final {
 public:
  ThreadPool(unsigned int worker_count = std::thread::hardware_concurrency());
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;
  ~ThreadPool();

  template <typename T, typename... ArgTypes>
  auto EnqueueTask(T&& task, ArgTypes&&... arguments);

  inline size_t GetWorkerCount() const { return workers_.size(); }

 private:
  std::vector<std::thread> workers_;
  std::atomic<bool> close_;

  std::condition_variable tasks_cv_;
  std::mutex tasks_mutex_;
  std::queue<std::function<void()>> tasks_;

  void WorkerThread();
};

extern ThreadPool default_thread_pool;

template <typename FunctionType, typename... ArgTypes>
auto ThreadPool::EnqueueTask(FunctionType&& function, ArgTypes&&... arguments) {
  using ResultType = std::invoke_result_t<FunctionType, ArgTypes...>;

  // TODO: Remove the make shared by creating a custom Task class similar to
  // std::function that stores the functor inside the class instead of
  // allocating as std::function does it (up to a certain size of course). The
  // reason we cannot use std::function for that directly is, that it requires
  // the function object to be copyable.
  auto task = std::make_shared<std::packaged_task<ResultType()>>(
      std::bind(std::forward<FunctionType>(function),
                std::forward<ArgTypes>(arguments)...));

  std::future<ResultType> result = task->get_future();
  {
    std::unique_lock<std::mutex> lock(tasks_mutex_);
    tasks_.emplace([task]() { (*task)(); });
  }
  tasks_cv_.notify_one();
  return result;
}

}  // namespace rainbow
