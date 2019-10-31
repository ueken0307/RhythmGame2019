#include "Game.hpp"

#define _USE_MATH_DEFINES
#include <math.h>


constexpr int centerX = 1920 / 2;
constexpr int upY = 0;
constexpr int bottomY = 1080;
constexpr int upW = 250;
constexpr int bottomW = 1400;
constexpr int judgeLineY = 1000;
constexpr double judgeLineW = convertRange(upY,bottomY,judgeLineY,upW,bottomW);
constexpr int startNoteH = 1;
constexpr int endNoteH = 30;
constexpr double longNoteJudgeDuration = 0.25;

constexpr Color laneBackColor = Color(40,40,40);

constexpr int laneEffectLength = 500;

constexpr Color LNSEActiveColor = Color(40, 100, 200); //ロングノーツの始点と終点の判定成功時のカラー
constexpr Color LNSEMissColor = Color(20, 50, 100); //ロングノーツの始点と終点の判定失敗時時のカラー
constexpr Color LNHalfwayActiveColor = Color(80, 200, 200); //ロングノーツの途中の判定成功時のカラー
constexpr Color LNHalfwayMissColor = Color(40, 100, 100); //ロングノーツの途中の判定失敗時時のカラー

constexpr double judgeEffectSecond = 0.25;
constexpr int judgeEffectStartY = judgeLineY - 100;
constexpr int judgeEffectDstY = judgeEffectStartY - 100;


struct JudgeStrEffect : IEffect {
  int lane;
  String judgeAssetName;

  JudgeStrEffect(const int lane, const String judgeAssetName)
    : lane(lane), judgeAssetName(judgeAssetName) {
  }

  bool update(double t) override {
    if (1 <= lane && lane <= 4) {
      int cX = (centerX - (judgeLineW / 2) + judgeLineW/8) + (lane-1)*(judgeLineW / 4);
      int cY = judgeEffectStartY + (judgeEffectDstY - judgeEffectStartY) * (t/judgeEffectSecond);
      TextureAsset(judgeAssetName).scaled(0.25).drawAt(cX, cY);
    }
    return t < judgeEffectSecond;
  }
};


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

Game::Game(const InitData& init) : IScene(init), font(30), isStart(false), isMusicStarted(false) {
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
  int startMeasure = 0;
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
        rhythmManager.BtoS(i[U"time"].get<int32>() + i[U"length"].get<int32>()),
        i[U"lane"].get<int32>(),
        i[U"length"].get<int32>()
      )
    );

    totalNotes++;
  }

  //長押しのtotalNotes処理
  for (const auto& laneNotes : allNotes) {
    for (const auto& note : laneNotes) {
      if (note.length != 0) {
        //終点 + 途中
        totalNotes += 1 + floor((note.endSecond - note.second) / longNoteJudgeDuration);
      }
    }
  }

  getData().result.reset(totalNotes);

  //音楽のロード、初期位置設定
  AudioAsset::Preload(getData().getSelectedInfo().getAssetName());
  AudioAsset(getData().getSelectedInfo().getAssetName()).setLoop(false);
  AudioAsset(getData().getSelectedInfo().getAssetName()).setPosSec(rhythmManager.getMusicInitPos());

  //タップ音のロード
  AudioAsset::Preload(U"tap");

  //エフェクト画像のプリロード
  for (const auto& judge : judges) {
    TextureAsset::Preload(judge.name);
  }
  TextureAsset::Preload(miss.name);


  laneEffect = Texture(Resource(U"laneEffect.png"));

  for (size_t i = 0; i < vLines.size(); ++i) {
    vLines.at(i) = Line({centerX - (upW/2)  + ((double)upW / (vLines.size() - 1)) * i, upY},
      { centerX - (bottomW / 2) + ((double)bottomW / (vLines.size() - 1)) * i, bottomY});
  }

  laneKeys = {KeyS, KeyD, KeyF, KeyJ, KeyK, KeyL};
  beforeKeyStatuses.fill(false);
  
  toJudgeLineNoteSpeed = getData().getNoteSpeed();
  toBottomNoteSpeed = toJudgeLineNoteSpeed /calcJudgeLineValue(0, 1);
}


