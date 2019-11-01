#include "Game.hpp"


//*** Lane ***
constexpr int centerX = 1920 / 2;
constexpr int upY = 0;
constexpr int bottomY = 1080;
constexpr int upW = 250;
constexpr int bottomW = 1400;
constexpr double laneLineThickness = 2.0;

constexpr Color laneLineColor(255,255,255);

//Background
constexpr Color laneBackColor(40, 40, 40);
constexpr Quad laneBackQuad({ centerX - upW / 2, upY }, { centerX + upW / 2, upY },
  { centerX + bottomW / 2, bottomY }, { centerX - bottomW / 2, bottomY });


//*** Judge Line ***
constexpr int judgeLineY = 1000;
constexpr double judgeLineW = convertRange(upY,bottomY,judgeLineY,upW,bottomW);
constexpr double judgeLineThickness = 4.0;

constexpr Line judgeLine({ centerX - judgeLineW / 2.0, judgeLineY },{ centerX + judgeLineW / 2.0, judgeLineY });
constexpr Color judgeLineColor(220, 30, 30);

//*** Note ***
constexpr int startNoteH = 5;
constexpr int endNoteH = 40;
constexpr double noteFrameThickness = 2.0;
constexpr Color noteFrameColor(40, 40, 40);

//long
constexpr double longNoteJudgeDuration = 0.25;

constexpr Color LNSEActiveColor(40, 100, 200); //�����O�m�[�c�̎n�_�ƏI�_�̔��萬�����̃J���[
constexpr Color LNSEMissColor(20, 50, 100); //�����O�m�[�c�̎n�_�ƏI�_�̔��莸�s�����̃J���[
constexpr Color LNHalfwayActiveColor(80, 200, 200); //�����O�m�[�c�̓r���̔��萬�����̃J���[
constexpr Color LNHalfwayMissColor(40, 100, 100); //�����O�m�[�c�̓r���̔��莸�s�����̃J���[

//slide
constexpr int arrowNum = 5;

//*** Lane Effect ***
constexpr int laneEffectLength = 500;


//*** Judge Effect ***
constexpr double judgeEffectSecond = 0.25;
constexpr int judgeEffectStartY = 400;
constexpr int judgeEffectDstY = judgeEffectStartY - 100;
constexpr int judgeEffectW = convertRange(upY,bottomY,judgeEffectStartY,upW,bottomW);

//*** MusicStatus ***
constexpr int musicStatusX = 0;
constexpr int musicStatusY = 50;
constexpr int musicStatusW = 525;
constexpr int musicStatusH = 175;
constexpr int musicStatusFrameThickness = 4.0;
constexpr int musicStatusJacketSize = 125;
constexpr int musicStatusJacketY = musicStatusY + (musicStatusH - musicStatusJacketSize) / 2;
constexpr int musicStatusJacketX = musicStatusX + (musicStatusH - musicStatusJacketSize) / 2;
constexpr Rect musicStatus({musicStatusX, musicStatusY}, {musicStatusW, musicStatusH});
constexpr Rect musicStatusJacket({musicStatusJacketX, musicStatusJacketY}, {musicStatusJacketSize, musicStatusJacketSize});
constexpr Color musicStatusColor(40, 128), musicStatusFrameColor(20);


struct JudgeStrEffect : IEffect {
  int lane;
  String judgeAssetName;

  JudgeStrEffect(const int lane, const String judgeAssetName)
    : lane(lane), judgeAssetName(judgeAssetName) {
  }

  bool update(double t) override {
    if (1 <= lane && lane <= 4) {
      double cX = (centerX - (judgeEffectW / 2) + judgeEffectW /8) + (lane-1)*(judgeEffectW / 4);
      double cY = judgeEffectStartY + (judgeEffectDstY - judgeEffectStartY) * (t/judgeEffectSecond);
      TextureAsset(judgeAssetName).scaled(0.3).drawAt(cX, cY);
    }
    return t < judgeEffectSecond;
  }
};


