#include "DrawUtility.hpp"

constexpr int backgroundW = 1920;
constexpr int backgroundH = 1080;

DrawBackground::DrawBackground(){
  backgroundColor = Color(100);
  lineColor = Color(240, 100);

  this->random();
  Scene::SetBackground(backgroundColor);
}

void DrawBackground::start(){
  tm.start();
}

void DrawBackground::update(){
  double before = tm.nowSecond();
  tm.update();
  double diff = tm.nowSecond() - before;

  for (size_t i = 0; i < lines.size(); ++i) {
    if (i % 2 == 0) {
      lines.at(i).begin.x += 4 * diff;
      lines.at(i).begin.y -= 4 * diff;
      lines.at(i).end.x -= 10 * diff;
      lines.at(i).end.y += 1 * diff;
    } else {
      lines.at(i).begin.x -= 1 * diff;
      lines.at(i).begin.y -= 4 * diff;
      lines.at(i).end.x -= 6 * diff;
      lines.at(i).end.y += 5 * diff;
    }
  }

  for (size_t i = 0; i < lines.size(); ++i) {
    if (i % 2 == 0) {
      circles.at(i).center.x += 5 * diff;
      circles.at(i).center.x -= 3 * diff;
      circles.at(i).r += 11 * diff;
    }
    else {
      circles.at(i).center.x -= 1 * diff;
      circles.at(i).center.x += 6 * diff;
      circles.at(i).r -= 5 * diff;
    }
  }
}

void DrawBackground::draw() const{

  for (const auto& line : lines) {
    line.draw(8,lineColor);
  }

  for (const auto& circle : circles) {
    circle.drawFrame(8,lineColor);
  }
}

void DrawBackground::random() {
  auto  time = std::chrono::high_resolution_clock::now();
  std::chrono::seconds second = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());
  Reseed(second.count());

  Random(1, 6);

  lines.clear();
  lines.push_back(
    Line({ Random(0, 1920),Random(0,1080) }, { Random(0, 1920),Random(0,1080) })
  );
  lines.push_back(
    Line({ Random(0, 1920),Random(0,1080) }, { Random(0, 1920),Random(0,1080) })
  );
  lines.push_back(
    Line({ Random(0, 1920),Random(-2000,0) }, { Random(0, 1920),Random(1080,2080) })
  );
  lines.push_back(
    Line({ Random(-2000,0),Random(0,1080) }, { Random(1920, 3920),Random(0,1080) })
  );

  circles.clear();
  circles.push_back(
    Circle({ Random(0, 1920),Random(0,1080) }, Random(5,200))
  );

  circles.push_back(
    Circle({ Random(0, 1920),Random(0,1080) }, Random(5, 800))
  );

  circles.push_back(
    Circle({ Random(0, 1920),Random(0,1080) }, Random(500, 1000))
  );

  circles.push_back(
    Circle({ Random(1920,4000),Random(-500,1500) }, Random(1000,2000))
  );

  
}
