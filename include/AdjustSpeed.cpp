#include"AdjustSpeed.hpp"

//*** Lane ***
constexpr int centerX = 1500;
constexpr int upY = 0;
constexpr int bottomY = 1000;
constexpr int upW = 60;
constexpr int bottomW = 350;
constexpr int startNoteH = 5;
constexpr int endNoteH = 40;

constexpr Line leftLine({ centerX - upW / 2, upY }, { centerX - bottomW / 2, bottomY });
constexpr Line rightLine({ centerX + upW / 2, upY }, { centerX + bottomW / 2, bottomY });

double AdjustSpeed::noteYFunc(double input) const{
  double start = 1.7 * s3d::Math::Pi;
  double end = 2.0 * s3d::Math::Pi;
  double x = per2v(start, end, input);
  double tmp = abs(sin(end) - sin(start));
  return pow(abs(sin(x) - sin(start)) / tmp, 2);
}

double AdjustSpeed::getNoteY(double t) const {
  double arg = t / getData().getNoteSpeed();
  return  convertRange(0, 1, noteYFunc(arg), upY, bottomY);
}

double AdjustSpeed::getNoteHeight(double t) const {
  double arg = t / getData().getNoteSpeed();
  return  convertRange(0, 1, noteYFunc(arg), startNoteH, endNoteH);
}

double AdjustSpeed::getNoteStartX(double y) const {
  return convertRange(upY, bottomY, y, leftLine.begin.x, leftLine.end.x);
}

double AdjustSpeed::getNoteEndX(double y) const {
  return convertRange(upY, bottomY, y, rightLine.begin.x, rightLine.end.x);
}

Quad  AdjustSpeed::getNoteQuad(double second) const {
  double noteCenterY = getNoteY(second);
  double noteHeight = getNoteHeight(second);
  double noteUpY = noteCenterY - noteHeight / 2.0, noteBottomY = noteCenterY + noteHeight / 2.0;

  return Quad(
    { getNoteStartX(noteUpY),noteUpY },
    { getNoteEndX(noteUpY),noteUpY },
    { getNoteEndX(noteBottomY),noteBottomY },
    { getNoteStartX(noteBottomY),noteBottomY }
  );
}




AdjustSpeed::AdjustSpeed(const InitData& init) : IScene(init), font30(30), font60(60) {
  getData().drawGuide.set(std::vector<String>({ U"�X�s�[�h���グ��",U"�X�s�[�h��������",U"����^�C�~���O�𑁂�����",U"����^�C�~���O��x������" }));

  stopwatch.restart();
}


void AdjustSpeed::update() {
  double t = stopwatch.sF();
  if (t > getData().getNoteSpeed()) {
    stopwatch.restart();
  }

  if (KeyEnter.pressed()) {
    this->changeScene(State::Game);
  }

  if (KeyD.down()) {
    getData().decNoteSpeed();
    stopwatch.restart();
  }

  if (KeyF.down()) {
    getData().incNoteSpeed();
    stopwatch.restart();
  }

  if (KeyJ.down()) {
    getData().decJudgeOffset();
  }

  if (KeyK.down()) {
    getData().incJudgeOffset();
  }
}

void AdjustSpeed::draw() const {
  double t = stopwatch.sF();
  getNoteQuad(t).draw();
  leftLine.draw(2.0);
  rightLine.draw(2.0);

  font60(U"�X�s�[�h  :  " + ToString(getData().getNoteSpeed())).drawAt(600, 300);
  font30(U"�m�[�c�������n�߂Ă��画�胉�C���ɓ��B����܂ł̕b[s]").drawAt(600, 350);

  font60(U"����^�C�~���O  :  " + ToString(getData().getJudgeOffset())).drawAt(600, 500);
  font30(U"LATE�������o��ꍇ�̓}�C�i�X�̒l�ɁAFAST�������o��ꍇ�͐��̒l�ɒ�������B ").drawAt(600, 550);

  getData().drawGuide.draw();
}
