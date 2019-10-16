#ifndef NOTE_DATA_HPP
#define NOTE_DATA_HPP

class NoteData {
public:
  NoteData(int count, double second, int lane, int length, bool isEndEffect = false,bool isLong = false,bool isEndLong = false);
  int count;
  double second;
  int lane;
  int length;
  bool isEndEffect;
  bool isLong;
  bool isEndLong;
};


#endif // !NOTE_DATA_HPP