void Game::update() {
  if (isStart) {
    rhythmManager.update();
    input();
    excludeEndedNote();

    if (!isMusicStarted && rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
      isMusicStarted = true;
      AudioAsset(getData().getSelectedInfo().getAssetName()).play();
    }

    for (auto& laneNotes : allNotes) {
      for (auto& note : laneNotes) {
        if (rhythmManager.getStartMeasure() * 9600 <= note.count && !note.isJudgeEnded && abs(note.second - rhythmManager.getSecond()) <= (1 / 60.0) * 2) {
          //AudioAsset(U"tap").stop();
          //AudioAsset(U"tap").play();
          //note.isJudgeEnded = true;
        }
      }
    }

  } else {
    if (KeySpace.pressed()) {
      isStart = true;
      rhythmManager.start();

      if (rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
        isMusicStarted = true;
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

void Game::input() {
  for (size_t i = 0; i < laneKeys.size(); ++i) {
    if (laneKeys.at(i).pressed()) {
      judge(i, beforeKeyStatuses.at(i));
      beforeKeyStatuses.at(i) = true;
    }
    else {
      beforeKeyStatuses.at(i) = false;
    }
  }

}

void Game::judge(size_t lane, bool beforeKeyStatus) {
  double mostLooseJudge = judges.back().duration;

  for (auto& note : allNotes.at(lane)) {
    if (!note.isJudgeEnded) {
      double diff = getJudgeDiff(note.second);
      if (diff > -mostLooseJudge) {
        if (note.length == 0 && !beforeKeyStatus) {
          judgeNormal(note);
        }
        else {
          judgeLong(note,beforeKeyStatus);
        }
      }
      else {
        //diff < -mostLooseJudge
        //(まだ判定の範囲内に入ってない)
        break;
      }
    }
  }
}

void Game::judgeNormal(NoteData& note) {
  double diff = getJudgeDiff(note.second);
  for (size_t i = 0; i < judges.size(); i++) {
    if (abs(diff) < judges.at(i).duration) {
      getData().result.incJudge(i, diff);
      effect.add<JudgeStrEffect>(note.lane,judges.at(i).name);
      note.isJudgeEnded = true;
      note.isVisible = false;
      break;
    }
  }
}


void Game::judgeLong(NoteData& note, bool beforeKeyStatus) {
  if (!note.inLong) {
    //始点
    double diff = getJudgeDiff(note.second);
    for (size_t i = 0; i < judges.size(); i++) {
      if (!beforeKeyStatus && abs(diff) < judges.at(i).duration) {
        getData().result.incJudge(i, diff);
        effect.add<JudgeStrEffect>(note.lane, judges.at(i).name);
        note.inLong = true;
        note.beforeJudgeResult = true;
        break;
      }
    }
  } else if (note.isLongJudgeEnded) {
    //終点の判定
    if (!note.firstKeyStatus) {
      getData().result.incJudge(0, 0.0);
      effect.add<JudgeStrEffect>(note.lane, judges.at(0).name);
      note.beforeJudgeResult = true;
      note.isJudgeEnded = true;
    }
  } else {
    //途中の判定

    //判定区間内にいるとき
    double fromStartSecond = getJudgeDiff(note.second);
    double nowJudgeDurationStart = note.inLongJudgeIndex * longNoteJudgeDuration;
    double nowJudgeDurationEnd = (note.inLongJudgeIndex + 1) * longNoteJudgeDuration;

    if (!note.firstKeyStatus && (nowJudgeDurationStart <= fromStartSecond && fromStartSecond < nowJudgeDurationEnd)) {
      getData().result.incJudge(0, 0.0);
      effect.add<JudgeStrEffect>(note.lane, judges.at(0).name);
      note.beforeJudgeResult = true;
      if (note.inLongJudgeIndex + 1 < floor((note.endSecond - note.second) / longNoteJudgeDuration)) {
        //インデックスが増やせないときは判定終わりにする
        note.inLongJudgeIndex++;
      }
      else {
        note.isLongJudgeEnded = true;
      }
    }
  }
}

double Game::getJudgeDiff(double noteSecond) {
  //早いときは負の値、遅いときは正の値になる
  //     ---> 早いときはjudgeOffsetを正の値に、遅いときは負の値にする 
  return rhythmManager.getSecond() - noteSecond + getData().getJudgeOffset();
}

void Game::excludeEndedNote() {
  double mostLooseJudge = judges.back().duration;
  for (auto& laneNotes : allNotes) {
    for (auto& note : laneNotes) {
      if (note.length == 0) {
        //判定が終わってなく、一番ゆるい判定内から外れていたらミスにする
        if (!note.isJudgeEnded && getJudgeDiff(note.second) > mostLooseJudge) {
          getData().result.incMiss();
          effect.add<JudgeStrEffect>(note.lane, miss.name);
          note.isJudgeEnded = true;
        }

        //ノートが描画範囲外に行ったら描画しないようにする
        if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
          note.isVisible = false;
        }
      }
      else {
        if (!note.inLong) {
          //始点のミス判定
          //始点の判定が、一番ゆるい判定内から外れていたらミスにする
          if (getJudgeDiff(note.second) > mostLooseJudge) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.firstKeyStatus = true;
            note.inLong = true;
            note.beforeJudgeResult = false;
          }

        } else if (note.isLongJudgeEnded) {
          //終点のミス判定
          //終点までの判定が終わってなくて、一番ゆるい判定内から外れていたらミスにする
          if (!note.isJudgeEnded && getJudgeDiff(note.endSecond) > mostLooseJudge) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.isJudgeEnded = true;
            note.beforeJudgeResult = false;
          }

        } else {
          //途中のミス判定

          //最初に押しっぱで入ったときは
          if (note.firstKeyStatus && !beforeKeyStatuses.at(note.lane)) {
            note.firstKeyStatus = false;
          }

          //現在の判定区間が終わったとき
          double nowJudgeDurationEnd = (note.inLongJudgeIndex + 1) * longNoteJudgeDuration;
          if (getJudgeDiff(note.second) >= nowJudgeDurationEnd) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.beforeJudgeResult = false;
            if (note.inLongJudgeIndex + 1 < floor((note.endSecond - note.second) / longNoteJudgeDuration)) {
              //インデックスが増やせないときは判定終わりにする
              note.inLongJudgeIndex++;
            }else{
              note.isLongJudgeEnded = true;
            }
          }
        }

        //ノートが描画範囲外に行ったら描画しないようにする
        if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::afterBottom) {
          note.isVisible = false;
        }
      }
      
    }
  }
}