// (0 < input < 1)  (0 < output < 1)
double noteYFunc(double input) {
  double start = 1.7 * s3d::Math::Pi;
  double end = 2.0 * s3d::Math::Pi;
  double x = per2v(start, end, input);
  double tmp = abs(sin(end) - sin(start));
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

Game::Game(const InitData& init) : IScene(init), font(20), isStart(false), isMusicStarted(false) {
  JSONReader reader(getData().getScoreFileName());
  DEBUG_PRINTF("%s",getData().getScoreFileName().narrow().c_str());
  if (!reader) {
    //���ʃt�@�C���J���Ȃ������Ƃ��̏���
  }

  //---------bpm---------
  std::vector<BpmData> bpms;
  for (const auto& i : reader[U"bpms"].arrayView()) {
    bpms.push_back(BpmData(i[U"time"].get<int32>(), i[U"bpm"].get<double>(), i[U"beat"].get<int32>()));
  }

  //���ʂ̃I�t�Z�b�g
  double scoreOffset = getData().getSelectedInfo().getOffset();
  //�J�n����
  int startMeasure = 0;
  //���ʂ��n�܂�O�̂̎���
  double beforeSec = 1.0;

  //RhythmManager�̏�����
  rhythmManager = RhythmManager(bpms,scoreOffset,startMeasure,beforeSec);

  //---------note--------
  int totalNotes = 0;
  for (const auto& i : reader[U"notes"].arrayView()) {
    if (i[U"length"].get<int32>() != 0 && (i[U"lane"].get<int32>() == 0 || i[U"lane"].get<int32>() == 5)) {
      //�����O�m�[�c�Œ[�����̃��[���̏ꍇ�͒ʏ�m�[�c�ɕύX����
      allNotes.at(i[U"lane"].get<int32>()).push_back(
        NoteData(
          i[U"time"].get<int32>(),
          rhythmManager.BtoS(i[U"time"].get<int32>()),
          rhythmManager.BtoS(i[U"time"].get<int32>() + 0),
          i[U"lane"].get<int32>(),
          0
        )
      );
    } else {
      allNotes.at(i[U"lane"].get<int32>()).push_back(
        NoteData(
          i[U"time"].get<int32>(),
          rhythmManager.BtoS(i[U"time"].get<int32>()),
          rhythmManager.BtoS(i[U"time"].get<int32>() + i[U"length"].get<int32>()),
          i[U"lane"].get<int32>(),
          i[U"length"].get<int32>()
        )
      );
    }
    totalNotes++;
  }

  //��������totalNotes����
  for (const auto& laneNotes : allNotes) {
    for (const auto& note : laneNotes) {
      if (note.length != 0) {
        //�I�_ + �r��
        int longCounts = static_cast<int>(floor((note.endSecond - note.second) / longNoteJudgeDuration));
        totalNotes += 1 + ((longCounts == 0)? 1 : longCounts);
      }
    }
  }

  getData().result.reset(totalNotes);

  //���y�̃��[�h�A�����ʒu�ݒ�
  AudioAsset::Preload(getData().getSelectedInfo().getAssetName());
  AudioAsset(getData().getSelectedInfo().getAssetName()).setLoop(false);
  AudioAsset(getData().getSelectedInfo().getAssetName()).setPosSec(rhythmManager.getMusicInitPos());

  //�^�b�v���̃��[�h
  AudioAsset::Preload(U"tap");

  //�G�t�F�N�g�摜�̃v�����[�h
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

  getData().drawBackground.random();
}


void Game::update() {
  getData().drawBackground.update();

  if (isStart) {
    rhythmManager.update();

    if (!isMusicStarted && rhythmManager.getSecond() >= rhythmManager.getMusicStartSec()) {
      isMusicStarted = true;
      AudioAsset(getData().getSelectedInfo().getAssetName()).play();
    }

    input();
    excludeEndedNote();

    /*
    for (auto& laneNotes : allNotes) {
      for (auto& note : laneNotes) {
        if (rhythmManager.getStartMeasure() * 9600 <= note.count && !note.isJudgeEnded && abs(note.second - rhythmManager.getSecond()) <= (1 / 60.0) * 2) {
          AudioAsset(U"tap").stop();
          AudioAsset(U"tap").play();
          note.isJudgeEnded = true;
        }
      }
    }*/

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
        //(�܂�����͈͓̔��ɓ����ĂȂ�)
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
    //�n�_
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
    //�I�_�̔���
    if (!note.firstKeyStatus) {
      getData().result.incJudge(0, 0.0);
      effect.add<JudgeStrEffect>(note.lane, judges.at(0).name);
      note.beforeJudgeResult = true;
      note.isJudgeEnded = true;
    }
  } else {
    //�r���̔���

    //�����ԓ��ɂ���Ƃ�
    double fromStartSecond = getJudgeDiff(note.second);
    double nowJudgeDurationStart = note.inLongJudgeIndex * longNoteJudgeDuration;
    double nowJudgeDurationEnd = (note.inLongJudgeIndex + 1) * longNoteJudgeDuration;

    if (!note.firstKeyStatus && (nowJudgeDurationStart <= fromStartSecond && fromStartSecond < nowJudgeDurationEnd)) {
      getData().result.incJudge(0, 0.0);
      effect.add<JudgeStrEffect>(note.lane, judges.at(0).name);
      note.beforeJudgeResult = true;
      if (note.inLongJudgeIndex + 1 < floor((note.endSecond - note.second) / longNoteJudgeDuration)) {
        //�C���f�b�N�X�����₹�Ȃ��Ƃ��͔���I���ɂ���
        note.inLongJudgeIndex++;
      }
      else {
        note.isLongJudgeEnded = true;
      }
    }
  }
}

double Game::getJudgeDiff(double noteSecond) {
  //�����Ƃ��͕��̒l�A�x���Ƃ��͐��̒l�ɂȂ�
  //     ---> �����Ƃ���judgeOffset�𐳂̒l�ɁA�x���Ƃ��͕��̒l�ɂ��� 
  return rhythmManager.getSecond() - noteSecond + getData().getTotalOffset();
}

void Game::excludeEndedNote() {
  double mostLooseJudge = judges.back().duration;
  for (auto& laneNotes : allNotes) {
    for (auto& note : laneNotes) {
      if (note.length == 0) {
        //���肪�I����ĂȂ��A��Ԃ�邢���������O��Ă�����~�X�ɂ���
        if (!note.isJudgeEnded && getJudgeDiff(note.second) > mostLooseJudge) {
          getData().result.incMiss();
          effect.add<JudgeStrEffect>(note.lane, miss.name);
          note.isJudgeEnded = true;
        }

        //�m�[�g���`��͈͊O�ɍs������`�悵�Ȃ��悤�ɂ���
        if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
          note.isVisible = false;
        }
      }
      else {
        if (!note.inLong) {
          //�n�_�̃~�X����
          //�n�_�̔��肪�A��Ԃ�邢���������O��Ă�����~�X�ɂ���
          if (getJudgeDiff(note.second) > mostLooseJudge) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.firstKeyStatus = true;
            note.inLong = true;
            note.beforeJudgeResult = false;
          }

        } else if (note.isLongJudgeEnded) {
          //�I�_�̃~�X����
          //�I�_�܂ł̔��肪�I����ĂȂ��āA��Ԃ�邢���������O��Ă�����~�X�ɂ���
          if (!note.isJudgeEnded && getJudgeDiff(note.endSecond) > mostLooseJudge) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.isJudgeEnded = true;
            note.beforeJudgeResult = false;
          }

        } else {
          //�r���̃~�X����

          //�ŏ��ɉ������ςœ������Ƃ���
          if (note.firstKeyStatus && !beforeKeyStatuses.at(note.lane)) {
            note.firstKeyStatus = false;
          }

          //���݂̔����Ԃ��I������Ƃ�
          double nowJudgeDurationEnd = (note.inLongJudgeIndex + 1) * longNoteJudgeDuration;
          if (getJudgeDiff(note.second) >= nowJudgeDurationEnd) {
            getData().result.incMiss();
            effect.add<JudgeStrEffect>(note.lane, miss.name);
            note.beforeJudgeResult = false;
            if (note.inLongJudgeIndex + 1 < floor((note.endSecond - note.second) / longNoteJudgeDuration)) {
              //�C���f�b�N�X�����₹�Ȃ��Ƃ��͔���I���ɂ���
              note.inLongJudgeIndex++;
            }else{
              note.isLongJudgeEnded = true;
            }
          }
        }

        //�m�[�g���`��͈͊O�ɍs������`�悵�Ȃ��悤�ɂ���
        if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::afterBottom) {
          note.isVisible = false;
        }
      }
      
    }
  }
}


