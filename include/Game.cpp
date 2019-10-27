#include "Game.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


constexpr int centerX = 1920 / 2;
constexpr int upY = 0;
constexpr int bottomY = 1080;
constexpr int upW = 250;
constexpr int bottomW = 1400;
constexpr int judgeLineY = 1000;
constexpr int judgeLineW = convertRange(upY,bottomY,judgeLineY,upW,bottomW);
constexpr int startNoteH = 1;
constexpr int endNoteH = 30;

// (0 < input < 1)  (0 < output < 1)
double noteYFunc(double input) {
  double start = 1.7 * M_PI;
  double end = 2.0 * M_PI;
  double x = per2v(start, end, input);
  double tmp = abs(sin(end) - sin(start));
  //double x = ((3.0 / 2.0) * M_PI) + ((1.0 / 2.0) * M_PI) * t;
  return pow(abs(sin(x) - sin(start)) / tmp, 2);
}

double calcJudgeLineValue(double start, double end) {
  //static const double judgeLinePer = x2per(upY, bottomY, judgeLineY);
  double x = start + (end - start) / 2.0;
  double diff = convertRange(0, 1, noteYFunc(x), upY, bottomY) - judgeLineY;
  DEBUG_PRINTF("%lf\n", diff);

  if (abs(diff) < 0.5) {
    return x;
  }

  return (diff < 0) ?
    (calcJudgeLineValue(x, end)) :
    (calcJudgeLineValue(start, x));
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
    allNotes.at(i[U"lane"].get<int32>()).push_back(
      NoteData(
        i[U"time"].get<int32>(),
        rhythmManager.BtoS(i[U"time"].get<int32>()),
        i[U"lane"].get<int32>(),
        i[U"length"].get<int32>()
      )
    );

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
    vLines.at(i) = Line({centerX - (upW/2)  + ((double)upW / (vLines.size() - 1)) * i, upY},
      { centerX - (bottomW / 2) + ((double)bottomW / (vLines.size() - 1)) * i, bottomY});
  }

  laneKeys = {KeyS, KeyD, KeyF, KeyJ, KeyK, KeyL};
  beforeKeyStatus.fill(false);
  
  toJudgeLineNoteSpeed = getData().getNoteSpeed();
  toBottomNoteSpeed = toJudgeLineNoteSpeed /calcJudgeLineValue(0, 1);
}


void Game::update() {
  if (isStart) {
    rhythmManager.update();
    input();

    if (!AudioAsset(getData().getSelectedInfo().getAssetName()).isPlaying() && 
      rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
      AudioAsset(getData().getSelectedInfo().getAssetName()).play();
    }

    for (auto& laneNotes : allNotes) {
      for (auto& note : laneNotes) {
        if (rhythmManager.getStartMeasure() * 9600 <= note.count && !note.isEndEffect && abs(note.second - rhythmManager.getSecond()) <= (1 / 60.0) * 2) {
          AudioAsset(U"tap").stop();
          AudioAsset(U"tap").play();
          note.isEndEffect = true;
        }
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

  Line({ centerX - judgeLineW / 2.0, judgeLineY },
    { centerX + judgeLineW / 2.0, judgeLineY }).draw(4,Color(220,30,30));

  for (const auto& laneNotes : allNotes) {
    for (const auto& note : laneNotes) {
      double diff = rhythmManager.getSecond() + toBottomNoteSpeed - note.second;
      if (!note.isEndEffect && diff >= 0) {
        getNoteQuad(note).draw();
      }
      if (diff < 0) {
        break;
      }
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
  int noteHeight = getNoteHeight(second);
  int upY = centerY - noteHeight / 2.0, bottomY = centerY + noteHeight/2.0;

  return Quad(
    { getNoteStartX(upY,lane),upY }, 
    { getNoteEndX(upY,lane),upY }, 
    { getNoteEndX(bottomY,lane),bottomY }, 
    { getNoteStartX(bottomY,lane),bottomY }
  );

}

int Game::getNoteY(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return convertRange(0,1, noteYFunc(arg),upY,bottomY);
}

int Game::getNoteHeight(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return convertRange(0, 1, noteYFunc(arg), startNoteH, endNoteH);
}

int Game::getNoteStartX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 0;
  else if (lane == 5) index = 2;
  else index = lane - 1;

  return convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x);
}

int Game::getNoteEndX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 2;
  else if (lane == 5) index = 4;
  else index = lane;

  return convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x);
}
