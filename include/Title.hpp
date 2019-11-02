#ifndef TITLE_HPP
#define TITLE_HPP

#include "ShareData.hpp"
#include "State.hpp"

class Title :public SceneManager<State, ShareData>::Scene {
public:
  Title(const InitData& init) : IScene(init), font256(256, Resource(U"JKG-L_3.ttf"), FontStyle::Bold), font40(40, Resource(U"JKG-L_3.ttf")){
    getData().drawBackground.random();
    getData().drawGuide.set(std::vector<String>({ U"いずれかのボタンを押してスタート" }));
  }

  void update() override {
    getData().drawBackground.update();

    if ((KeyEnter | KeyD | KeyF | KeyJ | KeyK).down() ) {
      changeScene(State::HowToPlay);
    }
  }

  void draw() const override {
    getData().drawBackground.draw();
    Vec2 center = { 1920 / 2, 1000 / 2 };
    RectF area = font256(U"おとげー").regionAt(center);
    area.draw(Color(30, 128)).drawFrame(0,50,Color(140));
    font256(U"おとげー").drawAt(center);
    font40(U"ver 2019").draw(Arg::topRight = Vec2(area.x + area.w, area.y + area.h));
    getData().drawGuide.draw();
  }
  
private:
  Font font256,font40;
};


#endif // !TITLE_HPP
