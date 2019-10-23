#ifndef RHYTHM_MANAGER_HPP
#define RHYTHM_MANAGER_HPP

#include "TimeManager.hpp"

#include<vector>


class BpmData {
public:
  BpmData(int count, double bpm, int beat);
  int count;
  double bpm;
  int beat;
};

class RhythmManager {
public:
  RhythmManager(): bpmDatas(std::vector<BpmData>()), bmsCount(0), scoreOffset(0),beforeSec(0), totalOffset(0), startMeasure(0), musicInitPos(0), musicStartSec(0) {};
  RhythmManager(std::vector<BpmData> &bpmDatas, double scoreOffset, int startMeasure, double beforeSec);
  void start();
  void update();
  int getBmsCount() const { return bmsCount; };
  double getSecond() const { return timer.nowSecond(totalOffset); }
  double BtoS(int count) const;

  double getMusicInitPos() { return musicInitPos; };
  double getMusicStartSec() { return musicStartSec; };

private:
  std::vector<BpmData> bpmDatas;
  std::vector<double> changeTimes;
  TimeManager timer;
  int bmsCount;

  double scoreOffset, beforeSec, totalOffset;
  int startMeasure;
  double musicInitPos,musicStartSec;
};

#endif // RHYTHM_MANAGER_HPP
