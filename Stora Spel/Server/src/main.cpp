#define NOMINMAX
#include <iostream>

#include "util/timer.hpp"
#include "gameserver.hpp"

int main(unsigned argc, char** argv) {
  std::cout << "DEBUG: Starting Server" << std::endl;

  Timer timer;
  double accum = 0;
  double update_rate = 2;
  double update_time = 1.0 / update_rate;

  GameServer server;

  server.Init();

  int num_frames = 0;
  Timer t;
  Timer sleep;

  bool running = true;
  while (running) {
    timer.Restart();
    // start tick

    server.Update(update_time);

    // end tick
    num_frames++;
    double time = timer.Elapsed();
    double time_left = update_time - time;
    sleep.Restart();
    while (sleep.Elapsed() < time_left) {
      // busy wait lmao
    }

    if (t.Elapsed() > 2.0) {
      double elapsed = t.Restart();
      std::cout << "DEBUG: Real Tickrate = " << num_frames / elapsed << "\n";
      num_frames = 0;
    }
  }

  return EXIT_SUCCESS;
}