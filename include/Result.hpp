#ifndef RESULT_HPP
#define RESULT_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class Result :public SceneManager<State, ShareData>::Scene {
public:
  Result(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  Font font;
};

#endif // RESULT_HPP
