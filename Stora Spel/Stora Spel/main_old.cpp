/*


HOW TO USE: 
Set "Include in project" setting to FALSE for: engine.hpp, engine.cpp, main.cpp, entitycreation.hpp and entitycreation.cpp
Set "Include in project" setting to TRUE for: main_old.cpp

*/

#include <NetAPI/networkTest.hpp>
#include <NetAPI/socket/server.hpp>
#include <NetAPI/socket/tcpclient.hpp>
#undef min
#undef max
#include <entity/registry.hpp>
#include <entt.hpp>
#include <glm/gtx/transform.hpp>
#include <glob/graphics.hpp>
#include <glob/window.hpp>
#include <iostream>

#include <entity/registry.hpp>
#include "ability_component.hpp"
#include "ability_controller_system.hpp"
#include "ball_component.hpp"
#include "button_component.hpp"
#include "collision.hpp"
#include "collision_system.hpp"
#include "model_component.hpp"
#include "physics_system.hpp"
#include "player_controller_system.hpp"
#include "print_position_system.hpp"
#include "render_system.hpp"
#include "transform_component.hpp"
#include "button_system.hpp"

#include <GLFW/glfw3.h>  //NTS: This one must be included after certain other things
#include "collision_temp_debug_system.h"
#include <GLFW/glfw3.h> //NTS: This one must be included after certain other things.
#include "util/input.hpp"
#include "util/timer.hpp"
#include "util/meminfo.hpp"

#include "util/global_settings.hpp"

#include "entitycreation.hpp"

#include <chrono>
#include <thread>

// NTS: Move into game engine class once that exists
void CreateEntities(entt::registry& registry, glm::vec3* in_pos_arr,
                    unsigned int in_num_pos);
void ResetEntities(entt::registry& registry, glm::vec3* in_pos_arr,
                   unsigned int in_num_pos);
void AddBallComponents(entt::registry& registry, entt::entity& entity,
                       glm::vec3 in_pos, glm::vec3 in_vel);
void AddArenaComponents(entt::registry& registry, entt::entity& entity);
void AddPlayerComponents(entt::registry& registry, entt::entity& entity);
void AddRobotComponents(entt::registry& registry, entt::entity& entity,
                        glm::vec3 in_pos);


entt::registry registry;
bool control = true;

void init() {
  glob::window::Create();
  glob::Init();
  Input::Initialize();
}

void updateSystems(entt::registry* reg, float dt) {
  // collision_debug::Update(*reg);
  player_controller::Update(*reg, dt);

  UpdatePhysics(*reg, dt);
  UpdateCollisions(*reg);
  ability_controller::Update(*reg, dt);
  button_system::Update(*reg);

  auto view = reg->view<CameraComponent, TransformComponent>();
  for (auto v : view) {
    auto &cam_c = reg->get<CameraComponent>(v);
    auto &trans_c = reg->get<TransformComponent>(v);
    cam_c.cam->SetPosition(trans_c.position + trans_c.rotation*cam_c.offset);
  }

  //temporary
  if (Input::IsKeyPressed(GLFW_KEY_M)) {
    control = !control;
    glob::window::SetMouseLocked(control);
  }
  //temporary

  RenderSystem(*reg);
}

