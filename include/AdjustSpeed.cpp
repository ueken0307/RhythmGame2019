#include"AdjustSpeed.hpp"

AdjustSpeed::AdjustSpeed(const InitData& init) : IScene(init), font30(30), font60(60) {
  getData().drawGuide.set(std::vector<String>({ U"スピードを下げる",U"スピードを上げる",U"判定タイミングを早くする",U"判定タイミングを遅くする" }));
}


void AdjustSpeed::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::Game);
  }

  if (KeyD.down()) {
    getData().decNoteSpeed();
  }

  if (KeyF.down()) {
    getData().incNoteSpeed();
  }

  if (KeyJ.down()) {
    getData().decJudgeOffset();
  }

  if (KeyK.down()) {
    getData().incJudgeOffset();
  }
}

void AdjustSpeed::draw() const {
  getData().drawGuide.draw();

  font60(U"スピード  :  " + ToString(getData().getNoteSpeed())).drawAt(1920/2, 200);

  font60(U"判定タイミング  :  " + ToString(getData().getJudgeOffset())).drawAt(1920 / 2, 400);
  font30(U"LATEが多く出る場合はマイナスの値に、FASTが多く出る場合は正の値に調整する。 ").drawAt(1920 / 2, 450);
}

void AdjustSpeed::drawFadeIn(double t) const {
  draw();
}

void AdjustSpeed::drawFadeOut(double t) const {
  draw();
}