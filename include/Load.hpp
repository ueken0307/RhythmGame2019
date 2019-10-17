#ifndef LOAD_HPP
#define LOAD_HPP

#include <Siv3D.hpp>

#include <filesystem>

#include "ShareData.hpp"
#include "State.hpp"
#include "MusicInfo.hpp"

#define DEVELOP_MUSICS_PATH "../../../Musics/"

class Load :public SceneManager<State, ShareData>::Scene {
public:
  Load(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  Font font;
};

#endif // LOAD_HPP
