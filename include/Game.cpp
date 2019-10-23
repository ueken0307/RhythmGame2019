#include "Game.hpp"

#define UP_Y 0
#define BOTTOM_Y 1080
#define UP_W  250
#define BOTTOM_W 1400
#define CENTER_X (1920/2)
#define JUDGE_LINE_H 1000
#define JUDGE_LINE_W ((BOTTOM_W-UP_W)*( JUDGE_LINE_H /(double)(BOTTOM_Y-UP_Y)) + UP_W)

Game::Game(const InitData& init) : IScene(init), font(30), isStart(false) {
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

  //譜面のオフセット
  double scoreOffset = getData().getSelectedInfo().getOffset();
  //開始小節
  double startMeasure = 0;
  //譜面が始まる前のの時間
  double beforeSec = 1.0;

  //RhythmManagerの初期化
  rhythmManager = RhythmManager(bpms,scoreOffset,startMeasure,beforeSec);

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

  //音楽のロード、初期位置設定
  AudioAsset::Preload(getData().getSelectedInfo().getAssetName());
  AudioAsset(getData().getSelectedInfo().getAssetName()).setPosSec(rhythmManager.getMusicInitPos());

  //タップ音のロード
  AudioAsset::Preload(U"tap");

  for (size_t i = 0; i < vLines.size(); ++i) {
    vLines.at(i) = Line({CENTER_X - (UP_W/2)  + ((double)UP_W / (vLines.size() - 1)) * i, UP_Y},
      { CENTER_X - (BOTTOM_W / 2) + ((double)BOTTOM_W / (vLines.size() - 1)) * i, BOTTOM_Y});
  }
}


void Game::update() {
  if (isStart) {
    rhythmManager.update();
    if (!AudioAsset(getData().getSelectedInfo().getAssetName()).isPlaying() && 
      rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
      AudioAsset(getData().getSelectedInfo().getAssetName()).play();
    }

    for (auto& i : notes) {
      if (rhythmManager.getStartMeasure() * 9600 <= i.count && !i.isEndEffect && abs(i.second - rhythmManager.getSecond()) <= (1/60.0)*2) {
        AudioAsset(U"tap").stop();
        AudioAsset(U"tap").play();
        i.isEndEffect = true;
      }
    }

  } else {
    if (KeySpace.pressed()) {
      isStart = true;
      rhythmManager.start();

      if (rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
        AudioAsset(getData().getSelectedInfo().getAssetName()).play();
      }
    }
  }

  if (KeyEnter.pressed()) {
    AudioAsset(getData().getSelectedInfo().getAssetName()).stop();
    AudioAsset::Release(getData().getSelectedInfo().getAssetName());
    this->changeScene(State::Result);
  }
}

void Game::draw() const {
  for (auto& i : vLines) {
    i.draw();
  }
  Line({ CENTER_X - JUDGE_LINE_W / 2.0, JUDGE_LINE_H }, { CENTER_X + JUDGE_LINE_W / 2.0, JUDGE_LINE_H }).draw();

  font(U"ゲーム").draw();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}