#pragma once
#include <chrono>
#include <thread>
#include <vector>

#include "command.hpp"
#include "tank.hpp"
#include "terrain.hpp"

class State {
public:
  static State& Instance() {
    static State instance;
    return instance;
  }
  State(State const&) = delete;
  void operator=(State const&) = delete;

  void Update();

  const std::vector<Tank> &getPlayerTanks();
  const std::vector<Tank> &getEnemyTanks();
private:
  State();

  // Game Variables
  std::chrono::milliseconds time_remaining;
  std::chrono::milliseconds timestamp;
  double width; /* Map width */
  double height; /* Map height */
  std::vector<Tank> player_tanks;
  std::vector<Tank> enemy_tanks;
//  std::unordered_map<std::string, Projectile> player_projectiles;
//  std::unordered_map<std::string, Projectile> enemy_projectiles;
  // Top left x-axis, top left y-axis, width, height
  std::vector<Terrain> solid_terrain; /* Can not shoot or move */
  std::vector<Terrain> impassable_terrain; /* Cannot move, can shoot */

  // Misc.
  std::string team;
};

State::State() {
  Command &command = Command::Instance();
  team = command.GetTeamName();
}

void State::Update() {
  Command &command = Command::Instance();
  std::string json = command.GetStateJSON();

  std::chrono::milliseconds _timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
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
    } else if(comm_type.compare("GAME_END") == 0) {
      // Game has ended
      // TODO: Something to end the program
    } else if(comm_type.compare("GAMESTATE") == 0) {
      // General State Stuff
      timestamp = _timestamp;

     // TODO: Map stuff

      // Players
      rapidjson::Value &v_players = d["players"];

      // Clear existing tank data
      player_tanks.clear();
      enemy_tanks.clear();
      for(rapidjson::SizeType i = 0; i < v_players.Size(); i++) {
        rapidjson::Value &v_player = v_players[i];
        std::string player_name = v_player["name"].GetString();

        // Set new data
        rapidjson::Value &v_tanks = v_player["tanks"];
        for(rapidjson::SizeType j = 0; j < v_tanks.Size(); j++) {
          rapidjson::Value &v_tank = v_tanks[j];
          if(!v_tank["alive"].GetBool()) {
            continue;
          }
          Tank tank(v_tank["id"].GetString());
          tank.alive = v_tank["alive"].GetBool();
          tank.x = v_tank["position"][0].GetDouble();
          tank.y = v_tank["position"][1].GetDouble();
          tank.tracks = v_tank["tracks"].GetDouble();
          tank.turret = v_tank["turret"].GetDouble();

          if(team.compare(player_name) == 0) {
            // It's us!
            player_tanks.push_back(tank);
          } else {
            // It's the enemy!
            enemy_tanks.push_back(tank);
          }
        }

      }

    }

  } else {
    // Got Mach ID (GUID)
    // TODO: Assert maybe?
  }
}

const std::vector<Tank> & State::getPlayerTanks() {
  return player_tanks;
};


const std::vector<Tank> & State::getEnemyTanks() {
  return enemy_tanks;
};
