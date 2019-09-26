#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <NetAPI/socket/tcpclient.hpp>
#include <NetAPI/socket/Client.hpp>
#include <entt.hpp>
#include <unordered_map>
#include <glob/graphics.hpp>
#include "shared/shared.hpp"

class Engine {
 public:
  Engine();
  ~Engine();
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  void Init();

  void Update(float dt);

  void UpdateNetwork();

  void Render();

 private:
  void UpdateSystems(float dt);

  void CreatePlayer(PlayerID id);
  
  NetAPI::Socket::Client client;
  entt::registry registry_;

  PlayerID my_id = -1;

  std::unordered_map<int, int> keybinds_;
  std::unordered_map<int, int> mousebinds_;

  glob::Font2DHandle font_test_ = 0;
};

#endif  // ENGINE_HPP_