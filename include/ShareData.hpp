#ifndef SHARE_DATA_HPP
#define SHARE_DATA_HPP

#include "MusicInfo.hpp"

class ShareData {

public:
  ShareData() {
    selected = 0;
  };

  size_t selected;
  std::vector<MusicInfo> infos;
};


#endif //SHARE_DATA_HPP