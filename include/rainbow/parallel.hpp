#pragma once

#include <cassert>
#include <vector>
#include "rainbow/integral_range.hpp"
#include "rainbow/thread_pool.hpp"

namespace rainbow {

template <typename IteratorType, typename FuncType>
void ParallelForEach(IteratorType begin, IteratorType end,
                     FuncType&& function) {
  static_assert(std::is_invocable_v<FuncType, decltype(*begin)>);
  using ResultType = std::invoke_result_t<FuncType, decltype(*begin)>;

  const auto worker_count = default_thread_pool.GetWorkerCount();
  const auto distance = std::distance(begin, end);
  const auto elements_per_thread = distance / worker_count;
  auto remaining_elements = distance - elements_per_thread * worker_count;

  std::vector<std::future<void>> results;
  results.reserve(worker_count);
  for (size_t i = 0; i < default_thread_pool.GetWorkerCount(); ++i) {
    auto local_end = std::next(begin, elements_per_thread);
    if (remaining_elements > 0) {
      std::advance(local_end, 1);
      --remaining_elements;
    }
    results.emplace_back(
        default_thread_pool.EnqueueTask([function, begin, local_end]() mutable {
          while (begin != local_end) {
            function(*begin);
            std::advance(begin, 1);
          }
        }));
    begin = local_end;
  }
  assert(begin == end);
  for (auto& result : results) {
    result.get();
  }
}

template <typename ContainterType, typename FuncType>
void ParallelForEach(const ContainterType& container, FuncType&& function) {
  ParallelForEach(std::begin(container), std::end(container),
                  std::move(function));
}

template <typename IntegralType, typename FuncType>
void ParallelFor(IntegralType end, FuncType&& function) {
  ParallelForEach(IntegralIterator<IntegralType>{},
                  IntegralIterator<IntegralType>{end}, std::move(function));
}

template <typename IntegralType, typename FuncType>
void ParallelFor(IntegralType begin, IntegralType end, FuncType&& function) {
  ParallelForEach(IntegralIterator<IntegralType>{begin},
                  IntegralIterator<IntegralType>{end}, std::move(function));
}

}  // namespace rainbow
