#include "camera.hpp"

#include <cmath>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>

namespace rainbow {

void Camera::ComputeViewDirections(
    glm::uvec2 resolution, std::vector<glm::vec3>* direction_buffer) const {
  assert(direction_buffer != nullptr);
  const auto directions_count = resolution.x * resolution.y;
  direction_buffer->resize(0);
  direction_buffer->reserve(directions_count);

  // float horizontal_field_of_view;
  // float vertical_field_of_view;
  // if (std::isnan(horizontal_field_of_view_) &&
  //     std::isnan(vertical_field_of_view)) {
  // }

  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 forward;
  GetAxisVectors(&right, &up, &forward);

  for (unsigned y = 0; y < resolution.y; ++y) {
    const float y_normalized =
        static_cast<float>(y) / (resolution.y - 1) - 0.5f;
    for (unsigned x = 0; x < resolution.x; ++x) {
      const float x_normalized =
          static_cast<float>(x) / (resolution.x - 1) - 0.5f;

      const auto ray_direction =
          x_normalized * right + y_normalized * up + forward;

      direction_buffer->push_back(glm::normalize(ray_direction));
    }
  }
}

void Camera::Rotate(float yaw, float pitch) {
  yaw_ += yaw;
  pitch_ += pitch;
}

void Camera::GetAxisVectors(glm::vec3* right, glm::vec3* up,
                            glm::vec3* forward) const {
  glm::mat4 rotation_matrix = glm::yawPitchRoll(yaw_, pitch_, 0.0f);
  if (right != nullptr) {
    *right = rotation_matrix[0];
  }
  if (up != nullptr) {
    *up = rotation_matrix[1];
  }
  if (forward != nullptr) {
    *forward = rotation_matrix[2];
  }
}

}  // namespace rainbow
