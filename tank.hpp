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

private:
  std::string id;
  double x;
  double y;
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
