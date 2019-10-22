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
  Font font;

  RhythmManager rhythmManager;
  std::vector<NoteData> notes;
};

#endif // GAME_HPP
