#include "Game.hpp"

Game::Game(const InitData& init) : IScene(init), font(30), isStart(false) {
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
  double startMeasure = 0;
  //���ʂ��n�܂�O�̂̎���
  double beforeSec = 1.0;

  //RhythmManager�̏�����
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

  //���y�̃��[�h�A�����ʒu�ݒ�
  AudioAsset::Preload(getData().getSelectedInfo().getAssetName());
  AudioAsset(getData().getSelectedInfo().getAssetName()).setPosSec(rhythmManager.getMusicInitPos());

  //�^�b�v���̃��[�h
  AudioAsset::Preload(U"tap");
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
  font(U"�Q�[��").draw();
}

void Game::drawFadeIn(double t) const {
  draw();
}

void Game::drawFadeOut(double t) const {
  draw();
}