#ifndef RESULT_HPP
#define RESULT_HPP

#include <Siv3D.hpp>

#include "ShareData.hpp"
#include "State.hpp"

class Result :public SceneManager<State, ShareData>::Scene {
public:
  Result(const InitData& init);

  void update() override;
  void draw() const override;

private:
  Font font40,font60,font100;
};

#endif // RESULT_HPP
