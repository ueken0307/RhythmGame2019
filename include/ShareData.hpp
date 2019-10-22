#ifndef SHARE_DATA_HPP
#define SHARE_DATA_HPP

#include "MusicInfo.hpp"

class ShareData {

public:
  ShareData() {
    selected = 0;
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

private:
  size_t selected;
};


#endif //SHARE_DATA_HPP