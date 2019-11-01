#ifndef GAME_HPP
#define GAME_HPP

#include <Siv3D.hpp>

#include "Utility.hpp"
#include "ShareData.hpp"
#include "State.hpp"
#include "RhythmManager.hpp"
#include "NoteData.hpp"

enum class NoteDrawStatus {
  before,
  within,
  afterJudgeLine,
  afterBottom
};

class Game :public SceneManager<State, ShareData>::Scene {
public:
  Game(const InitData& init);

  void update() override;
  void draw() const override;

private:
  void input();
  void judge(size_t lane, bool beforeKeyStatus);
  void judgeNormal(NoteData &note);
  void judgeLong(NoteData& note, bool beforeKeyStatus);
  double getJudgeDiff(double noteSecond);
  void excludeEndedNote();

  void drawLaneEffect() const;
  void drawNotes() const;
  void drawNormalNote(const NoteData& note) const;
  void drawLongNote(const NoteData& note) const;
  Quad getNoteQuad(int lane, double second) const;
  Quad getLongQuad(const NoteData& note) const;
  double getNoteY(double t) const;
  double getNoteHeight(double t) const;
  double getNoteStartX(double y, int lane) const;
  double getNoteEndX(double y, int lane) const;
  NoteDrawStatus getNoteDrawStatus(double t)const;


  Font font,font100;
  Texture laneEffect;
  Effect effect;
  Stopwatch stopwatch;
  bool isStart, isMusicStarted;
  std::array<Line,5> vLines; //vertical lines
  double toJudgeLineNoteSpeed;
  double toBottomNoteSpeed;

  RhythmManager rhythmManager;
  std::array<std::list<NoteData>, 6> allNotes;

  std::array<Key, 6> laneKeys;
  std::array<bool, 6> beforeKeyStatuses;
};

#endif // GAME_HPP
