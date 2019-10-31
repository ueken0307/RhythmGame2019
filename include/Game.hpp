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
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

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
  int getNoteY(double t) const;
  double getNoteHeight(double t) const;
  int getNoteStartX(int y, int lane) const;
  int getNoteEndX(int y, int lane) const;
  NoteDrawStatus getNoteDrawStatus(double t)const;


  Font font;
  Texture laneEffect;
  Effect effect;
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
