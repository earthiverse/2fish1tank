#pragma once
#include <vector>

#include "command.hpp"

enum TankType { FAST, SLOW };

struct PositionHistory {
  double x;
  double y;
  double time;
};

class Tank {
  friend class State;

public:
  Tank(std::string id);
  void Fire();
  void StopFire();
  void Move(const Direction &dir, const double &amt);
  void Rotate(const Rotation &rot, const double &amt);
  void RotateTurret(const Rotation &rot, const double &amt);

  bool isAlive();

  const std::string &getID();
  const double &getx();
  const double &gety();
  const double &getturret();

private:
  std::string id;
  double x;
  double y;
  double tracks;
  double turret;
  bool alive;

  std::vector<PositionHistory> previous_positions;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  Command &command = Command::Instance();
  command.Fire(id);
}

void Tank::StopFire() {
  Command &command = Command::Instance();
  command.StopFire(id);
}

bool Tank::isAlive() { return alive; };
const std::string &Tank::getID() { return id; }
const double &Tank::getx() { return x; }
const double &Tank::gety() { return y; }


void Tank::RotateTurret(const Rotation &rot, const double &amt) {
  Command &command = Command::Instance();
  command.RotateTurret(id, rot, amt);
}

void Tank::Move(const Direction &dir, const double &amt) {
  Command &command = Command::Instance();
  command.Move(id, dir, amt);
}
void Tank::Rotate(const Rotation &rot, const double &amt) {
  Command &command = Command::Instance();
  command.Rotate(id, rot, amt);
}

const double & Tank::getturret() {
  return turret;
}
