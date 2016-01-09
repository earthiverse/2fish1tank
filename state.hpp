#pragma once
#include <chrono>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "command.hpp"
#include "tank.hpp"

class State {
public:
  static State& Instance() {
    static State instance;
    return instance;
  }
  State(State const&) = delete;
  void operator=(State const&) = delete;

  void Update(const std::string &json); /* Parses state JSON */

  // Concurrency
  std::mutex mutex;

  // State monitoring
  void StartMonitoring(); /* Creates & starts thread to monitor state */
  void StopMonitoring(); /* Stops the thread monitoring state */
   
  const std::unordered_map<std::string, Tank> &getPlayerTanks();
  const std::unordered_map<std::string, Tank> &getEnemyTanks();
private:
  State();

  // Game Variables
  double width; /* Map width */
  double height; /* Map height */
  std::unordered_map<std::string, Tank> player_tanks;
  std::unordered_map<std::string, Tank> enemy_tanks;
//  std::unordered_map<std::string, Projectile> player_projectiles;
//  std::unordered_map<std::string, Projectile> enemy_projectiles;
  // Top left x-axis, top left y-axis, width, height
  std::vector<std::tuple<double, double, double, double>> solid_terrain; /* Can not shoot or move */
  std::vector<std::tuple<double, double, double, double>> impassable_terrain; /* Cannot move, can shoot */

  // State Monitoring
  std::mutex running;
  std::thread monitor_thread;
  void UpdateState(); /* Infinite loop of updating state */
};

State::State() {
}

void State::Update(const std::string &json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  if(d.IsObject()) {
    // Got Match Data (JSON)
    // TODO: Parse
    const std::string comm_type = d["comm_type"].GetString();
    if(comm_type.compare("GAME_START") == 0) {
      // Match has started
    } else if(comm_type.compare("MatchEnd") == 0) {
      // Match has ended
    } else if(comm_type.compare("GAMESTATE") == 0) {
      // Game is currently running
//      double _time_remaining = d["timeRemaining"].GetDouble();
      double _timestamp = d["timestamp"].GetDouble();

      #ifdef NDEBUG
      std::cout << "Message Timestamp: " << _timestamp << std::endl;
      std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
      std::cout << "Current Timestamp: " << ms.count() << std::endl;
      #endif

      // Lock & Update State
      std::lock_guard<std::mutex> lock(mutex);
      // TODO: Update
    }

  } else {
    // Got Mach ID (GUID)
    // TODO: Assert maybe?
  }
}

void State::UpdateState() {
  while(true) {
    // Check if we're still supposed to be running
    if(running.try_lock()) {
      // Get new state data
      Command &command = Command::Instance();
      Update(command.GetStateJSON());
    } else {
      // We're supposed to die now
      break;
    }
    running.unlock();
  }
}

void State::StartMonitoring() {
  monitor_thread = std::thread(&State::UpdateState, this);
}

void State::StopMonitoring() {
  running.lock();
  monitor_thread.join();
  running.unlock();
}


const std::unordered_map<std::string, Tank> & State::getPlayerTanks() {
  return player_tanks;
};


const std::unordered_map<std::string, Tank> & State::getEnemyTanks() {
  return enemy_tanks;
};
