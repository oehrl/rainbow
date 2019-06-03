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

  inline void Move(const glm::vec3& offset) { position_ += offset; }

  void Rotate(float yaw, float pitch);
  void GetAxisVectors(glm::vec3* right, glm::vec3* up,
                      glm::vec3* forward) const;

  inline glm::vec3 GetRightVector() const {
    glm::vec3 right;
    GetAxisVectors(&right, nullptr, nullptr);
    return right;
  }

  inline glm::vec3 GetUpVector() const {
    glm::vec3 up;
    GetAxisVectors(nullptr, &up, nullptr);
    return up;
  }

  inline glm::vec3 GetForwardVector() const {
    glm::vec3 forward;
    GetAxisVectors(nullptr, nullptr, &forward);
    return forward;
  }

 private:
  glm::vec3 position_ = glm::vec3{0.0f, 1.0f, 10.0f};
  float yaw_ = glm::pi<float>();
  float pitch_ = 0.0f;
  float vertical_field_of_view_ = glm::half_pi<float>();
  float horizontal_field_of_view_ = std::numeric_limits<float>::quiet_NaN();
};

}  // namespace rainbow
