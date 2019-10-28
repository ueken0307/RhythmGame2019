#ifndef NOTE_DATA_HPP
#define NOTE_DATA_HPP

class NoteData {
public:
  NoteData(int count, double second, double endSecond, int lane, int length);
  int count;
  double second;
  double endSecond;
  int lane;
  int length;
  bool isJudgeEnded;
  bool firstKeyStatus;
  bool inLong;
  bool isLongJudgeEnded;
  int inLongJudgeIndex;
  bool isVisible;
};


#endif // !NOTE_DATA_HPP
