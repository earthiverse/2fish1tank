#pragma once

enum TankType { FAST, SLOW };
enum TankDirection { FWD, REV };
enum TankRotation { CW, CCW };

class Tank {
public:
  Tank(std::string id);
  void Fire();
  void Move(TankRotation rot, double amt);

private:
  std::string id;
};

Tank::Tank(std::string id) : id(id) {
}

void Tank::Fire() {
  // TODO: Determine if we'll hit friend

  // TODO: Send fire command
}
