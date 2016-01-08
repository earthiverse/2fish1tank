#pragma once
using namespace std;

enum TankType { FAST, SLOW };

class Tank {
  friend class State;

public:
  //Tank(string _id, bool _alive, double _hp);
	Tank();
	void move_tank(float x, float y);
	void rotate_tank(float rads);
	void rotate_cannon(float rads);
	void fire_cannon();

private:

	TankType tankType;
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
/*
Tank::Tank(string _id, bool _alive, double _hp) : id(_id), alive(_alive), health(_hp) {
}
*/
Tank::Tank() {
  alive = false;
}

void Tank::move_tank(float x, float y) {
  Command::Instance().move_tank(id, x, y);
}

void Tank::rotate_tank(float rads){
  Command::Instance().rotate_tank(id, rads);
}

void Tank::rotate_cannon(float rads){
  Command::Instance().rotate_cannon(id, rads);
}

void Tank::fire_cannon(){
  Command::Instance().fire(id);
}


