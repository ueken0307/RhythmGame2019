#include "Result.hpp"

constexpr int jacketCenterX = 400;
constexpr int jacketCenterY = 450;
constexpr int jacketSize = 300;
constexpr int titleCenterX = jacketCenterX;
constexpr int titleCenterY = jacketCenterY + jacketSize;
constexpr Rect jacket({ jacketCenterX - jacketSize / 2, jacketCenterY - jacketSize / 2 }, { jacketSize, jacketSize });

constexpr int elemSize = 600;
constexpr int elemCenterX = 1400;
constexpr int elemCenterY = 1080/2;
constexpr double elemFrameThickness = 3.0;
constexpr Color elemColor1(40), elemColor2(80);
constexpr std::array<Color, 2> judgeColors({ Color(220, 50, 50), Color(50, 220, 50) });
constexpr Color missColor(100, 100, 220);

constexpr Quad calcQuadFromCenter(int cX, int cY) {
  return Quad({ cX - elemSize / 2, cY }, { cX, cY - elemSize / 2 }, { cX + elemSize / 2, cY }, { cX, cY + elemSize / 2 });
};

constexpr std::array<Quad,4> elems({
  calcQuadFromCenter(elemCenterX, elemCenterY - elemSize / 2),
  calcQuadFromCenter(elemCenterX - elemSize / 2, elemCenterY),
  calcQuadFromCenter(elemCenterX + elemSize / 2, elemCenterY),
  calcQuadFromCenter(elemCenterX, elemCenterY + elemSize / 2)
});

constexpr int judgeRankElemSize = 300;
constexpr Quad judgeRankElem({ elemCenterX - judgeRankElemSize / 2, elemCenterY }, { elemCenterX, elemCenterY - judgeRankElemSize / 2 },
  { elemCenterX + judgeRankElemSize / 2, elemCenterY }, { elemCenterX, elemCenterY + judgeRankElemSize / 2 });
constexpr Color judgeRankColor(200, 200, 200);

constexpr int elemPerOffsetY = 250;
constexpr int elemComboOffsetY = elemPerOffsetY + 130;

constexpr int elemTitleOffsetY = 140;
constexpr int elemCountOffsetY = elemTitleOffsetY + 70;
constexpr int elemFastOffsetY = elemCountOffsetY + 70;
constexpr int elemSlowOffsetY = elemFastOffsetY + 50;
constexpr int elemAveOffsetY = elemSlowOffsetY + 50;

constexpr int elemMissTitleOffsetY = 200;
constexpr int elemMissCountOffsetY = elemMissTitleOffsetY + 100;

String getJudgeRank(double per) {
  if (per == 1) {
    return U"EX";
  }
  else if (per > 0.95) {
    return U"S";
  }
  else if (per > 0.9) {
    return U"A";
  }
  else if (per > 0.8) {
    return U"B";
  }
  else if (per > 0.7) {
    return U"C";
  }
  else if (per > 0.6) {
    return U"D";
  }

  return U"E";
}




Result::Result(const InitData& init) : IScene(init), font40(40) , font60(60) , font100(100){
  getData().drawBackground.random();
}


void Result::update() {
  getData().drawBackground.update();

  if (KeyEnter.pressed()) {
    this->changeScene(State::MusicSelection);
  }
}

void Result::draw() const {
  getData().drawBackground.draw();

  jacket(TextureAsset(getData().getSelectedInfo().getAssetName())).draw();
  font40(getData().getSelectedInfo().getTitle()).drawAt(titleCenterX, titleCenterY);


  for (size_t i = 0; i < elems.size(); ++i) {
    elems.at(i).draw((i%3 == 0)? elemColor2 : elemColor1);
  }

  auto& result = getData().result;

  font60(ToString(result.getTotalPer() * 100) + U"%").drawAt(elems.at(0).p1.x, elems.at(0).p1.y + elemPerOffsetY);
  font40(U"Max Combo:" + ToString(result.getMaxCombo())).drawAt(elems.at(0).p1.x, elems.at(0).p1.y + elemComboOffsetY);

  for (size_t i = 0; i < judges.size(); ++i) {
    JudgeTiming tmp = result.getJudgeTiming(i);
    font60(judges.at(i).name).drawAt(elems.at(i + 1).p1.x, elems.at(i + 1).p1.y + elemTitleOffsetY, judgeColors.at(i));
    font40(ToString(result.getJudgeCounts(i))).drawAt(elems.at(i + 1).p1.x, elems.at(i + 1).p1.y + elemCountOffsetY);
    font40(U"Fast    : " + ToString(tmp.fast)).drawAt(elems.at(i + 1).p1.x, elems.at(i + 1).p1.y + elemFastOffsetY);
    font40(U"Slow    : " + ToString(tmp.slow)).drawAt(elems.at(i + 1).p1.x, elems.at(i + 1).p1.y + elemSlowOffsetY);
    font40(U"Average : " + ToString(tmp.ave)).drawAt(elems.at(i + 1).p1.x, elems.at(i + 1).p1.y + elemAveOffsetY);
  }

  font60(miss.name).drawAt(elems.at(3).p1.x, elems.at(3).p1.y + elemMissTitleOffsetY, missColor);
  font40(ToString(result.getMissCounts())).drawAt(elems.at(3).p1.x, elems.at(3).p1.y + elemMissCountOffsetY);

  judgeRankElem.draw(judgeRankColor);
  font100(getJudgeRank(getData().result.getTotalPer())).drawAt(elemCenterX, elemCenterY, Palette::Yellow);
}

void Result::drawFadeIn(double t) const {
  draw();
}

void Result::drawFadeOut(double t) const {
  draw();
}