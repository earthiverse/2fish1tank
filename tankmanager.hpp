#pragma once
#include <vector>
#include <cmath>
#include <utility>
#include "command.hpp"
#include "tank.hpp"
#include "state.hpp"

class TankManager {
public:
  void Act();
  Tank getClosestEnemyTank(Tank tank);
  double getDistance(Tank tank1, Tank tank2);

  void aimAt(Tank shooter, Tank target);
  double getShellTravelTime(Tank tank, Tank tank2);

  double canFire(double x, double y, double x1, double y1);

  std::pair<double,double> getVector(Tank tank);
  std::pair<double, double> getVector(double x, double y, double x1, double y2);

private:
};

void TankManager::Act() {
  State &state = State::Instance();

  // If we don't have any tanks yet don't act
  if(state.getPlayerTanks().empty() || state.getEnemyTanks().empty()) {
    std::cout << "we're all out of tanks" << std::endl;
    std::cout << "player tanks: " << state.getPlayerTanks().size() << std::endl;
    std::cout << "enemy tanks: " << state.getEnemyTanks().size() << std::endl;
    return;
  }

  for (auto tank : state.getPlayerTanks()) {
    if(!tank.isAlive()) {
      continue;
    }
    Tank target = getClosestEnemyTank(tank);
    aimAt(tank,target);
    std::cout << "tank id: " << tank.getID() << std::endl;
    return;
  }
}

Tank TankManager::getClosestEnemyTank(Tank shooter) {
  State &state = State::Instance();
  Tank target = state.getEnemyTanks()[0];
  int i = 0;
  for (const auto &tank : state.getEnemyTanks()) {
    if (getDistance(shooter, target) > getDistance(shooter, tank)) {
      std::cout << "i found a closer tank" << std::endl;
      target = tank;
    }
  }
  return target;
}

void TankManager::aimAt(Tank shooter, Tank target) {
  double angle = atan2(target.gety() - shooter.gety(), target.getx() - shooter.getx());
  double turn = angle - shooter.getturret();
  if (turn > 0) {
    shooter.RotateTurret(CCW, turn);
  } else {
    shooter.RotateTurret(CW, -1 * turn);
  }
//  shooter.Fire();
}


double TankManager::getDistance(Tank tank1, Tank tank2) {
  return std::pow(tank1.getx() - tank2.getx(), 2) + std::pow(tank1.gety() - tank2.gety(), 2);
}

double TankManager::getShellTravelTime(Tank tank, Tank tank2) {
  double dist = getDistance(tank, tank2);
  return dist / 30;
}

std::pair<double,double> TankManager::getVector(Tank tank) {
  std::pair<double,double> vec;
  vec.first = cos(tank.getturret());
  vec.second = sin(tank.getturret());
  return vec;
}

std::pair<double, double> TankManager::getVector(double x, double y, double x1, double y1) {
  std::pair<double, double> vec;
  vec.first = x - x1;
  vec.second = y - y1;
  return vec;
}
