#ifndef SHARE_DATA_HPP
#define SHARE_DATA_HPP

#include "MusicInfo.hpp"

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

class ResultData {
public:
  ResultData() { reset(1); };

  void reset(int totalNotes);
  void miss();
  void incJudge(size_t judgeIndex, double timing);
  double getTotalPer();
  double getNowPer();

private:
  int totalNotes;
  int combo;
  int maxCombo;
  int missCounts;
  std::array<int, JUDGE_NUM> judgeCounts;
  std::array<std::vector<double>, JUDGE_NUM> judgeTiming;

};

class ShareData {

public:
  ShareData() {
    selected = 0;
    levelNum = 0;
    levelFileName = { U"/easy.json",U"/normal.json",U"/hard.json" };
  };

  std::vector<MusicInfo> infos;

  size_t getSelected() const {
    return selected;
  };

  void incSelected() {
    selected = (selected + 1) % infos.size();
  };

  void decSelected() {
    selected = (selected == 0) ? (selected + infos.size() - 1) : (selected - 1);
  };

  size_t getLevelNum() const {
    return levelNum;
  };

  void incLevelNum() {
    if (levelNum < MAX_LEVEL_NUM) {
      levelNum++;
    }
  };

  void decLeveNum() {
    if (levelNum > 0) {
      levelNum--;
    }
  };

  String getScoreFileName() {
    return infos.at(selected).getFolderPath() + levelFileName.at(levelNum);
  }

  MusicInfo& getSelectedInfo() {
    return infos.at(selected);
  }

  std::array<String, 3> levelFileName;
  ResultData result;

private:
  size_t selected, levelNum;
};


#endif //SHARE_DATA_HPP