void Game::draw() const {
  Quad({ centerX - upW / 2, upY }, { centerX + upW / 2, upY },
   { centerX + bottomW / 2, bottomY }, { centerX - bottomW / 2, bottomY }).draw(laneBackColor);

  for (auto& i : vLines) {
    i.draw();
  }

  Line({ centerX - judgeLineW / 2.0, judgeLineY },
    { centerX + judgeLineW / 2.0, judgeLineY }).draw(4, Color(220, 30, 30));

  drawLaneEffect();
  drawNotes();
  effect.update();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}

void Game::drawLaneEffect() const {
  for (size_t i = 0; i < laneKeys.size(); ++i) {
    if (laneKeys.at(i).pressed()) {
      Quad(
        { getNoteStartX(judgeLineY - laneEffectLength, i),judgeLineY - laneEffectLength },
        { getNoteEndX(judgeLineY - laneEffectLength, i),judgeLineY - laneEffectLength },
        { getNoteEndX(judgeLineY,i),judgeLineY },
        { getNoteStartX(judgeLineY,i),judgeLineY })(laneEffect).draw(Color(255,64));
    }
  }
}

void Game::drawNotes() const {
  for (const auto& laneNotes : { allNotes.at(0), allNotes.at(5) ,allNotes.at(1) ,allNotes.at(2) ,allNotes.at(3) ,allNotes.at(4) }) {
    for (const auto& note : laneNotes) {
      if (note.isVisible) {
        if (getNoteDrawStatus(note.second) != NoteDrawStatus::before) {
          if (note.length == 0) {
            drawNormalNote(note);
          } else {
            drawLongNote(note);
          }
        } else {
          //描画範囲外(before)になったらbreak
          break;
        }
      }
    }
  }
}

