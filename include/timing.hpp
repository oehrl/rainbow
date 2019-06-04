#pragma once

#include <chrono>
#include <ostream>
#include <string_view>
#include <type_traits>

namespace rainbow {

template <typename DurationType>
class PrettyPrintDuration;

template <typename DurationType>
std::ostream& operator<<(std::ostream& os,
                         const PrettyPrintDuration<DurationType>& ppd);

template <typename DurationType>
class PrettyPrintDuration final {
 public:
  explicit PrettyPrintDuration(DurationType duration)
      : duration_{std::move(duration)} {}

  friend std::ostream& operator<<<>(
      std::ostream& os, const PrettyPrintDuration<DurationType>& ppd);

 private:
  DurationType duration_;
};

template <typename DurationType>
std::ostream& operator<<(std::ostream& os,
                         const PrettyPrintDuration<DurationType>& ppd) {
  const double HOURS_PER_SECOND = 1.0 / (60.0 * 60.0);
  const double MINUTES_PER_SECOND = 1.0 / 60.0;

  const auto seconds =
      std::chrono::duration_cast<std::chrono::duration<double>>(ppd.duration_)
          .count();

  const double hours = seconds / (60.0 * 60.0);
  if (hours > 1) {
    os << hours << "h";
    return os;
  }

  const double minutes = hours * 60.0;
  if (minutes > 1) {
    os << minutes << "m";
    return os;
  }

  if (seconds > 1) {
    os << seconds << "s";
    return os;
  }

  const double milliseconds = seconds * 1000.0;
  if (milliseconds > 1) {
    os << milliseconds << "ms";
    return os;
  }

  const double microseconds = milliseconds * 1000.0;
  if (microseconds > 1) {
    os << microseconds << "us";
    return os;
  }

  const double nanoseconds = microseconds * 1000.0;
  os << nanoseconds << "ns";
  return os;
}

class Timer {
 protected:
  using Clock = std::chrono::steady_clock;

  explicit inline Timer(std::string_view name) : name_{name} {}
  Timer(const Timer&) = delete;
  Timer(Timer&&) = delete;
  ~Timer() = default;

  Timer& operator=(const Timer&) = delete;
  Timer& operator=(Timer&&) = delete;

  void Print(Clock::duration duration);

 private:
  std::string_view name_;
};

class ScopeTimer final : public Timer {
 public:
  explicit inline ScopeTimer(std::string_view name)
      : Timer{name}, t0_{Clock::now()} {}
  ~ScopeTimer() { Print(Clock::now() - t0_); }

 private:
  Clock::time_point t0_;
};

class SectionTimer final : public Timer {
 public:
  explicit inline SectionTimer(std::string_view name) : Timer{name} {}

  template <typename FuncType>
  inline void operator+=(FuncType&& function) {
    static_assert(std::is_invocable_v<FuncType>);

    const auto t0 = Clock::now();
    function();
    const auto t1 = Clock::now();
    Print(t1 - t0);
  }
};

}  // namespace rainbow

#define RAINBOW_TIME_FUNCTION() \
  ::rainbow::ScopeTimer rainbow_time_function { __PRETTY_FUNCTION__ }

#define RAINBOW_TIME_SECTION(name) ::rainbow::SectionTimer{name} += [&]()