void Game::draw() const {
  getData().drawBackground.draw();

  musicStatus.draw(musicStatusColor).drawFrame(musicStatusFrameThickness, musicStatusFrameColor);
  musicStatusJacket(TextureAsset(getData().getSelectedInfo().getAssetName())).draw();
  font(getData().getSelectedInfo().getTitle()).draw(musicStatusJacketX + musicStatusJacketSize + 10, musicStatusJacketY + 20);
  font(getData().getSelectedInfo().getArtist()).draw(musicStatusJacketX + musicStatusJacketSize + 40, musicStatusJacketY + 70);

  laneBackQuad.draw(laneBackColor);

  for (const auto& i : vLines) { i.draw(laneLineThickness, laneLineColor); }
  judgeLine.draw(judgeLineThickness,judgeLineColor);

  drawLaneEffect();

  drawNotes();

  effect.update();
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
          //�`��͈͊O(before)�ɂȂ�����break
          break;
        }
      }
    }
  }
}

void Game::drawNormalNote(const NoteData& note) const {
  Quad noteQuad = getNoteQuad(note.lane, note.second);
  noteQuad.draw((note.lane == 0 || note.lane == 5) ? Color(0, 255, 0) : Color(255, 255, 255)).drawFrame(noteFrameThickness, noteFrameColor);

  double noteUpW = noteQuad.p1.x - noteQuad.p0.x;
  double arrowGap = noteUpW / (arrowNum * 2);
  double arrowW = noteUpW / (arrowNum * 2);
  double arrowH = getNoteHeight(note.second) * 4.0;

  if (note.lane == 0) {
    for (int i = 0; i < arrowNum; ++i) {
      Triangle({noteQuad.p0.x + i * (arrowGap + arrowW), noteQuad.p0.y - (arrowH / 2)}, { noteQuad.p0.x + i * (arrowGap + arrowW) + arrowW , noteQuad.p0.y - arrowH },
        { noteQuad.p0.x + i * (arrowGap + arrowW) + arrowW , noteQuad.p0.y }).draw(Color(0, 255, 0)).drawFrame(noteFrameThickness, noteFrameColor);;
    }
    //Triangle({ getNoteStartX(noteQuad.p0.y - getNoteHeight(note.second) * 8.0 ,0), noteQuad.p0.y - getNoteHeight(note.second) * 8.0 }, noteQuad.p3, { noteQuad.p3.x - getNoteHeight(note.second) * 8.0,noteQuad.p3.y }).draw(Color(0, 255, 0));
  }else if(note.lane == 5) {
    for (int i = 0; i < arrowNum; ++i) {
      Triangle({ noteQuad.p1.x - i * (arrowGap + arrowW), noteQuad.p0.y - (arrowH / 2) }, { noteQuad.p1.x - i * (arrowGap + arrowW) - arrowW , noteQuad.p0.y - arrowH },
        { noteQuad.p1.x - i * (arrowGap + arrowW) - arrowW , noteQuad.p0.y }).draw(Color(0, 255, 0)).drawFrame(noteFrameThickness, noteFrameColor);;
    }
    //Triangle({ getNoteEndX(noteQuad.p1.y - getNoteHeight(note.second) * 8.0 ,5), noteQuad.p1.y - getNoteHeight(note.second) * 8.0 }, noteQuad.p2, { noteQuad.p2.x + getNoteHeight(note.second) * 8.0,noteQuad.p2.y }).draw(Color(0, 255, 0));
  }
}

