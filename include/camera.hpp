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
  void ComputeViewRays(glm::uvec2 resolution,
                       std::vector<Ray>* ray_buffer) const;
  inline std::vector<Ray> ComputeViewRays(glm::uvec2 resolution) const {
    std::vector<Ray> rays;
    ComputeViewRays(resolution, &rays);
    return rays;
  }

  void Rotate(float yaw, float pitch);

 private:
  glm::vec3 position_ = glm::vec3{0.0f, 1.0f, 10.0f};
  float yaw_ = glm::pi<float>();
  float pitch_ = 0.0f;
  float vertical_field_of_view_ = glm::half_pi<float>();
  float horizontal_field_of_view_ = std::numeric_limits<float>::quiet_NaN();
};

}  // namespace rainbow
