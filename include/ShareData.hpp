#ifndef SHARE_DATA_HPP
#define SHARE_DATA_HPP

#include "MusicInfo.hpp"
#include "Utility.hpp"
#include "DrawUtility.hpp"

#define MAX_LEVEL_NUM 2
#define JUDGE_NUM 2

class Judge {
public:
  Judge(String name, double per, double duration) :name(name), per(per), duration(duration) {};
  String name;
  double per;
  double duration;
};

extern Judge miss;
extern std::array<Judge, JUDGE_NUM> judges;

struct JudgeTiming {
  JudgeTiming(int fast, int slow, double ave) : fast(fast), slow(slow), ave(ave) {};
  int fast;
  int slow;
  double ave;
};

class ResultData {
public:
  ResultData() { reset(1); };

  void reset(int totalNotes);
  void incMiss();
  void incJudge(size_t judgeIndex, double timing);
  double getTotalPer();
  double getNowPer();
  int getCombo();
  int getMaxCombo();
  int getJudgeCounts(size_t judgeIndex);
  int getMissCounts();
  JudgeTiming getJudgeTiming(size_t judgeIndex);
  JudgeTiming getAllJudgeTiming();

private:
  int totalNotes;
  int combo;
  int maxCombo;
  int missCounts;
  std::array<int, JUDGE_NUM> judgeCounts;
  std::array<std::vector<double>, JUDGE_NUM> judgeTimings;

};

class ShareData {

public:
  ShareData() {
    reset();
  };

  void reset() {
    playNum = 0;

    selected = 0;
    levelNum = 0;
    levelFileName = { U"/easy.json",U"/normal.json",U"/hard.json" };
    noteSpeed = 0.8;
    judgeOffset = 0.0;
  }

  void setMusicFolderPath(String path);
  String getMusicFolderPath();
  void setGlobalOffset(double offset);
  double getGlobalOffset();

  size_t getSelected() const;
  void incSelected();
  void decSelected();
  size_t getLevelNum() const;
  void incLevelNum();
  void decLeveNum();
  double getNoteSpeed();
  void incNoteSpeed();
  void decNoteSpeed();
  double getJudgeOffset();
  void incJudgeOffset();
  void decJudgeOffset();
  double getTotalOffset();
  String getScoreFileName();
  MusicInfo& getSelectedInfo();
  MusicInfo& getInfoFromSelected(int offset);

  int getMaxPlayNum();
  int getPlayNum();
  void incPlayNum();


  std::vector<MusicInfo> infos;
  std::array<String, 3> levelFileName;
  ResultData result;
  DrawBackground drawBackground;
  DrawGuide drawGuide;

private:
  String musicFolderPath;
  double globalOffset;

  size_t selected, levelNum;
  double noteSpeed;
  double judgeOffset;
  int playNum;

};


#endif //SHARE_DATA_HPP