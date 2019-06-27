#include "rainbow/camera.hpp"

#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/mat4x4.hpp>

namespace rainbow {

void Camera::Rotate(float yaw, float pitch) {
  yaw_ += yaw;
  pitch_ += pitch;
}

void Camera::GetAxisVectors(Vector3* right, Vector3* up,
                            Vector3* forward) const {
  glm::mat4 rotation_matrix = glm::yawPitchRoll(yaw_, pitch_, 0.0f);
  if (right != nullptr) {
    for (int i = 0; i < 3; ++i) {
      (*right)[i] = rotation_matrix[0][i];
    }
  }
  if (up != nullptr) {
    for (int i = 0; i < 3; ++i) {
      (*up)[i] = rotation_matrix[1][i];
    }
  }
  if (forward != nullptr) {
    for (int i = 0; i < 3; ++i) {
      (*forward)[i] = rotation_matrix[2][i];
    }
  }
}

}  // namespace rainbow
