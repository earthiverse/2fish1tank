#pragma once
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
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

  double getAngle(double x , double y, double x1, double y1); 
  bool isFriendinLine(Tank shooter, Tank target);

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
    aimAt(tank, target);
//    tank.Move(FWD, 1);
    if ( isFriendinLine(tank,target)) { 
    }
    else {
      tank.Fire();
    }
    std::cout << "tank id: " << tank.getID() << std::endl;
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

/*void TankManager::aimAt(Tank shooter, Tank target) {
  double angle = atan2(target.gety() - shooter.gety(), target.getx() - shooter.getx());
  double turn = angle - shooter.getturret();
  if (turn > 0) {
    shooter.RotateTurret(CCW, turn);
  } else {
    shooter.RotateTurret(CW, -1 * turn);
  }
//  shooter.Fire();
}*/

void TankManager::aimAt(Tank shooter, Tank target) {
  double angle = atan2(target.gety() - shooter.gety(), target.getx() - shooter.getx());
  if(angle < 0) {
    angle = angle + (2 * M_PI);
  }
  double current = shooter.getturret();
  double amount = angle - current;
  if(amount > 0) {
    shooter.RotateTurret(CCW, amount / 2);
    shooter.Rotate(CCW, amount / 2);
  } else {
    shooter.RotateTurret(CW, -amount / 2);
    shooter.Rotate(CW, -amount / 2);
  }
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

double TankManager::getAngle(double x , double y, double x1, double y1) {
  double angle = atan2(x- y, x1 - y1);
  if(angle < 0) {
    angle = angle + (2 * M_PI);
  }
  return angle;
}

bool TankManager::isFriendinLine(Tank shooter, Tank target) {
  //auto firevec = getVector(tank);
  for( auto friendly : State::Instance().getPlayerTanks() ) {
    if( friendly.getID() == shooter.getID()) {
      continue;
    }
    double x = friendly.getx();
    double y = friendly.gety();

    double x1, y1, x2, y2 , x3, y3, x4, y4;
    x1 = x - 2; y1 = y - 2; x2 = x + 2; y2 = y + 2; x3 = x - 2; y3 = y + 2; x4 = x + 2; y4 =y - 2;
    
    double angle1, angle2, angle3, angle4, angle2target;
    angle2target = getAngle(shooter.getx(), shooter.gety(), target.getx(), target.gety());
    angle1 = getAngle(shooter.getx(), shooter.gety(), x1, y1);
    angle2 = getAngle(shooter.getx(), shooter.gety(), x2, y2);
    angle3 = getAngle(shooter.getx(), shooter.gety(), x3, y3);
    angle4 = getAngle(shooter.getx(), shooter.gety(), x4, y4);
    auto result = std::minmax({angle1,angle2,angle3,angle4});
    auto max = result.second;
    auto min = result.first;
    if( angle2target < max && angle2target > min ) {
      return true;
    }
  }
  return false;
}
