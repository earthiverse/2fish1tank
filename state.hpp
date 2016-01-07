#pragma once
#include <mutex>
#include <vector>
#include "rapidjson/document.h"
#include "map.hpp"
#include "player.hpp"
#include "tank.hpp"
using namespace std;
using namespace rapidjson;

class State {
public:
  static State& Instance() {
    static State instance;
    return instance;
  }
  State(State const&) = delete;
  void operator=(State const&) = delete;
  void ParseJSON(string &json, string &match_id);
  void SetMatchData(string _matchID); // TODO: password, team, name
  // TODO: Refactor so it grabs the info out of state

private:
  State();

  double time_remaining;
  double time_updated;
  Map map;
  Player self;
  Player enemy;

  static std::mutex mutex;
};

State::State() {
}

void State::ParseJSON(string &json, string &match_id) {
  Document d;
  d.Parse(json.c_str());

  if(d.IsObject()) {
    // Got Match Data (JSON)
    string comm_type = d["comm_type"].GetString();
    if(comm_type.compare("GAME_START") == 0) {
      // Game is Starting
      cout << "Game has Started!" << endl;
    } else if(comm_type.compare("MatchEnd") == 0) {
      // Game is Ending
      cout << "Game has Ended!" << endl;
    } else if(comm_type.compare("GAMESTATE") == 0) {
      // New Game State! Parse away!
      cout << "Game In Progress!" << endl;
      cout << "  Time Remaining: " << d["timeRemaining"].GetDouble() << "s" << endl;

      // Time
      time_remaining = d["timeRemaining"].GetDouble();
      time_updated = d["timestamp"].GetDouble();

      // Map & Terrain
      const Value &v1 = d["map"]["terrain"];
      std::vector<Terrain> new_terrain;
      // Terrain is unlikely to change (unless beginning of game or sudden death...)
      new_terrain.reserve(map.terrain.size());
      for(SizeType i = 0; i < v1.Size(); i++) {
        const Value &v2 = v1[i];
        // Corners
        Terrain t;
        t.x = v2["boundingBox"]["corner"][0].GetDouble();
        t.y = v2["boundingBox"]["corner"][1].GetDouble();
        // Size
        t.width = v2["boundingBox"]["size"][0].GetDouble();
        t.height = v2["boundingBox"]["size"][1].GetDouble();
        // Type
        string type_s = v2["type"].GetString();
        if(type_s.compare("IMPASSABLE") == 0) {
          t.type = TerrainType::IMPASSABLE;
        } else if(type_s.compare("SOLID") == 0) {
          t.type = TerrainType::SOLID;
        }

        new_terrain.push_back(t);
      }
      map.terrain = new_terrain;

      // Tanks
      const Value &v2 = d["players"];
      // TODO: Move team name to state so we can see what player we are and stuff
    } else {
      // Unknown Command
      cout << "Unknown comm_type: " << comm_type << endl;
    }
  } else {
    // Got Match ID (GUID)
    assert(match_id.compare(json) == 0);
  }
}
