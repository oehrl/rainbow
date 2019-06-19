#pragma once

#include <type_traits>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "rainbow/intersection.hpp"

namespace rainbow {

class Camera {
 public:
  void ComputeViewDirections(glm::uvec2 resolution,
                             std::vector<Vector3>* ray_buffer) const;
  inline std::vector<Vector3> ComputeViewDirections(
      glm::uvec2 resolution) const {
    std::vector<Vector3> directions;
    ComputeViewDirections(resolution, &directions);
    return directions;
  }

  inline void Move(const Vector3& offset) { position_ += offset; }
  inline Vector3 GetPosition() const { return position_; }

  void Rotate(float yaw, float pitch);
  void GetAxisVectors(Vector3* right, Vector3* up, Vector3* forward) const;

  inline Vector3 GetRightVector() const {
    Vector3 right;
    GetAxisVectors(&right, nullptr, nullptr);
    return right;
  }

  inline Vector3 GetUpVector() const {
    Vector3 up;
    GetAxisVectors(nullptr, &up, nullptr);
    return up;
  }

  inline Vector3 GetForwardVector() const {
    Vector3 forward;
    GetAxisVectors(nullptr, nullptr, &forward);
    return forward;
  }

 private:
  Vector3 position_ = Vector3{0.0f, 1.0f, 10.0f};
  float yaw_ = glm::pi<float>();
  float pitch_ = 0.0f;
  float vertical_field_of_view_ = glm::half_pi<float>();
  float horizontal_field_of_view_ = std::numeric_limits<float>::quiet_NaN();
};

}  // namespace rainbow
