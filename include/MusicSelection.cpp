#include "MusicSelection.hpp"

MusicSelection::MusicSelection(const InitData& init) : IScene(init), font(30) {

}


void MusicSelection::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::AdjustSpeed);
  }
}

void MusicSelection::draw() const {
  font(U"‘I‹È").draw();
}

void MusicSelection::drawFadeIn(double t) const {
  draw();
}

void MusicSelection::drawFadeOut(double t) const {
  draw();
}