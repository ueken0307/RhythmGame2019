#ifndef RHYTHM_GAME_MANAGER_HPP
#define RHYTHM_GAME_MANAGER_HPP

#include "RhythmManager.hpp"

class NoteData {
public:
  NoteData(int count, double second, int lane, int length, bool isEndEffect = false);
  int count;
  double second;
  int lane;
  int length;
  bool isEndEffect;
};

class RhythmGameManager {
public:
  RhythmGameManager(std::vector<NoteData> notes, std::vector<BpmData> bpms, double offset, bool isAuto = false);
  void start();
  void update();
  int getBmsCount() { return rhythmManager.getBmsCount(); };
  double getSecond() { return rhythmManager.getSecond(); };

private:
  RhythmManager rhythmManager;
  std::vector<NoteData> notes;
  bool isAuto;


};

#endif // !RHYTHM_GAME_MANAGER_HPP