#ifndef PROJECTILE_COMPONENT_HPP_
#define PROJECTILE_COMPONENT_HPP_

#include "shared/shared.hpp"

struct ProjectileComponent {
  ProjectileID projectile_id;
  long creator = TEAM_NONE;
};

#endif  // !PROJECTLIE_COMPONENT_HPP_