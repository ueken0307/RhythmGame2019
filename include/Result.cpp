#include "Result.hpp"

Result::Result(const InitData& init) : IScene(init), font(30) {

}


void Result::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::MusicSelection);
  }
}

void Result::draw() const {
  auto& result = getData().result;
  font(ToString(result.getTotalPer() * 100) + U"%").draw(0, 0);
  font(U"Max Combo:" + ToString(result.getMaxCombo())).draw(0, 30);

  for (size_t i = 0; i < judges.size(); ++i) {
    int fast = 0, late = 0;
    double ave = 0.0;
    for (auto timing : result.getJudgeTimings(i)) {
      if (timing < 0) {
        fast++;
      }
      else if (timing > 0) {
        late++;
      }
      ave += timing;
    }
    if (!result.getJudgeTimings(i).empty()) {
      ave /= result.getJudgeTimings(i).size();
    }
    else {
      ave = 0.0;
    }
    

    result.getJudgeCounts(i);

    font(judges.at(i).name + U" : " + ToString(result.getJudgeCounts(i)) + U" (Fast:" + ToString(fast) + U", Late:" + ToString(late) + U", Average:" + ToString(ave) + U")").draw(0, 70 + 30 * i);
  }

  font(miss.name + U" : " + ToString(result.getMissCounts())).draw(0, 70 + 30 * judges.size());
}

void Result::drawFadeIn(double t) const {
  draw();
}

void Result::drawFadeOut(double t) const {
  draw();
}