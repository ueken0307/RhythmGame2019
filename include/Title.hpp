#ifndef TITLE_HPP
#define TITLE_HPP

#include "ShareData.hpp"
#include "State.hpp"

class Title :public SceneManager<State, ShareData>::Scene {
public:
  Title(const InitData& init) : IScene(init) {
    getData().drawBackground.random();
  }

  void update() override {
    getData().drawBackground.update();

    if (KeyEnter.down()) {
      changeScene(State::HowToPlay);
    }
  }

  void draw() const override {
    //getData().drawBackground.draw();
    TextureAsset(U"title").draw(0, 0);
  }
  
};


#endif // !TITLE_HPP
