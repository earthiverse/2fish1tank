#pragma once

#include "command.hpp"

enum TankType { FAST, SLOW };

class Tank {
  friend class State;

public:
  Tank(std::string id);
  void Fire();
  void Move(const Direction &dir, const double &amt);
  void Rotate(const Rotation &rot, const double &amt);
  void RotateTurret(const Rotation &rot, const double &amt);

  bool isAlive();

  const std::string &getID();
  const double &getx();
  const double &gety();

private:
  std::string id;
  double x;
  double y;
  bool alive;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  // TODO: Determine if we'll hit friend
  Command &command = Command::Instance();
  command.Fire(id);
// TODO: Send fire command
}

bool Tank::isAlive() { return alive; };
const std::string &Tank::getID() { return id; }
const double &Tank::getx() { return x; }
const double &Tank::gety() { return y; }


void Tank::RotateTurret(const Rotation &rot, const double &amt) {
  Command &command = Command::Instance();
  command.RotateTurret(id, rot, amt);
}
