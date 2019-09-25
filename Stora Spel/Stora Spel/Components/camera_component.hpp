#ifndef CAMERA_COMPONENT_HPP_
#define CAMERA_COMPONENT_HPP_

#include <glm/glm.hpp>
#include "../util/transform_helper.hpp"

const float pi = 3.14159265358979323846f;

struct CameraComponent {
  // relative to transform
  glm::vec3 offset = glm::vec3(0, 0, 0);
  float yaw = 0;
  float pitch = 0;
  bool clamped = true;

  // prevent camera from spinning around an axis infinitely if clamped.
  void ClampAngles() {
    if (clamped) {
      pitch = glm::clamp(pitch, glm::radians(-89.0f), glm::radians(89.0f));
      if (yaw > pi * 2.f || yaw < pi * -2.f) {
        yaw = 0.0f;
      }
    }
  }
  void SetAngles(float _yaw, float _pitch) {
    yaw = _yaw;
    pitch = _pitch;
    ClampAngles();
    //cam->SetAnglesViaRadians(yaw, pitch);
  }
  void AddAngles(float _yaw, float _pitch) {
    yaw += _yaw;
    pitch -= _pitch;
    ClampAngles();
    //cam->SetAnglesViaRadians(yaw, pitch);
  }
  glm::vec3 LookDirection() {
    return transform_helper::DirVectorFromRadians(yaw, pitch);
  }
};

#endif  // CAMERA_COMPONENT_HPP_