#pragma once
#include <mutex>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "rapidjson/document.h"
using namespace std;
using namespace rapidjson;
using namespace boost::geometry;

enum TerrainType { SOLID, IMPASSABLE };

struct Terrain {
  // top left x-axis
  double x;
  // top left y-axis
  double y;
  double width;
  double height;
  TerrainType type;
};

struct Map {
  double width;
  double height;
  std::vector<Terrain> terrain;
};

class State {
public:
  static State& Instance() {
    static State instance;
    return instance;
  }
  State(State const&) = delete;
  void operator=(State const&) = delete;
  void ParseJSON(string &json, string &match_id);

private:
  State();

  double time_remaining;
  double time_updated;
  Map map;

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
      const Value &t = d["map"]["terrain"];
      std::vector<Terrain> new_terrain;
      for(SizeType i = 0; i < t.Size(); i++) {
        const Value &t2 = t[i];
        // Corners
        Terrain t3;
        t3.x = t2["boundingBox"]["corner"][0].GetDouble();
        t3.y = t2["boundingBox"]["corner"][1].GetDouble();
        // Size
        t3.width = t2["boundingBox"]["size"][0].GetDouble();
        t3.height = t2["boundingBox"]["size"][1].GetDouble();
        // Type
        string type_s = t2["type"].GetString();
        if(type_s.compare("IMPASSABLE") == 0) {
          t3.type = TerrainType::IMPASSABLE;
        } else if(type_s.compare("SOLID") == 0) {
          t3.type = TerrainType::SOLID;
        }

        new_terrain.push_back(t3);
      }
      map.terrain = new_terrain;
      cout << "  We have " << map.terrain.size() << " thingies of terrain" << endl;
    } else {
      // Unknown Command
      cout << "Unknown comm_type: " << comm_type << endl;
    }
  } else {
    // Got Match ID (GUID)
    assert(match_id.compare(json) == 0);
  }
}
