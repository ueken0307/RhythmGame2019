#include "Game.hpp"

Game::Game(const InitData& init) : IScene(init), font(30) {

}


void Game::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::Result);
  }
}

void Game::draw() const {
  font(U"�Q�[��").draw();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}