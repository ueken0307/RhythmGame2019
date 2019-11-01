#include "ShareData.hpp"

Judge miss = Judge(U"Miss", 0, 0);

std::array<Judge, JUDGE_NUM> judges = {
  Judge(U"Perfect",1   ,(1.0 / 60.0) * 3.0),
  Judge(U"Good"   ,0.5 ,(1.0 / 60.0) * 5.0)
};

void ResultData::reset(int totalNotes) {
  this->totalNotes = totalNotes;
  combo = 0;
  maxCombo = 0;
  missCounts = 0;
  for (auto& tmp : judgeTimings) {
    tmp.clear();
  }
  judgeCounts.fill(0);
};

void ResultData::incMiss() {
  if (combo > maxCombo) maxCombo = combo;
  combo = 0;

  missCounts++;
  DEBUG_PRINTF("%s\n", miss.name.narrow().c_str());
}

void ResultData::incJudge(size_t judgeIndex, double timing) {
  combo++;
  judgeCounts.at(judgeIndex)++;
  judgeTimings.at(judgeIndex).push_back(timing);
  DEBUG_PRINTF("%s : %lf\n", judges.at(judgeIndex).name.narrow().c_str(), timing);
}

double ResultData::getTotalPer() {
  double sum = 0.0;
  for (size_t i = 0; i < JUDGE_NUM; ++i) {
    sum += judgeCounts[i] * judges[i].per;
  }

  return sum / totalNotes;
}

double ResultData::getNowPer() {
  int noteNum = std::accumulate(judgeCounts.begin(), judgeCounts.end(), 0) + missCounts;
  if (noteNum == 0) return 100.0;

  double sum = 0.0;
  for (size_t i = 0; i < JUDGE_NUM; ++i) {
    sum += judgeCounts[i] * judges[i].per;
  }

  return sum / noteNum;
}

int ResultData::getMaxCombo() {
  return maxCombo;
}

int ResultData::getJudgeCounts(size_t judgeIndex) {
  return judgeCounts.at(judgeIndex);
}

int ResultData::getMissCounts() {
  return missCounts;
}

JudgeTiming ResultData::getJudgeTiming(size_t judgeIndex) {
  JudgeTiming res(0,0,0.0);
  for (const auto& timing : judgeTimings.at(judgeIndex)) {
    if (timing < 0) {
      res.fast++;
    } else if (timing > 0) {
      res.slow++;
    }
    res.ave += timing;
  }

  if (res.fast + res.slow > 0) {
    res.ave /= res.fast + res.slow;
  }

  return res;
}

JudgeTiming ResultData::getAllJudgeTiming() {
  JudgeTiming res(0, 0, 0.0);
  for (const auto& judgeTiming : judgeTimings) {
    for (const auto& timing : judgeTiming) {
      if (timing < 0.0) {
        res.fast++;
      } else if (timing > 0.0) {
        res.slow++;
      }
      res.ave += timing;
    }
  }

  if (res.fast + res.slow > 0) {
    res.ave /= res.fast + res.slow;
  }
  
  return res;
}



void ShareData::setMusicFolderPath(String path) {
  musicFolderPath = path;
}

String ShareData::getMusicFolderPath() {
  return musicFolderPath;
}

void ShareData::setGlobalOffset(double offset) {
  globalOffset = offset;
}

double ShareData::getGlobalOffset() {
  return globalOffset;
}

size_t ShareData::getSelected() const {
  return selected;
};

void ShareData::incSelected() {
  selected = (selected + 1) % infos.size();
};

void ShareData::decSelected() {
  selected = (selected == 0) ? (selected + infos.size() - 1) : (selected - 1);
};

size_t ShareData::getLevelNum() const {
  return levelNum;
};

void ShareData::incLevelNum() {
  if (levelNum < MAX_LEVEL_NUM) {
    levelNum++;
  }
};

void ShareData::decLeveNum() {
  if (levelNum > 0) {
    levelNum--;
  }
};

double ShareData::getNoteSpeed() {
  return noteSpeed;
};

void ShareData::incNoteSpeed() {
  noteSpeed += 0.05;
}

void ShareData::decNoteSpeed() {
  if (noteSpeed > 0.1) noteSpeed -= 0.05;
}

double ShareData::getJudgeOffset() {
  return judgeOffset;
}

void ShareData::incJudgeOffset() {
  judgeOffset += 0.01;
}

void ShareData::decJudgeOffset() {
  judgeOffset -= 0.01;
}

double ShareData::getTotalOffset() {
  return globalOffset + judgeOffset;
}

String ShareData::getScoreFileName() {
  return infos.at(selected).getFolderPath() + levelFileName.at(levelNum);
}

MusicInfo& ShareData::getSelectedInfo() {
  return infos.at(selected);
}

MusicInfo& ShareData::getInfoFromSelected(int offset) {
  int index = static_cast<int>(selected) + offset;

  if (index < 0) {
    index += infos.size();
  }
  else if (index >= infos.size()) {
    index %= infos.size();
  }

  return infos.at(index);
}
