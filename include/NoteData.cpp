#include"NoteData.hpp"

NoteData::NoteData(int count, double second, int lane, int length):
  count(count), second(second), lane(lane), length(length){
  isJudgeEnded = false;
  isLong = false;
  isLongJudgeEnded = false;
  isVisible = true;
}