#ifndef THANK_YOU_HPP
#define THANK_YOU_HPP

#include "ShareData.hpp"
#include "State.hpp"

class ThankYou :public SceneManager<State, ShareData>::Scene {
public:
  ThankYou(const InitData& init) : IScene(init) {
    getData().reset();
    getData().drawBackground.random();
    stopwatch.restart();
  }

  void update() override {
    getData().drawBackground.update();

    if (stopwatch.sF() > 3.0 || KeyEnter.pressed()) {
      changeScene(State::Title);
    }
  }

  void draw() const override {
    //getData().drawBackground.draw();
    TextureAsset(U"thankyou").draw(0, 0);
  }

private:
  Stopwatch stopwatch;

};

#endif // !THANK_YOU_HPP
