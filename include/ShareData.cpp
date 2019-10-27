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

std::vector<double>& ResultData::getJudgeTimings(size_t judgeIndex) {
  return judgeTimings.at(judgeIndex);
}

int ResultData::getMissCounts() {
  return missCounts;
}