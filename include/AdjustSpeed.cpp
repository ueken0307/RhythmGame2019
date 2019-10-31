#include"AdjustSpeed.hpp"

AdjustSpeed::AdjustSpeed(const InitData& init) : IScene(init), font30(30), font60(60) {
  getData().drawGuide.set(std::vector<String>({ U"�X�s�[�h��������",U"�X�s�[�h���グ��",U"����^�C�~���O�𑁂�����",U"����^�C�~���O��x������" }));
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

  font60(U"�X�s�[�h  :  " + ToString(getData().getNoteSpeed())).drawAt(1920/2, 200);

  font60(U"����^�C�~���O  :  " + ToString(getData().getJudgeOffset())).drawAt(1920 / 2, 400);
  font30(U"LATE�������o��ꍇ�̓}�C�i�X�̒l�ɁAFAST�������o��ꍇ�͐��̒l�ɒ�������B ").drawAt(1920 / 2, 450);
}

void AdjustSpeed::drawFadeIn(double t) const {
  draw();
}

void AdjustSpeed::drawFadeOut(double t) const {
  draw();
}