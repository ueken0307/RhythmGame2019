#include"NoteData.hpp"

NoteData::NoteData(int count, double second, double endSecond, int lane, int length):
  count(count), second(second), endSecond(endSecond), lane(lane), length(length){
  isJudgeEnded = false;
  isLong = false;
  isLongJudgeEnded = false;
  isVisible = true;
}