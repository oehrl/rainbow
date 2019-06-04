#include "timing.hpp"

#include <iostream>

namespace rainbow {

void Timer::Print(Clock::duration duration) {
  std::cout << name_ << ": " << PrettyPrintDuration{duration} << "\n";
}

}  // namespace rainbow
