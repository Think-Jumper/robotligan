#include <iostream>
#include <entt.hpp>
#include <NetAPI/networkTest.hpp>
#include <NetAPI/socket/server.hpp>
#include <NetAPI/socket/tcpclient.hpp>
#include <glm/gtx/transform.hpp>
#include <glob/graphics.hpp>
#include <glob/window.hpp>
#include <entity/registry.hpp>

#include "ability_component.hpp"
#include "collision_system.hpp"
#include "physics_system.hpp"
#include "collision.hpp"
#include "ball_component.hpp"
#include "print_position_system.hpp"
#include "player_controller_system.hpp"
#include "ability_controller_system.hpp"
#include "collision_temp_debug_system.h"
#include <GLFW/glfw3.h> //NTS: This one must be included after certain other things
#include "util/input.hpp"
#include "util/meminfo.hpp"
#include "util/timer.hpp"
#include "util/meminfo.hpp"

#include "util/global_settings.hpp"

#include <thread>
#include <chrono>

void init() {
  glob::window::Create();
  glob::Init();
  Input::Initialize();
}

void updateSystems(entt::registry *reg, float dt) {
  collision_debug::Update(*reg);
  player_controller::Update(*reg, dt);
  ability_controller::Update(*reg, dt);
 
  UpdatePhysics(*reg, dt);
  UpdateCollisions(*reg);
  
}

int main(unsigned argc, char **argv) {
  Timer timer;

  //Tell the GlobalSettings class to do a first read from the settings file
  //NTS: Do this in init()? Why is init not first in main()?
  GlobalSettings::Access()->UpdateValuesFromFile();

  std::cout << "Hello World!*!!!111\n";

  std::cout << "Test fr�n development\n";
  entt::registry registry;

  auto entity = registry.create();
  registry.assign<BallComponent>(entity, true, false);
  registry.assign<PhysicsComponent>(entity, glm::vec3(1.0f, 0.0f, 0.0f));
  registry.assign<physics::Sphere>(entity, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);

  entity = registry.create();

  entity = registry.create();
  registry.assign<physics::Arena>(entity, -10.f, 10.f, 0.f, 5.f, -10.f, 10.f);
  registry.assign<PhysicsComponent>(entity, glm::vec3(.0f, .0f, .0f));

  glob::window::Create();
  glob::Init();
  init();  // Initialize everything

  glob::ModelHandle model_h =
      glob::GetModel("assets/Mech/Mech_humanoid_posed_unified_AO.fbx");
  glob::ModelHandle model_h2 = glob::GetModel("assets/Ball/Ball.fbx");
  auto avatar = registry.create();  // this is the player avatar
  registry.assign<CameraComponent>(
      avatar, (Camera *)glob::GetCamera(),
      glm::vec3(0, 1, 0));  // get the camera pointer from glob renderer
  registry.assign<PlayerComponent>(avatar);
  registry.assign<TransformComponent>(avatar, glm::vec3(-9.f, 4.f, 0.f),
                                      glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
  registry.assign<PhysicsComponent>(avatar, glm::vec3(.0f, .0f, .0f));
  registry.assign<physics::OBB>(
      avatar, glm::vec3(5.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.f, 0.f),
      glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), 1.f, 1.f, 1.f);
  //registry.assign<AbilityComponent>(avatar);
  registry.assign<AbilityComponent>(
	  avatar,																//Entity
	  SUPER_STRIKE,															//Primary abiliy id
	  false,																//Use primary ability
	  GlobalSettings::Access()->ValueOf("ABILITY_SUPER_STRIKE_COOLDOWN"),	//Primary ability cooldown
      0.0f,																	//Remaining cooldown
	  NULL_ABILITY,															//Secondary ability
	  false,																//Use secondary ability
	  false,																//Shoot
	  0.0f																	//Remaining shoot cooldown
	  );

  timer.Restart();
  float dt = 0.0f;
  while (!glob::window::ShouldClose()) {
    dt = timer.Restart();
    Input::Reset();
    // tick
    // render

	//Check if the keys for global settings are pressed
    if (Input::IsKeyPressed(GLFW_KEY_U)) {
      // Update contents of GlobalSettings from file
      GlobalSettings::Access()->UpdateValuesFromFile();
      // Write contents of GlobalSettings to console
      GlobalSettings::Access()->WriteMapToConsole();
    }

    glob::Submit(model_h, glm::translate(glm::vec3(0)) *
                              glm::rotate(0.5f, glm::vec3(0, 1, 0)));
    glob::Submit(model_h2,
                 glm::translate(glm::vec3(0, 9, 3.f * 0)));
    updateSystems(&registry, dt);

    glob::Render();
    glob::window::Update();
  }

  glob::window::Cleanup();
  std::cout << "RAM usage: " << util::MemoryInfo::GetInstance().GetUsedRAM()
            << " MB\n";
  std::cout << "VRAM usage: " << util::MemoryInfo::GetInstance().GetUsedVRAM()
            << " MB\n";

  std::cout << "WSA is initialized? " << std::boolalpha << NetAPI::Initialization::WinsockInitialized() << std::endl;

  std::cin.ignore();
  return EXIT_SUCCESS;
}