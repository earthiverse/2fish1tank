#pragma once
#include <vector>
#include <cmath>

#include "command.hpp"
#include "tank.hpp"
#include "state.hpp"

class TankManager {
public:
  void Act();
  Tank getClosestEnemyTank(Tank tank);
  double getDistance(Tank tank1, Tank tank2);
  double getDistance(double x, double y, double x1, double y1);

  void fireAt(Tank shooter, Tank target);
  double getShellTravelTime(Tank tank, double x, double y);
private:
};

void TankManager::Act() {
  State &state = State::Instance();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (auto tank : State::Instance().getPlayerTanks()) {
    if(!tank.isAlive()) {
      continue;
    }

    std::cout << "tank id: " << tank.getID() << std::endl;
    //Tank target = getClosestEnemyTank(tank);
    tank.RotateTurret(CW, 3.14);
    tank.Fire();
    tank.Move(FWD, 5);
    tank.Rotate(CW, 3.14);
  }
}

Tank TankManager::getClosestEnemyTank(Tank shooter) {
  if(State::Instance().getEnemyTanks().empty()) {
    
  }
  Tank target = State::Instance().getEnemyTanks()[0];
  for (auto tank : State::Instance().getEnemyTanks()) {
    if (getDistance(shooter,target) > getDistance(shooter,tank)) {
      target = tank;
    }
  }
  return target;
}

void TankManager::fireAt(Tank shooter, Tank target) {
  
}


double TankManager::getDistance(Tank tank1, Tank tank2) {
  return std::sqrt(std::pow(tank1.getx() - tank2.getx(),2) + std::pow(tank1.gety() - tank2.gety(), 2));
}

double TankManager::getDistance(double x, double y, double x1, double y1) {
  return std::sqrt(std::pow(x - x1,2) + std::pow(y - y1, 2));
}


double TankManager::getShellTravelTime(Tank tank, double x, double y) {
  double dist = getDistance(tank.getx(), tank.gety(), x, y);
  return dist / 30;
}
