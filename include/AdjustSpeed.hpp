#ifndef ADJUST_SPEED_HPP
#define ADJUST_SPEED_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class AdjustSpeed :public  SceneManager<State, ShareData>::Scene {
public:
  AdjustSpeed(const InitData& init);

  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  Font font30,font60;
};

#endif // !ADJUST_HPP
