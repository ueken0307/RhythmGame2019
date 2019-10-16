#include"RhythmGameManager.hpp"

NoteData::NoteData(int count,double second,int lane,int length,bool isEndEffect) {
  this->count = count;
  this->second = second;
  this->lane = lane;
  this->length = length;
  this->isEndEffect = isEndEffect;
}

RhythmGameManager::RhythmGameManager(std::vector<NoteData> notes, std::vector<BpmData> bpms, double offset, bool isAuto):rhythmManager(bpms,offset), isAuto(isAuto){
  for (auto i : notes) {
    this->notes.push_back(i);
  }
}

void RhythmGameManager::start() {
  rhythmManager.start();
}

void RhythmGameManager::update() {
  rhythmManager.update();
}