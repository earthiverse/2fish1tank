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
  double getDistance(double x, double y, double x1, double y1);

  void fireAt(Tank shooter, Tank target);
  double getShellTravelTime(Tank tank, double x, double y);

  std::pair<double,double> getVector(Tank tank);
  std::pair<double, double> getVector(double x, double y, double x1, double y2);

private:
};

void TankManager::Act() {
  State &state = State::Instance();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (auto tank : State::Instance().getPlayerTanks()) {
    if(!tank.isAlive()) {
      continue;
    }
    Tank target = getClosestEnemyTank(tank);
    //tank.RotateTurret(CW, 3.14);
    //tank.Fire();
    //tank.Move(FWD, 5);
    //tank.Rotate(CW, 3.14);
    fireAt(tank,target);
    std::cout << "tank id: " << tank.getID() << std::endl;
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
  /*auto vec1 = getVector(shooter);
  double dot = vec1.first * target.getx() + vec1.second * target.gety();
  double length = sqrt( pow( vec1.first, 2) + pow( vec1.second, 2)) * sqrt( pow( target.getx(),2) + pow( target.gety(),2) );
  double theta = acos(dot/length);
  if ( theta > 3.14 ) {
    shooter.RotateTurret(CCW, theta);
  } else {
    shooter.RotateTurret(CW, theta);
  }
  shooter.Fire();*/
  double angle = atan2(target.gety() - shooter.gety(), target.getx() - shooter.getx());
  double turn = angle - shooter.getturret();
  if (turn  > 0) {
    shooter.RotateTurret(CCW, turn);
  } else {
    shooter.RotateTurret(CW, -1 * turn);
  }
  shooter.Fire();
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