void Game::drawLongNote(const NoteData& note) const {
  getLongQuad(note).draw((note.beforeJudgeResult)? LNHalfwayActiveColor : LNHalfwayMissColor)
    .drawFrame(noteFrameThickness, noteFrameColor);

  //�������n�_
  if (getNoteDrawStatus(note.endSecond) != NoteDrawStatus::afterJudgeLine) {
    if (getNoteDrawStatus(note.second) == NoteDrawStatus::within) {
      getNoteQuad(note.lane, note.second).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor)
        .drawFrame(noteFrameThickness, noteFrameColor);
    }
    else if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterJudgeLine) {
      if (note.beforeJudgeResult) {
        getNoteQuad(note.lane, rhythmManager.getSecond()).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor)
          .drawFrame(noteFrameThickness, noteFrameColor);
      }
      else {
        getNoteQuad(note.lane, note.second).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor)
          .drawFrame(noteFrameThickness, noteFrameColor);
      }
    } else if (note.beforeJudgeResult && getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
      getNoteQuad(note.lane, rhythmManager.getSecond()).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor)
        .drawFrame(noteFrameThickness, noteFrameColor);
    }
  }

  //�������I�_
  if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::within || 
    (!note.beforeJudgeResult && getNoteDrawStatus(note.endSecond) == NoteDrawStatus::afterJudgeLine)) {
    getNoteQuad(note.lane, note.endSecond).draw((note.beforeJudgeResult) ? LNSEActiveColor : LNSEMissColor)
      .drawFrame(noteFrameThickness, noteFrameColor);
  }
}

