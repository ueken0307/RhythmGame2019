#ifndef HOW_TO_PLAY_HPP
#define HOW_TO_PLAY_HPP

#include "ShareData.hpp"
#include "State.hpp"

class HowToPlay :public SceneManager<State, ShareData>::Scene {
public:
  HowToPlay(const InitData& init) : IScene(init) {
    getData().drawBackground.random();
  }

  void update() override {
    getData().drawBackground.update();

    if (KeyEnter.down()) {
      changeScene(State::MusicSelection);
    }
  }

  void draw() const override {
    //getData().drawBackground.draw();
    TextureAsset(U"howto").draw(0, 0);
  }

};

#endif // !HOW_TO_PLAY_HPP
