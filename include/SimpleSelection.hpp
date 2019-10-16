#ifndef SIMPLE_SELECTION_HPP
#define SIMPLE_SELECTION_HPP

#include <Siv3D.hpp>
#include <HamFramework.hpp>

#include "ShareData.hpp"

class SimpleSelection :public SceneManager<String, ShareData>::Scene {
public:
  void init() override;
  void update() override;
  void draw() const override;
  void drawFadeIn(double t) const override;
  void drawFadeOut(double t) const override;

private:
  GUI gui;

};

#endif // !SIMPLE_SELECTION_HPP