Quad Game::getNoteQuad(int lane, double second) const {
  double noteCenterY = getNoteY(second);
  double noteHeight = getNoteHeight(second);
  double noteUpY = noteCenterY - noteHeight / 2.0, noteBottomY = noteCenterY + noteHeight / 2.0;

  return Quad(
    { getNoteStartX(noteUpY,lane),noteUpY },
    { getNoteEndX(noteUpY,lane),noteUpY },
    { getNoteEndX(noteBottomY,lane),noteBottomY },
    { getNoteStartX(noteBottomY,lane),noteBottomY }
  );
}

Quad Game::getLongQuad(const NoteData& note) const {
  double longNoteUpY = 0;
  if (getNoteDrawStatus(note.endSecond) == NoteDrawStatus::before) {
    longNoteUpY = upY;
  } else {
    longNoteUpY = getNoteY(note.endSecond);
  }

  double longNoteBottomY = 0;
  if (note.beforeJudgeResult) {
    longNoteBottomY = judgeLineY;
  } else {
    if (getNoteDrawStatus(note.second) == NoteDrawStatus::afterBottom) {
      longNoteBottomY = bottomY;
    } else {
      longNoteBottomY = getNoteY(note.second);
    }
  }

  //����I����������O�m�[�c�����܂ōs�����̉���
  if (longNoteBottomY < longNoteUpY) {
    longNoteUpY = longNoteBottomY;
  }

  return Quad(
    { getNoteStartX(longNoteUpY,note.lane),longNoteUpY },
    { getNoteEndX(longNoteUpY,note.lane),longNoteUpY },
    { getNoteEndX(longNoteBottomY,note.lane),longNoteBottomY },
    { getNoteStartX(longNoteBottomY,note.lane),longNoteBottomY });
}

double Game::getNoteY(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return  convertRange(0,1, noteYFunc(arg),upY,bottomY);
}

double Game::getNoteHeight(double t) const {
  double arg = (rhythmManager.getSecond() + toJudgeLineNoteSpeed - t) / toBottomNoteSpeed;
  return  convertRange(0, 1, noteYFunc(arg), startNoteH, endNoteH);
}

double Game::getNoteStartX(double y, int lane) const {
  size_t index;
  if (lane == 0) index = 0;
  else if (lane == 5) index = 2;
  else index = lane - 1;

  return convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x);
}

double Game::getNoteEndX(double y, int lane) const {
  size_t index;
  if (lane == 0) index = 2;
  else if (lane == 5) index = 4;
  else index = lane;

  return convertRange(upY, bottomY, y, vLines.at(index).begin.x, vLines.at(index).end.x);
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