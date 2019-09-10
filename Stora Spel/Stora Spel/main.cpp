#include <iostream>

#include <glob/graphics.h>
#include <entt.hpp>
#include "PrintPositionSystem.h"
#include "util/meminfo.h"
//#include <glad/glad.h>

int main(unsigned argc, char **argv) {
  std::cout << "Hello World!*!!!111\n";

  std::cout << "Test fr�n development\n";

  entt::registry registry;

  auto entity = registry.create();
  registry.assign<Position>(entity, 1.0f, 2.0f, 3.0f);
  registry.assign<Velocity>(entity, 3.0f, 2.0f, 1.0f);

  print(registry);

  std::cout << "Test fr�n development2 " << glob::GraphicsTest() << "\n";
  std::cout << "RAM usage: " << util::memoryinfo::getInstance().getRamUsage() << "MB\n";
  std::cout << "VRAM usage: " << util::memoryinfo::getInstance().getUsedVRAM() << "MB\n";
  std::cin.ignore();
  return EXIT_SUCCESS;
}