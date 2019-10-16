#include"RhythmGameManager.hpp"

NoteData::NoteData(int count,double second,int lane,int length,bool isEndEffect) {
  this->count = count;
  this->second = second;
  this->lane = lane;
  this->length = length;
  this->isEndEffect = isEndEffect;
}

RhythmGameManager::RhythmGameManager(std::vector<NoteData> notes, std::vector<BpmData> bpms, double offset):rhythmManager(bpms,offset) {
  for (auto i : notes) {
    this->notes.push_back(i);
  }
}

void RhythmGameManager::start(bool isAuto) {
  this->isAuto = isAuto;
  rhythmManager.start();
}

void RhythmGameManager::update() {
  rhythmManager.update();
}