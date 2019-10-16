#include"AdjustSpeed.hpp"

AdjustSpeed::AdjustSpeed(const InitData& init) : IScene(init), font(30) {

}


void AdjustSpeed::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::Game);
  }
}

void AdjustSpeed::draw() const {
  font(U"スピード調整").draw();
}

void AdjustSpeed::drawFadeIn(double t) const {
  draw();
}

void AdjustSpeed::drawFadeOut(double t) const {
  draw();
}