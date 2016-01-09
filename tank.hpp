#pragma once

#include "command.hpp"

enum TankType { FAST, SLOW };
enum TankDirection { FWD, REV };
enum TankRotation { CW, CCW };

class Tank {
public:
  Tank(std::string id);
  void Fire();
  void Move(TankRotation rot, double amt);
  float getx();
  float gety();

private:
  std::string id;
  float x;
  float y;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  // TODO: Determine if we'll hit friend
  Command &command = Command::Instance();
  command.Fire(id);  
// TODO: Send fire command
}


void Tank::Move(TankRotation rot, double amt) {
  //Command::Instance().Move(rot, amt);
}

float Tank::getx() { return x; }
float Tank::gety() { return y; }
