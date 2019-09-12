#include <iostream>

#include <glob/graphics.h>
#include <entity/registry.hpp>
#include "PrintPositionSystem.h"
#include "collision_system.h"
#include "physics_system.h"
#include "collision.h"
#include "ball_component.h"

#include "util/meminfo.h"
//#include <glad/glad.h>

int main(unsigned argc, char **argv) {
  std::cout << "Hello World!*!!!111\n";

  std::cout << "Test fr�n development\n";

  entt::registry registry;

  auto entity = registry.create();
  registry.assign<BallComponent>(entity, true, false);
  registry.assign<Velocity>(entity, glm::vec3(1.0f, 0.0f, 0.0f));
  registry.assign<physics::Sphere>(entity, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);

  entity = registry.create();
  registry.assign<Velocity>(entity, glm::vec3(.0f, .0f, .0f));
  registry.assign<physics::OBB>(
      entity, glm::vec3(5.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.f, 0.f),
      glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), 1.f, 1.f, 1.f);

  entity = registry.create();
  registry.assign<physics::Arena>(entity, -10.f, 10.f, 0.f, 5.f, -10.f, 10.f);
  registry.assign<Velocity>(entity, glm::vec3(.0f, .0f, .0f));

  for (int i = 0; i < 20; i++) {
    UpdatePhysics(registry, 1.0f);
    UpdateCollisions(registry);
  }

  std::cout << "Test fr�n development2 " << glob::GraphicsTest() << "\n";
  std::cout << "RAM usage: " << util::MemoryInfo::GetInstance().GetUsedRAM() << " MB\n";
  std::cout << "VRAM usage: " << util::MemoryInfo::GetInstance().GetUsedVRAM() << " MB\n";
  std::cin.ignore();
  return EXIT_SUCCESS;
}