#include"AdjustSpeed.hpp"

//*** Lane ***
constexpr int centerX = 400;
constexpr int upY = 0;
constexpr int bottomY = 1000;
constexpr int upW = 60;
constexpr int bottomW = 350;
constexpr int startNoteH = 5;
constexpr int endNoteH = 40;
constexpr double previewDuration = 0.5;

constexpr Line leftLine({ centerX - upW / 2, upY }, { centerX - bottomW / 2, bottomY });
constexpr Line rightLine({ centerX + upW / 2, upY }, { centerX + bottomW / 2, bottomY });
constexpr Line judgeLine({ centerX - bottomW / 2, bottomY }, { centerX + bottomW / 2, bottomY });

//*** Judge Effect ***
constexpr double judgeEffectSecond = 0.25;
constexpr int judgeEffectStartY = 400;
constexpr int judgeEffectDstY = judgeEffectStartY - 100;

struct previewJudgeEffect : IEffect {
  String judgeAssetName;

  previewJudgeEffect(const String judgeAssetName) : judgeAssetName(judgeAssetName) {};

  bool update(double t) override {
    double cX = centerX;
    double cY = judgeEffectStartY + (judgeEffectDstY - judgeEffectStartY) * (t / judgeEffectSecond);

    TextureAsset(judgeAssetName).scaled(0.3).drawAt(cX, cY);
    return t < judgeEffectSecond;
  }
};

double AdjustSpeed::getJudgeDiff() {
  //早いときは負の値、遅いときは正の値になる
  //     ---> 早いときはjudgeOffsetを正の値に、遅いときは負の値にする 
  return stopwatch.sF() - getData().getNoteSpeed() + getData().getTotalOffset();
}


void AdjustSpeed::judge() {
  double diff = getJudgeDiff();
  recentJudgeResult = diff;
  for (size_t i = 0; i < judges.size(); i++) {
    if (abs(diff) < judges.at(i).duration) {
      effect.add<previewJudgeEffect>(judges.at(i).name);
      isJudgeEnded = true;
      AudioAsset(U"tap").stop();
      AudioAsset(U"tap").play();
      break;
    }
  }
}

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




AdjustSpeed::AdjustSpeed(const InitData& init) : IScene(init), font30(30), font60(60), isJudgeEnded(false), isSpeed(true), recentJudgeResult(0.0) {
  getData().drawGuide.set(std::vector<String>({ U"判定を確認する",U"スピードを上げる",U"スピードを下げる",U"判定タイミング調整に切り替える" }));

  stopwatch.restart();
}


void AdjustSpeed::update() {
  double t = stopwatch.sF();
  if (t > getData().getNoteSpeed() + previewDuration) {
    stopwatch.restart();
  }

  if (KeyEnter.pressed()) {
    this->changeScene(State::Game);
  }

  if (KeyD.down()) {
    judge();
  }

  if (KeyF.down()) {
    if (isSpeed) {
      getData().decNoteSpeed();
    } else {
      getData().decJudgeOffset();
    }
    stopwatch.restart();
  }

  if (KeyJ.down()) {
    if (isSpeed) {
      getData().incNoteSpeed();
    } else {
      getData().incJudgeOffset();
    }
    stopwatch.restart();
  }

  if (KeyK.down()) {
    isSpeed = !isSpeed;
    if (isSpeed) {
      getData().drawGuide.set(std::vector<String>({ U"判定を確認する",U"スピードを上げる",U"スピードを下げる",U"判定タイミング調整に切り替える" }));
    } else {
      getData().drawGuide.set(std::vector<String>({ U"判定を確認する",U"判定タイミングを早くする",U"判定タイミングを遅くする",U"スピード調整に切り替える" }));
    }
  }
}

void AdjustSpeed::draw() const {
  double t = stopwatch.sF();
  getNoteQuad(t).draw();
  leftLine.draw(2.0);
  rightLine.draw(2.0);
  judgeLine.draw(10.0, Color(240, 20, 20));
  

  font60(((isSpeed)? U"→スピード  :  ": U"スピード　:　" )+ ToString(getData().getNoteSpeed()))
    .drawAt(1200, 300,((isSpeed)? Color(255,80,80) : Color(255)));
  font30(U"ノーツが見え始めてから判定ラインに到達するまでの秒[s]").drawAt(1200, 350);

  font60(((!isSpeed) ? U"→判定タイミング  :  " : U"判定タイミング  :  ") + ToString(getData().getJudgeOffset()))
    .drawAt(1200, 500, ((!isSpeed) ? Color(255, 80, 80) : Color(255)));
  font30(U"SLOWが多く出る場合はマイナスの値に、FASTが多く出る場合は正の値に調整する。 ").drawAt(1200, 550);

  if (getData().getPlayNum() != 0) {
    font30(U"前回のプレイからオススメの判定タイミング : " + ToString(-1 * getData().result.getAllJudgeTiming().ave)).drawAt(1200, 600, Color(255, 80, 80));
  }
  

  font30(U"現在のズレ : " + ToString(recentJudgeResult)).drawAt(centerX, 750, Color(40));

  effect.update();
  getData().drawGuide.draw();
}
