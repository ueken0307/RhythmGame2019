#ifndef GAME_HPP
#define GAME_HPP

#include <Siv3D.hpp>

#include "Utility.hpp"
#include "ShareData.hpp"
#include "State.hpp"
#include "RhythmManager.hpp"
#include "NoteData.hpp"

class Game :public SceneManager<State, ShareData>::Scene {
public:
  Game(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  void input();
  void judge(size_t lane);

  Quad getNoteQuad(const NoteData &note) const;
  int getNoteY(double t) const;
  int getNoteHeight(double t) const;
  int getNoteStartX(int y, int lane) const;
  int getNoteEndX(int y, int lane) const;


  Font font;
  bool isStart;
  std::array<Line,5> vLines; //vertical lines

  RhythmManager rhythmManager;
  std::vector<NoteData> notes;

  std::array<Key, 6> laneKeys;
  std::array<bool, 6> beforeKeyStatus;
};

#endif // GAME_HPP