void Game::drawNormalNote(const NoteData& note) const {
  getNoteQuad(note.lane, note.second).draw((note.lane == 0 || note.lane == 5) ? Color(0, 255, 0) : Color(255, 255, 255));
}

void Game::drawLongNote(const NoteData& note) const {
  getLongQuad(note).draw((note.beforeJudgeResult)? LNHalfwayActiveColor : LNHalfwayMissColor);

  //長押し始点
  if (getNoteDrawStatus(note.endSecond) != NoteDrawStatus::afterJudgeLine) {
    if (getNoteDrawStatus(note.second) == NoteDrawStatus::within) {
      getNoteQuad(note.lane, note.second).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor);
    }
    else if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterJudgeLine) {
      if (note.beforeJudgeResult) {
        getNoteQuad(note.lane, rhythmManager.getSecond()).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor);
      }
      else {
        getNoteQuad(note.lane, note.second).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor);
      }
    } else if (note.beforeJudgeResult && getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
      getNoteQuad(note.lane, rhythmManager.getSecond()).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor);
    }
  }

  //長押し終点
  if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::within || 
    (!note.beforeJudgeResult && getNoteDrawStatus(note.endSecond) == NoteDrawStatus::afterJudgeLine)) {
    getNoteQuad(note.lane, note.endSecond).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor);
  }
}

Quad Game::getNoteQuad(int lane, double second) const {
  int noteCenterY = getNoteY(second);
  int noteHeight = getNoteHeight(second);
  int noteUpY = static_cast<int>(noteCenterY - noteHeight / 2.0), noteBottomY = static_cast<int>(noteCenterY + noteHeight/2.0);

  return Quad(
    { getNoteStartX(noteUpY,lane),noteUpY },
    { getNoteEndX(noteUpY,lane),noteUpY },
    { getNoteEndX(noteBottomY,lane),noteBottomY },
    { getNoteStartX(noteBottomY,lane),noteBottomY }
  );
}

Quad Game::getLongQuad(const NoteData& note) const {
  int longNoteUpY = 0;
  if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::before) {
    longNoteUpY = upY;
  } else {
    longNoteUpY = getNoteY(note.endSecond);
  }

  int longNoteBottomY = 0;
  if (note.beforeJudgeResult) {
    longNoteBottomY = judgeLineY;
  } else {
    if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
      longNoteBottomY = bottomY;
    } else {
      longNoteBottomY = getNoteY(note.second);
    }
  }

  //判定終わったロングノーツが下まで行く問題の解決
  if (longNoteBottomY < longNoteUpY) {
    longNoteUpY = longNoteBottomY;
  }

  return Quad(
    { getNoteStartX(longNoteUpY,note.lane),longNoteUpY },
    { getNoteEndX(longNoteUpY,note.lane),longNoteUpY },
    { getNoteEndX(longNoteBottomY,note.lane),longNoteBottomY },
    { getNoteStartX(longNoteBottomY,note.lane),longNoteBottomY });
}

int Game::getNoteY(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return  static_cast<int>(convertRange(0,1, noteYFunc(arg),upY,bottomY));
}

int Game::getNoteHeight(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return  static_cast<int>(convertRange(0, 1, noteYFunc(arg), startNoteH, endNoteH));
}

int Game::getNoteStartX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 0;
  else if (lane == 5) index = 2;
  else index = lane - 1;

  return static_cast<int>(convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x));
}

int Game::getNoteEndX(int y, int lane) const {
  size_t index;
  if (lane == 0) index = 2;
  else if (lane == 5) index = 4;
  else index = lane;

  return static_cast<int>(convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x));
}

NoteDrawStatus Game::getNoteDrawStatus(double t) const {
  double diff = rhythmManager.getSecond() + toJudgeLineNoteSpeed - t;
 
  if (diff < 0) {
    return NoteDrawStatus::before;
  } else if ( diff > toBottomNoteSpeed) {
    return NoteDrawStatus::afterBottom;
  } else if (diff > toJudgeLineNoteSpeed) {
    return NoteDrawStatus::afterJudgeLine;
  }

  return NoteDrawStatus::within;
}