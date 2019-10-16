#include "RhythmManager.hpp"


BpmData::BpmData(int count, double bpm,int beat) {
  this->count = count;
  this->bpm = bpm;
  this->beat = beat;
}

RhythmManager::RhythmManager(std::vector<BpmData> &bpmDatas,double offsetSecond,int startMeasure){

  //BPM•Ï‰»•b‚ÌŽæ“¾
  this->bpmDatas.push_back(BpmData(bpmDatas[0].count, bpmDatas[0].bpm, bpmDatas[0].beat));
  this->changeTimes.push_back(0.0);

  for (int i = 1; i < bpmDatas.size(); ++i) {
    this->bpmDatas.push_back(BpmData(bpmDatas[i].count, bpmDatas[i].bpm, bpmDatas[i].beat));
    this->changeTimes.push_back((60.0 / bpmDatas[i-1].bpm)*((bpmDatas[i].count - bpmDatas[i-1].count) / (9600.0/bpmDatas[i-1].beat)) + changeTimes[i-1]);
  }

  startSecond = BtoS(9600 * startMeasure);
  this->offsetSecond = offsetSecond + startSecond;
  bmsCount = (timer.nowSecond(offsetSecond) * (this->bpmDatas[0].bpm / 60.0) * (9600.0 / this->bpmDatas[0].beat));
}

double RhythmManager::BtoS(int count) const {
  int tmp = 0;
  for (int i = 0; i < bpmDatas.size(); ++i) {
    if (bpmDatas[i].count <= count) {
      tmp = i;
    } else {
      break;
    }
  }
  return changeTimes[tmp] + ((count - bpmDatas[tmp].count)/(9600.0/bpmDatas[tmp].beat)) * (60.0/bpmDatas[tmp].bpm);
}

void RhythmManager::start() {
  bmsCount = 0;
  timer.start();
}

void RhythmManager::update() {
  timer.update();

  int tmp = 0;
  for (int i = 0; i < changeTimes.size(); ++i) {
    if (changeTimes[i] <= timer.nowSecond(offsetSecond)) {
      tmp = i;
    } else {
      break;
    }
  }

  bmsCount = bpmDatas[tmp].count + ((timer.nowSecond(offsetSecond) - changeTimes[tmp]) * (bpmDatas[tmp].bpm / 60.0) * (9600.0 / bpmDatas[tmp].beat));
}
