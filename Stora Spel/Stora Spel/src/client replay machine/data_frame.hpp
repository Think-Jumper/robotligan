#ifndef DATA_FRAME_HPP_
#define DATA_FRAME_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ecs/components.hpp>
#include <ecs/components/player_component.hpp>
#include <shared/transform_component.hpp>

#include <glob/graphics.hpp>
//---

class DataFrame {
 private:
 public:
  DataFrame();
  ~DataFrame();

  virtual DataFrame* Clone() = 0;

  virtual bool ThresholdCheck(DataFrame& in_future_df) = 0;
  virtual DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                        unsigned int in_dist_to_point_b,
                                        DataFrame& in_point_b) = 0;
};

//---

class PlayerFrame : public DataFrame {
 protected:
  // Transform values
  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  // Player values (for animations)
  bool pc_sprinting_ = false;
  bool pc_running_ = false;
  bool pc_jumping_ = false;

  glm::vec3 pc_vel_dir_ = glm::vec3(1.f, 0.f, 0.f);
  glm::vec3 pc_look_dir_ = glm::vec3(1.f, 0.f, 0.f);
  glm::vec3 pc_move_dir_ = glm::vec3(1.f, 0.f, 0.f);

  // physics stuff
  glm::vec3 velocity_;

  // Team
  int team_ = TEAM_RED;

 public:
  PlayerFrame();
  PlayerFrame(TransformComponent& in_transform_c, PlayerComponent& in_player_c,
              PhysicsComponent& in_phys_c, int player_team);
  ~PlayerFrame();

  DataFrame* Clone();

  bool ThresholdCheck(DataFrame& in_future_df);
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  void WriteBack(TransformComponent& in_transform_c,
                 PlayerComponent& in_player_c, PhysicsComponent& in_phys_c,
                 int& in_team);
};

//---

class BallFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;
  // glm::vec3 scale_;

  // NTS: See comment in declaration of PlayerFrame
  glm::vec4 trail_color_;

 public:
  BallFrame();
  BallFrame(TransformComponent& in_transform_c, TrailComponent& in_trail_c);
  ~BallFrame();

  DataFrame* Clone();

  bool ThresholdCheck(DataFrame& in_future_df);
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  void WriteBack(TransformComponent& in_transform_c, TrailComponent& in_trail_c);
};

//---

class PickUpFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

 public:
  PickUpFrame();
  PickUpFrame(TransformComponent& in_transform_c);
  ~PickUpFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& in_transform_c);
};

//-----Wall---------------
class WallFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;
  unsigned int team_ = TEAM_RED;
 public:
  WallFrame();
  WallFrame(TransformComponent& trans_c, WallComponent& wall_c);
  ~WallFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& trans_c, WallComponent& wall_c);
};

//---

class ShotFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

  glm::vec4 trail_color_;

 public:
  ShotFrame();
  ShotFrame(TransformComponent& in_transform_c, TrailComponent& in_trail_c);
  ~ShotFrame();

  ShotFrame* Clone();

  bool ThresholdCheck(DataFrame& in_future_df);
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  void WriteBack(TransformComponent& in_transform_c, TrailComponent& in_trail_c);
};

//---

class TeleportShotFrame : public DataFrame {
 protected:
  glm::vec3 position_;

 public:
  TeleportShotFrame();
  TeleportShotFrame(TransformComponent& in_transform_c);
  ~TeleportShotFrame();

  TeleportShotFrame* Clone();

  bool ThresholdCheck(DataFrame& in_future_df);
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  void WriteBack(TransformComponent& in_transform_c);
};

//---

class MissileFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

 public:
  MissileFrame();
  MissileFrame(TransformComponent& in_transform_c);
  ~MissileFrame();

  MissileFrame* Clone();

  bool ThresholdCheck(DataFrame& in_future_df);
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  void WriteBack(TransformComponent& in_transform_c);
};

//---

class ForcePushFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

 public:
  ForcePushFrame();
  ForcePushFrame(TransformComponent& trans_c);
  ~ForcePushFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& trans_c);
};

//---

class MineFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  unsigned int team_;

 public:
  MineFrame();
  MineFrame(TransformComponent& trans_c, MineComponent& mine_c);
  ~MineFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& trans_c, MineComponent& mine_c);
};

//---

class BlackholeFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

 public:
  BlackholeFrame();
  BlackholeFrame(TransformComponent& trans_c);
  ~BlackholeFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& trans_c);
};

//---

class HookFrame : public DataFrame {
 protected:
  glm::vec3 position_;
  glm::quat rotation_;

  EntityID owner_;

 public:
  HookFrame();
  HookFrame(TransformComponent& trans_c, HookComponent& hook_c);
  ~HookFrame();

  DataFrame* Clone();
  DataFrame* InterpolateForward(unsigned int in_dist_to_target,
                                unsigned int in_dist_to_point_b,
                                DataFrame& in_point_b);
  bool ThresholdCheck(DataFrame& in_future_df);
  void WriteBack(TransformComponent& trans_c, HookComponent& hook_c);
};

#endif  // DATA_FRAME_HPP_
