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

private:
  std::string id;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  // TODO: Determine if we'll hit friend
  Command &command = Command::Instance();
  // TODO: Send fire command
}

