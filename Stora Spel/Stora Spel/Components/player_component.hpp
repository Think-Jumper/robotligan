#ifndef PLAYER_COMPONENT_HPP_
#define PLAYER_COMPONENT_HPP_

struct PlayerComponent {
  int id = 0;
  float walkspeed = 1.f;
  bool no_clip = false;
};

#endif  // PLAYER_COMPONENT_H_