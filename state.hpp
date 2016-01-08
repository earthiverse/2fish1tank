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
  void SetMatchData(string &_matchID, string &_teamName); // TODO: password, team, name
  // TODO: Refactor so it grabs the info out of state

private:
  State();

  string match_id;
  string team_name;

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

      // Players
      const Value &v2 = d["players"];
      for(SizeType i = 0; i < v2.Size(); i++) {
        const Value &v3 = v2[i];
        Player p;
        p.name = v3["name"].GetString();
        p.score = v3["score"].GetDouble();

        // Tanks
        const Value &v4 = v3["tanks"];
        for(SizeType j = 0; j < v4.Size(); j++) {
          const Value &v5 = v4[j];
          string id = v5["id"].GetString();
          double hp = v5["health"].GetDouble();
          double hitRadius = v5["hitRadius"].GetDouble();
          double collisionRadius = v5["collisionRadius"].GetDouble();
          bool alive = v5["alive"].GetBool();
          double x = v5["position"][0].GetDouble();
          double y= v5["position"][1].GetDouble();
          double tracks = v5["tracks"].GetDouble();
          double turret = v5["turret"].GetDouble();
          double speed = v5["speed"].GetDouble();

          // Projectiles
          // TODO: Well, if we want to try to dodge them, we should add them.
        }

        // Assign user
        if(team_name.compare(v3["name"].GetString()) == 0) {
          self = p;
        } else {
          enemy = p;
        }
      }
    } else {
      // Unknown Command
      cout << "Unknown comm_type: " << comm_type << endl;
    }
  } else {
    // Got Match ID (GUID)
    assert(match_id.compare(json) == 0);
  }
}

void State::SetMatchData(string &_match_id, string &_team_name) {
  team_name = _team_name;
  match_id = _match_id;
}
