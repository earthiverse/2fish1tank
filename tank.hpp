#pragma once
using namespace std;

class Tank {
  friend class State;

public:
  Tank(string _id, bool _alive, double _hp);
private:
  string id;
  bool alive;
  double health;
  double hit_radius;
  double collision_radius;
  // Position (x, y)
  double x;
  double y;
  double tracks;
  double turret;
};

Tank::Tank(string _id, bool _alive, double _hp) : id(_id), alive(_alive), health(_hp) {
}
