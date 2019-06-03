#pragma once

#include <type_traits>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "intersection.hpp"

namespace rainbow {

class Camera {
 public:
  void ComputeViewRays(glm::uvec2 resolution, std::vector<Ray>* ray_buffer);
  inline std::vector<Ray> ComputeViewRays(glm::uvec2 resolution) {
    std::vector<Ray> rays;
    ComputeViewRays(resolution, &rays);
    return rays;
  }

 private:
  glm::vec3 position_ = glm::vec3{0.0f, 0.0f, -10.0f};
  float yaw_ = 0.0f;
  float pitch_ = 0.0f;
  float vertical_field_of_view_ = glm::half_pi<float>();
  float horizontal_field_of_view_ = std::numeric_limits<float>::quiet_NaN();
};

}  // namespace rainbow
