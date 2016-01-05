#pragma once
#include <iostream>
#include "state.hpp"
using namespace std;

class Bot {
public:
  Bot();
  void run();
private:
  State state;
};

Bot::Bot() {
}
