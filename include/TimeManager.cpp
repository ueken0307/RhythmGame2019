#include "TimeManager.hpp"

#include <cmath>

std::chrono::high_resolution_clock::time_point getTime() {
  return std::chrono::high_resolution_clock::now();
}

int diff(std::chrono::high_resolution_clock::time_point start,
  std::chrono::high_resolution_clock::time_point end) {
  std::chrono::microseconds diff = 
    std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  return static_cast<int>(diff.count());
}

TimeManager::TimeManager() {
  startTime = getTime();
  nowTime = startTime;
}

void TimeManager::start() {
  startTime = getTime();
  nowTime = getTime();
}

void TimeManager::update() {
  nowTime = getTime();
}

double TimeManager::nowSecond(double offsetSecond) const {
  return diff(startTime, nowTime) * pow(10.0,-6.0) + offsetSecond;
}

int TimeManager::nowMicroSecond(int offsetMicroSecond) const {
  return diff(startTime, nowTime) + offsetMicroSecond;
}
