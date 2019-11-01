#ifndef LOAD_HPP
#define LOAD_HPP

#include <Siv3D.hpp>

#include <filesystem>

#include "ShareData.hpp"
#include "State.hpp"
#include "MusicInfo.hpp"
#include "Utility.hpp"


class Load :public SceneManager<State, ShareData>::Scene {
public:
  Load(const InitData& init);

  void update() override;
  void draw() const override;

private:
  Font font;
  std::vector<String> failList;
};

#endif // LOAD_HPP
