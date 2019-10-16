#ifndef GAME_HPP
#define GAME_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class Game :public SceneManager<State, ShareData>::Scene {
public:
  Game(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  Font font;
};

#endif // GAME_HPP
