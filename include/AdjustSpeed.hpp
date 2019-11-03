#ifndef ADJUST_SPEED_HPP
#define ADJUST_SPEED_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class AdjustSpeed :public  SceneManager<State, ShareData>::Scene {
public:
  AdjustSpeed(const InitData& init);

  void update() override;
  void draw() const override;

private:
  double getJudgeDiff();
  void judge();
  double noteYFunc(double input) const;
  double getNoteY(double t) const;
  double getNoteHeight(double t) const;
  double getNoteStartX(double y) const;
  double getNoteEndX(double y) const;
  Quad  getNoteQuad(double second) const;


  Font font30,font60;
  Stopwatch stopwatch;
  Effect effect;
  bool isJudgeEnded;
  bool isSpeed;
  double recentJudgeResult;
};

#endif // !ADJUST_HPP