int main(unsigned argc, char** argv) {
  init();  // Initialize everything
  Timer timer;

  // Tell the GlobalSettings class to do a first read from the settings file
  GlobalSettings::Access()->UpdateValuesFromFile();

  // Create a registry and create some entities in it
  entt::registry registry;
  glm::vec3 start_positions[3] = {
      glm::vec3(5.f, 0.f, 0.f),   // Ball
      glm::vec3(-9.f, 4.f, 0.f),  // Player
      glm::vec3(0.f, 0.f, 0.f)    // Others
  };
  CreateEntities(registry, start_positions, 3);

  // Create light
  auto light = registry.create();
  registry.assign<LightComponent>(light, glm::vec3(0.3f, 0.3f, 1.0f), 15.f,
                                  0.2f);
  registry.assign<TransformComponent>(light, glm::vec3(12.f, -4.f, 0.f),
                                      glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f));

  light = registry.create();
  registry.assign<LightComponent>(light, glm::vec3(1.f, 0.3f, 0.3f), 15.f, 0.f);
  registry.assign<TransformComponent>(light, glm::vec3(-12.f, -4.f, 0.f),
                                      glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f));

  // Create text
  glob::Font2DHandle font_test = glob::GetFont("assets/fonts/fonts/comic.ttf");
  glob::Font2DHandle font_test2 =
      glob::GetFont("assets/fonts/fonts/ariblk.ttf");
  glob::Font2DHandle font_test3 =
      glob::GetFont("assets/fonts/fonts/OCRAEXT_2.TTF");

  // Create 2D element
  glob::E2DHandle e2D_test = glob::GetE2DItem("assets/GUI_elements/point_table.png");
  glob::E2DHandle e2D_test2 =
      glob::GetE2DItem("assets/GUI_elements/Scoreboard_V1.png");

  // Create GUI elementds
  glob::GUIHandle gui_test =
      glob::GetGUIItem("assets/GUI_elements/Scoreboard_V1.png");
  glob::GUIHandle gui_teamscore =
      glob::GetGUIItem("assets/GUI_elements/point_table.png");
  glob::GUIHandle gui_stamina_base =
      glob::GetGUIItem("assets/GUI_elements/stamina_bar_base.png");
  glob::GUIHandle gui_stamina_fill =
      glob::GetGUIItem("assets/GUI_elements/stamina_bar_fill.png");
  glob::GUIHandle gui_stamina_icon =
      glob::GetGUIItem("assets/GUI_elements/stamina_bar_icon.png");
  glob::GUIHandle gui_quickslots =
      glob::GetGUIItem("assets/GUI_elements/koncept_abilities.png");

  float time = 0.f;
  timer.Restart();
  float dt = 0.0f;
  while (!glob::window::ShouldClose()) {
    dt = timer.Restart();
    // std::cout << 1.0f / dt << std::endl;
    Input::Reset();
    // tick
    //if (Input::IsKeyDown(GLFW_KEY_K)) {
    //  //auto& c = registry.get<CameraComponent>(avatar);
    //  //c.offset.x += 0.01f;
    //  //std::cout << "Camera: " << c.offset.x << std::endl;
    //  registry.get<PhysicsComponent>(ball).velocity.x += 10;
    //  std::cout << registry.get<PhysicsComponent>(ball).velocity.x << std::endl;
    //}
    //if (Input::IsKeyDown(GLFW_KEY_L)) {
    //  //auto &c = registry.get<CameraComponent>(avatar);
    //  //c.offset.x -= 0.01f;
    //  //std::cout << "Camera: " << c.offset.x << std::endl;
    //  registry.get<PhysicsComponent>(ball).is_airborne = false;
    //  registry.get<TransformComponent>(ball).position.y += 0.1f;
    //}
    //if (Input::IsKeyDown(GLFW_KEY_O)) {
    //  auto &c = registry.get<CameraComponent>(avatar);
    //  c.offset.y += 0.01f;
    //  std::cout << "Camera y: " << c.offset.y << std::endl;
    //}
    //if (Input::IsKeyDown(GLFW_KEY_P)) {
    //  auto &c = registry.get<CameraComponent>(avatar);
    //  c.offset.y -= 0.01f;
    //  std::cout << "Camera y: " << c.offset.y << std::endl;
    //}
    //if (Input::IsKeyDown(GLFW_KEY_U)) {
    //  auto &c = registry.get<CameraComponent>(avatar);
    //  c.offset.z += 0.01f;
    //  std::cout << "Camera z: " << c.offset.z << std::endl;
    //}
    //if (Input::IsKeyDown(GLFW_KEY_I)) {
    //  auto &c = registry.get<CameraComponent>(avatar);
    //  c.offset.z -= 0.01f;
    //  std::cout << "Camera z: " << c.offset.z << std::endl;
    //}
    // render

    // Check if the keys for global settings are pressed
    if (Input::IsKeyPressed(GLFW_KEY_U)) {
      // Update contents of GlobalSettings from file
      GlobalSettings::Access()->UpdateValuesFromFile();
      // Write contents of GlobalSettings to console
      GlobalSettings::Access()->WriteMapToConsole();
    }

    // Reset positions and velocities
    if (Input::IsKeyPressed(GLFW_KEY_K)) {
      // K as in kickoff
      ResetEntities(registry, start_positions, 3);
    }

    updateSystems(&registry, dt);

	// Submit 2D Element TEST
    glob::Submit(e2D_test, glm::vec3(10.5f, 1.0f, 0.0f), 2, -90.0f, glm::vec3(0, 1, 0));
    glob::Submit(e2D_test, glm::vec3(-10.5f, 1.0f, 0.0f), 2, 90.0f,
                 glm::vec3(0, 1, 0));
    glob::Submit(e2D_test2, glm::vec3(0.0f, 1.0f, -7.0f), 7, 0.0f,
                 glm::vec3(1));

	// Show statistics TEST
    if (Input::IsKeyDown(GLFW_KEY_TAB)) {
      glob::Submit(gui_test, glm::vec2(285, 177), 0.6, 100);
    }

    // Show GUI TEST
	// Temp Update of stamina bar
    float stam_len = 0.0f;
    registry.view<PlayerComponent>().each(
        [&](auto entity, PlayerComponent& player_c) {
          stam_len = player_c.energy_current;
        });
    glob::Submit(gui_stamina_base, glm::vec2(0, 5), 0.85, 100);
    glob::Submit(gui_stamina_fill, glm::vec2(7, 12), 0.85, stam_len);
    glob::Submit(gui_stamina_icon, glm::vec2(0, 5), 0.85, 100);
    glob::Submit(gui_quickslots, glm::vec2(7, 50), 0.3, 100);
    glob::Submit(gui_teamscore, glm::vec2(497, 648), 1, 100);

	// Submit text TEST
    //glob::Submit(font_test3, glm::vec2(626, 695), 36, "5:33",		// Replace string w/ actual match time
    //             glm::vec4(0.89, 0.82, 0.21, 1));
    //glob::Submit(font_test3, glm::vec2(582, 705), 72, "0",			// Replace string with Red team's actual score
    //             glm::vec4(0, 0.26, 1, 1));
    //glob::Submit(font_test3, glm::vec2(705, 705), 72, "12",			// Replace string with Blue team's actual score
    //             glm::vec4(1, 0, 0, 1));

    glob::Render();
    glob::window::Update();
  }

  glob::window::Cleanup();
  std::cout << "RAM usage: " << util::MemoryInfo::GetInstance().GetUsedRAM()
            << " MB\n";
  std::cout << "VRAM usage: " << util::MemoryInfo::GetInstance().GetUsedVRAM()
            << " MB\n";

  std::cout << "WSA is initialized? " << std::boolalpha
            << NetAPI::Initialization::WinsockInitialized() << std::endl;

  std::cin.ignore();
  return EXIT_SUCCESS;
}

