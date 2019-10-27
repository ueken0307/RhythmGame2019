#ifndef NOTE_DATA_HPP
#define NOTE_DATA_HPP

class NoteData {
public:
  NoteData(int count, double second, int lane, int length);
  int count;
  double second;
  int lane;
  int length;
  bool isJudgeEnded;
  bool isLong;
  bool isLongJudgeEnded;
  bool isVisible;
};


#endif // !NOTE_DATA_HPP
