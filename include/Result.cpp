#include "Result.hpp"

Result::Result(const InitData& init) : IScene(init), font(30) {

}


void Result::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::MusicSelection);
  }
}

void Result::draw() const {
  font(U"ƒŠƒUƒ‹ƒg").draw();
}

void Result::drawFadeIn(double t) const {
  draw();
}

void Result::drawFadeOut(double t) const {
  draw();
}