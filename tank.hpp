#pragma once

#include "command.hpp"

enum TankType { FAST, SLOW };

class Tank {
public:
  Tank(std::string id);
  void Fire();
  void Move(const Direction &dir, const double &amt);
  void Rotate(const Rotation &rot, const double &amt);
  void RotateTurret(const Rotation &rot, const double &amt);

  const double &getx();
  const double &gety();
  const double &getturret();

private:
  std::string id;
  double x;
  double y;
  double turret;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  // TODO: Determine if we'll hit friend
  Command &command = Command::Instance();
  command.Fire(id);
// TODO: Send fire command
}

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
  Command $command = Command::Instance();
  command.Rotate(id, rot, amt);
}

const double & Tank::getturret() {
  return turret;
}
