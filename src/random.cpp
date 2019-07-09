#include <ctime>
#include "rainbow/random.hpp"

namespace rainbow {

namespace {
std::mt19937 CreateDefaultRandomNumberEngine() {
  try {
    std::random_device random_device{};
    return std::mt19937{random_device()};
  } catch (const std::exception&) {
    return std::mt19937{
        static_cast<std::mt19937::result_type>(std::time(NULL))};
  }
}
}  // namespace

std::mt19937 default_random_number_engine = CreateDefaultRandomNumberEngine();

}  // namespace rainbow
