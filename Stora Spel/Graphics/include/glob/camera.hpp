#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#ifdef MAKEDLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera {
 private:
  // Variables for the view matrix
  glm::vec3 position_;  // Camera position

  /*
  float yaw_;    // Yaw is for looking left/right. Tracked in radians
  float pitch_;  // Pitch is for looking up/down. Tracked in radians
  glm::vec3 cam_direction_;  // Vector FROM camera in the direction its looking
  */
  glm::quat orientation_;

  glm::vec3 world_up_;   // Up in the world
  glm::vec3 cam_right_;  // Straight right of the camera
  glm::vec3 cam_up_;     // Direction of the top of the camera

  // Variables for the perpective matrix
  float fov_;
  float nearplane_;
  float farplane_;
  float aspect_;

  // Matrices
  glm::mat4 view_mat_;         // View matrix
  glm::mat4 perspective_mat_;  // Perspective matrix

  // Functions
  // void UpdateDirectionalVectors();
  void UpdateViewMatrix();
  void UpdatePerspectiveMatrix();

 public:
  EXPORT Camera(glm::vec3 in_pos,     // Position where camera is created
                glm::vec3 in_target,  // Point camera is looking
                float in_fov_deg,     // Field of view
                float in_aspect,      // Aspect ratio
                float in_nearplane,   // Near z-plane in view-space
                float in_farplane     // Far z-plane in view-space
  );
  EXPORT Camera();
  EXPORT ~Camera();

  EXPORT glm::vec3 GetLookDir() const {
    return orientation_ * glm::vec3(1, 0, 0);
  }
  EXPORT glm::quat GetOrientation() const { return orientation_; }

  EXPORT glm::vec3 GetPosition() const;
  EXPORT glm::mat4 GetViewPerspectiveMatrix() const;

  EXPORT void SetOrientation(const glm::quat& orientation);
  EXPORT void MoveCamera(glm::vec3 in_vec);
  EXPORT void SetPosition(glm::vec3 in_vec);

  EXPORT glm::mat4 GetViewMatrix() { return view_mat_; }
  EXPORT glm::mat4 GetProjectionMatrix() { return perspective_mat_; }

  glm::vec3 GetUpVector() const {
    return orientation_ * glm::vec3(0.f, 1.f, 0.f);
  }
  glm::vec3 GetDir() const { return orientation_ * glm::vec3(1.f, 0.f, 0.f); }
  EXPORT void SetFov(float val) {
    fov_ = val;
    UpdateViewMatrix();
    UpdatePerspectiveMatrix();
  }

  EXPORT float GetNearPlane() { return nearplane_; }
  EXPORT float GetFarPlane() { return farplane_; }

  EXPORT float GetFov() { return fov_; }
  // EXPORT void LookAtPoint(glm::vec3 in_target);
  /*
  EXPORT void TurnCameraViaDegrees(float in_yaw_deg, float in_pitch_deg);
  EXPORT void SetAnglesViaDegrees(float in_yaw_deg, float in_pitch_deg);
  EXPORT void SetAnglesViaRadians(float in_yaw_rad, float in_pitch_rad);
  EXPORT void TurnCameraViaRadians(float in_yaw_rad, float in_pitch_rad);
  */
};

#endif  // !CAMERA_HPP
