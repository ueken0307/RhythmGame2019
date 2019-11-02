#ifndef THANK_YOU_HPP
#define THANK_YOU_HPP

#include "ShareData.hpp"
#include "State.hpp"

class ThankYou :public SceneManager<State, ShareData>::Scene {
public:
  ThankYou(const InitData& init) : IScene(init), font150(150, Resource(U"JKG-L_3.ttf"), FontStyle::Bold), font40(40, Resource(U"JKG-L_3.ttf")) {
    getData().reset();
    getData().drawBackground.random();
    stopwatch.restart();
  }

  void update() override {
    getData().drawBackground.update();

    if (stopwatch.sF() > 5.0 || KeyEnter.pressed()) {
      changeScene(State::Title);
    }
  }

  void draw() const override {
    getData().drawBackground.draw();
    RectF area = font150(U"Thank you for playing!!").regionAt(1920 / 2, 1000 / 2);;
    area.draw(Color(30, 128)).drawFrame(0, 10, Color(140));
    font150(U"Thank you for playing!!").drawAt(1920 / 2, 1000 / 2);

    font40(U"開発環境 : C++, OpenSiv3D").draw(Arg::bottomRight = Vec2(1920, 1080 - 100));
    font40(U"フォント : JKフォントL").draw(Arg::bottomRight = Vec2(1920, 1080 - 50));
    font40(U"BGM,エフェクト : 魔王魂").draw(Arg::bottomRight = Vec2(1920,1080));
  }

private:
  Stopwatch stopwatch;
  Font font150, font40;

};

#endif // !THANK_YOU_HPP
