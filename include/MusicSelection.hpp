#ifndef MUSIC_SELECTION_HPP
#define MUSIC_SELECTION_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class MusicSelection :public SceneManager<State, ShareData>::Scene {
public:
  MusicSelection(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  Font font;
};

#endif // MUSIC_SELECTION_HPP
