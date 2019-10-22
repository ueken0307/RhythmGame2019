#ifndef SHARE_DATA_HPP
#define SHARE_DATA_HPP

#include "MusicInfo.hpp"

#define MAX_LEVEL_NUM 2

class ShareData {

public:
  ShareData() {
    selected = 0;
    levelNum = 0;
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

private:
  size_t selected, levelNum;
};


#endif //SHARE_DATA_HPP