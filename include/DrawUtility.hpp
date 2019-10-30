#ifndef DRAW_UTILITY_HPP
#define DRAW_UTILITY_HPP

#include <Siv3D.hpp>
#include "TimeManager.hpp"

class DrawBackground {
public:
  DrawBackground();
  void start();
  void update();
  void draw() const;
  void random();
 

private:
  TimeManager tm;
  Color backgroundColor, lineColor;
  std::vector<Line> lines;
  std::vector<Circle> circles;

};

/*
class DrawGuide {

};*/

#endif // !DRAW_UTILITY_HPP

