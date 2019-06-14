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
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;
  ~ThreadPool();

  template <typename T, typename... ArgTypes>
  auto EnqueueTask(T &&task, ArgTypes &&... arguments);

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
auto ThreadPool::EnqueueTask(FunctionType &&function,
                             ArgTypes &&... arguments) {
  using ResultType = std::invoke_result_t<FunctionType, ArgTypes...>;

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

template <typename CounterType, typename FuncType>
void ParallelFor(CounterType begin, CounterType end, FuncType &&function) {
  static_assert(std::is_integral_v<CounterType>);
  static_assert(std::is_invocable_v<FuncType, CounterType>);

  using ResultType = std::invoke_result_t<FuncType, CounterType>;

  const auto worker_count = default_thread_pool.GetWorkerCount();
  const auto distance = end - begin;
  const auto elements_per_thread = distance / worker_count;

  std::vector<std::future<ResultType>> results;
  results.reserve(worker_count);
  for (size_t i = 0; i < worker_count; ++i) {
    const auto local_end = begin + elements_per_thread;
    results.emplace_back(
        default_thread_pool.EnqueueTask([function, begin, local_end]() {
          auto local_begin = begin;
          while (local_begin != local_end) {
            function(local_begin);
            ++local_begin;
          }
        }));
    begin = local_end;
  }
  while (begin != end) {
    function(begin);
    ++begin;
  }
  for (auto &result : results) {
    result.get();
  }
}

template <typename IteratorType, typename FuncType>
void ParallelForEach(IteratorType begin, IteratorType end,
                     FuncType &&function) {
  const auto distance = std::distance(begin, end);
  const auto elements_per_thread =
      distance / default_thread_pool.GetWorkerCount();
  for (size_t i = 0; i < default_thread_pool.GetWorkerCount(); ++i) {
    const auto local_end = std::next(begin, elements_per_thread);
    default_thread_pool.EnqueueTask([function, begin, local_end]() {
      while (begin != local_end) {
        function(*begin);
        std::advance(begin, 1);
      }
    });
    begin = local_end;
  }
  while (begin != end) {
    function(*begin);
    std::advance(begin, 1);
  }
}

template <typename ContainterType, typename FuncType>
void ParallelForEach(const ContainterType &container, FuncType &&function) {
  ParallelForEach(std::begin(container), std::end(container),
                  std::move(function));
}

} // namespace rainbow