void CreateEntities(entt::registry& registry, glm::vec3* in_pos_arr,
                    unsigned int in_num_pos) {
  // Create one ball entity and add components
  auto ball_entity = registry.create();
  AddBallComponents(registry, ball_entity, in_pos_arr[0], glm::vec3(0.0f));

  // Create one map entity and add components
  auto arena_entity = registry.create();
  AddArenaComponents(registry, arena_entity);

  // Create one player entity and add components
  auto avatar_entity = registry.create();
  AddPlayerComponents(registry, avatar_entity);
  AddRobotComponents(registry, avatar_entity, in_pos_arr[1]);

  // Create other robots and add components
  for (unsigned int i = 2; i < in_num_pos; i++) {
    auto other_robot_entity = registry.create();
    AddRobotComponents(registry, other_robot_entity, in_pos_arr[i]);
  }

  // test button
  auto button_entity = registry.create();
  registry.assign<ButtonComponent>(
      button_entity, glob::GetFont("assets/fonts/fonts/Marvel-Regular.ttf"));
  registry.assign<TransformComponent>(button_entity, glm::vec3(500, 500, 0));
  ButtonComponent& b_c = registry.get<ButtonComponent>(button_entity);
  b_c.text = "RESET";
  b_c.button_func = [&]() {
    glm::vec3 start_positions[3] = {
        glm::vec3(5.f, 0.f, 0.f),   // Ball
        glm::vec3(-9.f, 4.f, 0.f),  // Player
        glm::vec3(0.f, 0.f, 0.f)    // Others
    };
    ResetEntities(registry, start_positions, 3);
  };
}

void ResetEntities(entt::registry& registry, glm::vec3* in_pos_arr,
                   unsigned int in_num_pos) {
  // Get everything with a physics component and a transform component
  auto reset_view = registry.view<PhysicsComponent, TransformComponent>();

  unsigned int pos_counter = 0;

  for (auto entity : reset_view) {
    PhysicsComponent& physics_component =
        reset_view.get<PhysicsComponent>(entity);
    TransformComponent& transform_component =
        reset_view.get<TransformComponent>(entity);

    physics_component.velocity = glm::vec3(0.0f);
    physics_component.is_airborne = true;

    transform_component.position = in_pos_arr[pos_counter];
    pos_counter++;

    if (pos_counter >= in_num_pos) {
      GlobalSettings::Access()->WriteError("main.cpp", "ResetEntities()",
                                           "Counter out of scope");
    }
  }
}

void AddBallComponents(entt::registry& registry, entt::entity& entity,
                       glm::vec3 in_pos, glm::vec3 in_vel) {
  // Prepare hard-coded values
  bool ball_is_real = true;
  bool ball_is_airborne = true;
  float ball_friction = 0.0f;
  float ball_radius = 1.0f;
  glm::vec3 zero_vec = glm::vec3(0.0f);
  glm::vec3 ball_scale = glm::vec3(1.0f);
  glob::ModelHandle model_ball = glob::GetModel("assets/Ball/Ball.fbx");

  // Add components for a ball
  registry.assign<BallComponent>(entity, ball_is_real, ball_is_airborne);
  registry.assign<ModelComponent>(entity, model_ball);
  registry.assign<PhysicsComponent>(entity, in_vel, ball_is_airborne,
                                    ball_friction);
  registry.assign<TransformComponent>(entity, in_pos, zero_vec, ball_scale);

  // Add a hitbox
  registry.assign<physics::Sphere>(entity, zero_vec, ball_radius);
}

