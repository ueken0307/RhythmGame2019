#ifndef HOW_TO_PLAY_HPP
#define HOW_TO_PLAY_HPP

#include "ShareData.hpp"
#include "State.hpp"

class HowToPlay :public SceneManager<State, ShareData>::Scene {
public:
  HowToPlay(const InitData& init) : IScene(init) {
    getData().drawGuide.set(std::vector<String>({U"いずれかのボタンを押してスタート"}));
  }

  void update() override {

    if ((KeyEnter | KeyD | KeyF | KeyJ | KeyK).down()) {
      changeScene(State::MusicSelection);
    }
  }

  void draw() const override {
    TextureAsset(U"howto").draw(0, 0);
    getData().drawGuide.draw();
  }

};

#endif // !HOW_TO_PLAY_HPP
