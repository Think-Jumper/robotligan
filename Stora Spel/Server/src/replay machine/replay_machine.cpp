#include "replay_machine.hpp"

// Private---------------------------------------------------------------------



// Public----------------------------------------------------------------------
ReplayMachine::ReplayMachine() {
  // H
  // I
  this->replay_deterministic_ = nullptr;
  this->recording_max_seconds_ = 0.0f;
  this->recording_elapsed_seconds_ = 0.0f;
  this->assert_module_ = nullptr;
  this->assert_mode_on_ = false;
}

ReplayMachine::~ReplayMachine() {
  // B
  // Y
  // E
  if (this->replay_deterministic_ != nullptr) {
    delete this->replay_deterministic_;
  }
  if (this->assert_module_ != nullptr) {
    delete this->assert_module_;
  }
}

void ReplayMachine::Init(unsigned int in_seconds,
                         unsigned int in_frames_per_second,
                         float in_snapshot_interval_seconds,
                         bool in_assert_mode) {
  // Calculate values required for a deterministic
  // replay and allocate space for one of that size
  unsigned int max_num_of_frames = in_frames_per_second * in_seconds;
  unsigned int frames_between_snapshots =
      in_frames_per_second * in_snapshot_interval_seconds;
  this->replay_deterministic_ =
      new DeterministicReplay(max_num_of_frames, 10, 6, frames_between_snapshots);

  // Set the time the machine shall record
  this->recording_max_seconds_ = in_seconds;

  // If we are running in assert mode allocate space
  // to save a registry every frame
  if (in_assert_mode) {
    this->assert_module_ = new AssertModule(max_num_of_frames);
    this->assert_mode_on_ = true;
  }
}

bool ReplayMachine::SaveReplayFrame(std::bitset<10>& in_bitset,
                                    float& in_x_value, float& in_y_value,
                                    entt::registry& in_registry,
                                    const float& in_dt) {
  // Returns true if end if buffer was reached
  // or the full time of the recording has been
  // used up

  // Save a frame to the deterministic replay
  bool ret_val = this->replay_deterministic_->SaveFrame(
      in_bitset, in_x_value, in_y_value, in_registry);

  // If assert mode is on, save the registry
  if (this->assert_mode_on_) {
    this->assert_module_->SnapshotRegistry(in_registry);
  }

  // Count up the time
  // If the time is up set to return true
  this->recording_elapsed_seconds_ += in_dt;
  if (this->recording_elapsed_seconds_ >= this->recording_max_seconds_) {
    ret_val = true;
  }

  // Return
  return ret_val;
}

bool ReplayMachine::LoadReplayFrame(std::bitset<10>& in_bitset,
                                    float& in_x_value, float& in_y_value,
                                    entt::registry& in_registry) {
  // Returns true if end if buffer was reached

  // Load a frame from the deterministic replay
  bool ret_val = this->replay_deterministic_->LoadFrame(
      in_bitset, in_x_value, in_y_value, in_registry);

  // If assert mode is on, compare the received registry
  // to the one that has been saved for each frame of the replay.
  // The purpose of assert mode is to compare the registries
  // after the deterministic replay function has done it thing
  // to evaluate if the state created by the replay is a good
  // representation of the stste that actually occured in gameplay.
  if (this->assert_mode_on_) {
    this->assert_module_->AssertRegistry(in_registry);
  }

  // Return
  return ret_val;
}
