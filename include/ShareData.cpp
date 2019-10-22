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
  judgeCounts.fill(0);
};

void ResultData::miss() {
  if (combo > maxCombo) maxCombo = combo;
  combo = 0;

  missCounts++;
}

void ResultData::incJudge(size_t judgeIndex, double timing) {
  judgeCounts.at(judgeIndex)++;
  judgeTiming.at(judgeIndex).push_back(timing);
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