#include "Game.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#define UP_Y 0
#define BOTTOM_Y 1080
#define UP_W  250
#define BOTTOM_W 1400
#define CENTER_X (1920/2)
#define JUDGE_LINE_Y BOTTOM_Y
#define JUDGE_LINE_W ((BOTTOM_W-UP_W)*( JUDGE_LINE_Y /(double)(BOTTOM_Y-UP_Y)) + UP_W)

#define START_NOTE_H 1
#define FINAL_NOTE_H 30

#define NOTE_SPEED 0.7

// x (start ~ end)
double x2per(double start, double end, double x) {
  return abs(x - start) / abs(end - start);
}

//0 < per < 1
double per2x(double start, double end, double per) {
  return start + (end - start) * per;
}

// srcX (srcStart ~ srcEnd)
double convertRange(double srcStart, double srcEnd, double srcX, double dstStart, double dstEnd) {
  double per = x2per(srcStart, srcEnd, srcX);
  return per2x(dstStart,dstEnd,per);
}

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

  laneKeys = {KeyS, KeyD, KeyF, KeyJ, KeyK, KeyL};
  beforeKeyStatus.fill(false);

}


void Game::update() {
  if (isStart) {
    rhythmManager.update();
    input();

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

  Line({ CENTER_X - JUDGE_LINE_W / 2.0, JUDGE_LINE_Y },
    { CENTER_X + JUDGE_LINE_W / 2.0, JUDGE_LINE_Y }).draw(4,Color(220,30,30));

  for (const auto& i : notes) {
    if (!i.isEndEffect && rhythmManager.getSecond() + NOTE_SPEED - i.second >= 0) {
      getNoteQuad(i).draw();
    }
    if (rhythmManager.getSecond() + NOTE_SPEED - i.second < 0) {
      break;
    }
  }
 
  font(U"ゲーム").draw();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}

void Game::input() {
  for (size_t i = 0; i < laneKeys.size(); ++i) {
    if (laneKeys.at(i).pressed()) {
      if (!beforeKeyStatus.at(i)) {
        judge(i);
        DEBUG_PRINTF("%zd\n", i);
      }
      else {
        DEBUG_PRINTF("%lf\n", laneKeys.at(i).pressedDuration().count());
      }

      beforeKeyStatus.at(i) = true;
    }
    else {
      beforeKeyStatus.at(i) = false;
    }
  }

}

void Game::judge(size_t lane) {

}

Quad Game::getNoteQuad(const NoteData &note) const {
  int lane = note.lane;
  double second = note.second;

  int centerY = getNoteY(second);
  int noteHeight = getNoteHeight(centerY);
  int upY = centerY - noteHeight / 2.0, bottomY = centerY + noteHeight/2.0;

  return Quad(
    { getNoteStartX(upY,lane),upY }, 
    { getNoteEndX(upY,lane),upY }, 
    { getNoteEndX(bottomY,lane),bottomY }, 
    { getNoteStartX(bottomY,lane),bottomY }
  );

}

// input (0 < t < 1) output (0 < y < 1)
double fx(double t) {
  double start = 1.7 * M_PI;
  double end = 2.0 * M_PI;
  double x = per2x(start,end,t);
  double tmp = abs(sin(end) - sin(start));
  //double x = ((3.0 / 2.0) * M_PI) + ((1.0 / 2.0) * M_PI) * t;
  return pow(abs(sin(x) - sin(start))/tmp,2);
}

int Game::getNoteY(double t) const {
  return convertRange(0,1, fx((rhythmManager.getSecond() + NOTE_SPEED - t) / NOTE_SPEED),UP_Y,BOTTOM_Y);
}

int Game::getNoteHeight(int y) const {
  return convertRange(UP_Y, BOTTOM_Y, y, START_NOTE_H, FINAL_NOTE_H);
}

int Game::getNoteStartX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 0;
  else if (lane == 5) index = 2;
  else index = lane - 1;

  return convertRange(UP_Y, BOTTOM_Y, y, vLines.at(index).begin.x, vLines.at(index).end.x);
}

int Game::getNoteEndX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 2;
  else if (lane == 5) index = 4;
  else index = lane;

  return convertRange(UP_Y, BOTTOM_Y, y, vLines.at(index).begin.x, vLines.at(index).end.x);
}
