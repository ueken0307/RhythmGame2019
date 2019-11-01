#ifndef MUSIC_SELECTION_HPP
#define MUSIC_SELECTION_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"
#include "Utility.hpp"

enum class MoveStatus {
  NotMove,
  Inc,
  Dec
};

class MusicSelection :public SceneManager<State, ShareData>::Scene {
public:
  MusicSelection(const InitData& init);

  void update() override;
  void draw() const override;

private:
  void drawElem(int startX, int startY, MusicInfo& info) const;

  Font font30, font60;
  Stopwatch stopwatch;

  MoveStatus moveStatus;
  double moveSec;
  bool direction; // down -> false, up -> true
};

#endif // MUSIC_SELECTION_HPP