void AddArenaComponents(entt::registry& registry, entt::entity& entity) {
  // Prepare hard-coded values
  // Scale on the hitbox for the map
  float v1 = 7.171f;
  float v2 = 10.6859;  // 13.596f;
  float v3 = 5.723f;
  glm::vec3 zero_vec = glm::vec3(0.0f);
  glm::vec3 arena_scale = glm::vec3(1.0f);
  glob::ModelHandle model_arena =
      glob::GetModel("assets/Map_rectangular/map_rextangular.fbx");

  // Add components for an arena
  registry.assign<ModelComponent>(entity, model_arena);
  registry.assign<TransformComponent>(entity, zero_vec, zero_vec, arena_scale);

  // Add a hitbox
  registry.assign<physics::Arena>(entity, -v2, v2, -v3, v3, -v1, v1);
  auto md = glob::GetMeshData(model_arena);
  glm::mat4 matrix = glm::rotate(-90.f * glm::pi<float>() / 180.f, glm::vec3(1.f, 0.f, 0.f))
          *glm::rotate(90.f * glm::pi<float>() / 180.f,
                      glm::vec3(0.f, 0.f, 1.f));

  for (auto& v : md.pos) v = matrix * glm::vec4(v,1.f);
  auto& mh = registry.assign<physics::MeshHitbox>(entity, std::move(md.pos),
                                       std::move(md.indices));
  glob::LoadWireframeMesh(model_arena, mh.pos, mh.indices);
}

void AddPlayerComponents(entt::registry& registry, entt::entity& entity) {
  // Prepare hard-coded values
  AbilityID primary_id = SUPER_STRIKE;
  AbilityID secondary_id = NULL_ABILITY;
  float primary_cooldown =
      GlobalSettings::Access()->ValueOf("ABILITY_SUPER_STRIKE_COOLDOWN");
  glm::vec3 camera_offset = glm::vec3(0.38f, 0.62f, -0.06f);

  // Add components for a player
  registry.assign<AbilityComponent>(
      entity,            // Entity
      primary_id,        // Primary abiliy id
      false,             // Use primary ability
      primary_cooldown,  // Primary ability cooldown
      0.0f,              // Remaining cooldown
      secondary_id,      // Secondary ability
      false,             // Use secondary ability
      false,             // Shoot
      0.0f               // Remaining shoot cooldown
  );
  registry.assign<CameraComponent>(entity, (Camera*)glob::GetCamera(),
                                   camera_offset);
  registry.assign<PlayerComponent>(entity);
  registry.assign<LightComponent>(entity, glm::vec3(1, 1, 1), 3.f, 0.f);
}

void AddRobotComponents(entt::registry& registry, entt::entity& entity,
                        glm::vec3 in_pos) {
  // Prepare hard-coded values
  bool robot_is_airborne = true;
  float robot_friction = 0.0f;
  float coeff_x_side = (11.223f - (-0.205f));
  float coeff_y_side = (8.159f - (-10.316f));
  float coeff_z_side = (10.206f - (-1.196f));
  glm::vec3 zero_vec = glm::vec3(0.0f);
  glm::vec3 alter_scale =
      glm::vec3(5.509f - 5.714f * 2.f, -1.0785f, 4.505f - 5.701f * 1.5f);
  glm::vec3 character_scale = glm::vec3(0.1f);
  glob::ModelHandle robot_model =
      glob::GetModel("assets/Mech/Mech_humanoid_posed_unified_AO.fbx");

  // Add components for a robot
  registry.assign<ModelComponent>(entity, robot_model,
                                  alter_scale * character_scale);
  registry.assign<PhysicsComponent>(entity, zero_vec, robot_is_airborne,
                                    robot_friction);
  registry.assign<TransformComponent>(entity, in_pos, zero_vec,
                                      character_scale);

  // Add a hitbox
  registry.assign<physics::OBB>(
      entity,
      alter_scale * character_scale,            // Center
      glm::vec3(1.f, 0.f, 0.f),                 //
      glm::vec3(0.f, 1.f, 0.f),                 // Normals
      glm::vec3(0.f, 0.f, 1.f),                 //
      coeff_x_side * character_scale.x * 0.5f,  //
      coeff_y_side * character_scale.y * 0.5f,  // Length of each plane
      coeff_z_side * character_scale.z * 0.5f   //
  );
}