#pragma once
#include <vector>
using namespace std;

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

class Map {
  friend class State;

private:
  double width;
  double height;
  std::vector<Terrain> terrain;
};


