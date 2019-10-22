#include "Game.hpp"

Game::Game(const InitData& init) : IScene(init), font(30) {
  JSONReader reader(getData().getScoreFileName());
  DEBUG_PRINTF("%s",getData().getScoreFileName().narrow().c_str());
  if (!reader) {
    //譜面ファイル開けなかったときの処理
  }

  //---------bpm---------
  std::vector<BpmData> bpms;
  for (const auto& i : reader[U"bpms"].arrayView()) {
    bpms.push_back(BpmData(i[U"time"].get<int32>(), i[U"bpm"].get<double>(), i[U"beat"].get<int32>()));
  }

  rhythmManager = RhythmManager(bpms);

  //---------note--------
  int totalNotes = 0;
  for (const auto& i : reader[U"notes"].arrayView()) {
    notes.push_back(
      NoteData(i[U"time"].get<int32>(), 
        rhythmManager.BtoS(i[U"time"].get<int32>()),
        i[U"lane"].get<int32>(), i[U"length"].get<int32>()));
    totalNotes++;
    if (i[U"length"].get<int32>() != 0) totalNotes++;
  }

  getData().result.reset(totalNotes);

}


void Game::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::Result);
  }
}

void Game::draw() const {
  font(U"ゲーム").draw();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}