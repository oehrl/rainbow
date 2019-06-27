#pragma once

#include <type_traits>
#include <vector>

#include "rainbow/constants.hpp"
#include "rainbow/intersection.hpp"

namespace rainbow {

class Camera {
 public:
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
  Vector3 position_ = Vector3{0.0f, 1.0f, 3.0f};
  float yaw_ = Pi<float>();
  float pitch_ = 0.0f;
  float vertical_field_of_view_ = PiOverTwo<float>();
  float horizontal_field_of_view_ = NotANumber<float>();
};

}  // namespace rainbow
