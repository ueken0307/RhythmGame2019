#include "MusicSelection.hpp"

constexpr int elemNum = 7;
constexpr int selectedElemY = 500;
constexpr int elemH = 180;
constexpr int elemGap = 60;
constexpr int elemStartX = 900;
constexpr int elemStartY = selectedElemY - static_cast<int>(elemH * ((elemNum/2) + 0.5)) - elemGap * (elemNum/2);

constexpr int selectedStartY = 400;
constexpr int selectedH = 200;
constexpr Color SelectedColor(0, 255, 250, 200);

constexpr int levelStartX = 50;
constexpr int levelStartY = 750;
constexpr int levelGap = 50;
constexpr int levelW = 200;
constexpr Color levelBackgroundColor(180);
constexpr int levelFrameLength = 50;
constexpr int levelFrameThickness = 3;
constexpr Color levelFrameColor(0, 255, 250);
constexpr Color levelFontColor = Color(40);

constexpr int jacketSize = 450;
constexpr int jacketStartX = levelStartX + levelGap + levelW * 1.5 - jacketSize / 2;
constexpr int jacketStartY = 225;
constexpr int jacketFrameLength = 50;
constexpr int jacketFrameThickness = 5;
constexpr int jacketFrameGap = 20;
constexpr Color jacketFrameColor(0, 255, 250);

constexpr int playerStartX = 25;
constexpr int playerStartY = 25;
constexpr int playerW = 400;
constexpr int playerH = 150;
constexpr Color playerBackgroundColor(180, 180, 180);

constexpr double moveDuration = 0.20;

MusicSelection::MusicSelection(const InitData& init) : IScene(init), font30(30), font60(60), moveStatus(MoveStatus::NotMove) {
  getData().drawBackground.random();
  getData().drawBackground.start();
  getData().drawGuide.set(std::vector<String>({U"��Փx��������",U"��Փx���グ��",U"���̋�",U"�O�̋�" }));
}


void MusicSelection::update() {
  getData().drawBackground.update();
  moveSec = stopwatch.sF();

  if (KeyEnter.down()) {
    if (getData().infos.at(getData().getSelected()).getPlayLevels().at(getData().getLevelNum()) != 0) {
      this->changeScene(State::AdjustSpeed);
    }
  }

  if (moveStatus == MoveStatus::NotMove && KeyJ.pressed()) {
    getData().incSelected();
    moveStatus = MoveStatus::Inc;
    stopwatch.restart();
  }

  if (moveStatus == MoveStatus::NotMove && KeyK.pressed()) {
    getData().decSelected();
    moveStatus = MoveStatus::Dec;
    stopwatch.restart();
  }

  //�ω��̎��Ԃ𒴂����Ƃ��͓������~�߂ăX�g�b�v�E�H�b�`�����Z�b�g
  if (moveDuration < moveSec) {
    stopwatch.reset();
    moveStatus = MoveStatus::NotMove;
  }

  if (KeyD.down()) {
    getData().decLeveNum();
  }

  if (KeyF.down()) {
    getData().incLevelNum();
  }
}

void MusicSelection::draw() const {
  const auto info = getData().infos.at(getData().getSelected());
  double e = EaseInOutSine(moveSec / moveDuration);
  
  Rect({0, selectedStartY}, {1920, selectedH }).draw(SelectedColor);

  // *** background ***
  getData().drawBackground.draw();

  // *** back ***
  Quad({0,0}, {elemStartX - 20,0}, {elemStartX - 70,1080}, {0,1080}).draw(Color(0,0,0,128));

  // *** player ***
  //Rect(playerStartX, playerStartY, playerW, playerH).draw(playerBackgroundColor);
  font60(ToString(getData().getPlayNum() + 1) + U"/" + ToString(getData().getMaxPlayNum()) +U" �Ȗ�").draw(playerStartX, playerStartY);

  // *** Jacket ***
  Rect(jacketStartX, jacketStartY, jacketSize, jacketSize)(TextureAsset(info.getAssetName())).draw();
  //Frame Lines
  Line({ jacketStartX + jacketSize + jacketFrameGap , jacketStartY - jacketFrameGap },
    { jacketStartX + jacketSize + jacketFrameGap - jacketFrameLength , jacketStartY - jacketFrameGap }).draw(jacketFrameThickness,jacketFrameColor);
  Line({ jacketStartX + jacketSize + jacketFrameGap , jacketStartY - jacketFrameGap },
    { jacketStartX + jacketSize + jacketFrameGap , jacketStartY - jacketFrameGap + jacketFrameLength }).draw(jacketFrameThickness, jacketFrameColor);
  Line({ jacketStartX - jacketFrameGap , jacketStartY + jacketSize + jacketFrameGap },
    { jacketStartX - jacketFrameGap , jacketStartY + jacketSize + jacketFrameGap - jacketFrameLength}).draw(jacketFrameThickness, jacketFrameColor);
  Line({ jacketStartX - jacketFrameGap , jacketStartY + jacketSize + jacketFrameGap },
    { jacketStartX - jacketFrameGap + jacketFrameLength, jacketStartY + jacketSize + jacketFrameGap }).draw(jacketFrameThickness, jacketFrameColor);

  // *** Level ***
  std::array<String, 3> levelStr({ U"���񂽂�", U"�ӂ�", U"�ނ�������" });
  for (int i = 0; i < info.getPlayLevels().size(); ++i) {
    if (info.getPlayLevels().at(i) != 0) {
      Quad({ levelStartX + i * (levelGap + levelW)         , levelStartY + levelW / 2 }, { levelStartX + i * (levelGap + levelW) + levelW / 2, levelStartY },
        { levelStartX + i * (levelGap + levelW) + levelW, levelStartY + levelW / 2 }, { levelStartX + i * (levelGap + levelW) + levelW / 2, levelStartY + levelW })
        .draw(levelBackgroundColor);

      font30(levelStr.at(i)).drawAt({ levelStartX + i * (levelGap + levelW) + levelW / 2 , levelStartY + levelW / 2.0 }, levelFontColor);
    }
    

    //Frame Lines
    if (i == getData().getLevelNum()) {
      double startX = levelStartX + i * (levelGap + levelW);
      double endX = levelStartX + i * (levelGap + levelW) + levelW;
      double startY = levelStartY;
      double endY = levelStartY + levelW;

      Line({ startX, startY }, { startX + levelFrameLength, startY }).draw(levelFrameThickness, levelFrameColor);
      Line({ startX, startY }, { startX , startY + levelFrameLength }).draw(levelFrameThickness, levelFrameColor);

      Line({ endX, startY }, { endX - levelFrameLength, startY }).draw(levelFrameThickness, levelFrameColor);
      Line({ endX, startY }, { endX, startY + levelFrameLength }).draw(levelFrameThickness, levelFrameColor);

      Line({ startX, endY }, { startX + levelFrameLength, endY }).draw(levelFrameThickness, levelFrameColor);
      Line({ startX, endY }, { startX, endY - levelFrameLength }).draw(levelFrameThickness, levelFrameColor);

      Line({ endX, endY }, { endX - levelFrameLength, endY }).draw(levelFrameThickness, levelFrameColor);
      Line({ endX, endY }, { endX, endY - levelFrameLength }).draw(levelFrameThickness, levelFrameColor);

    }

  }

  // *** Elements ***
  for (int i = 0; i < elemNum; ++i) {
    if (moveStatus != MoveStatus::NotMove) {
      Vec2  end(elemStartX, elemStartY + i * (elemH + elemGap));
      Vec2 start;
      if (moveStatus == MoveStatus::Inc) {
        //�C���N�������g�̂Ƃ��͉��̗̂v�f�̏ꏊ���瓮���Ă���
        start = Vec2(end.x,end.y + (elemH + elemGap));
      } else if (moveStatus == MoveStatus::Dec) {
        //�f�N�������g�̂Ƃ��͏�̗v�f�̏ꏊ���瓮���Ă���
        start = Vec2(end.x, end.y - (elemH + elemGap));
      }

      Vec2 pos = start.lerp(end, e);
      drawElem(pos.x, pos.y, getData().getInfoFromSelected(i - (elemNum / 2)));
    } else {
      drawElem(elemStartX, elemStartY + i * (elemH + elemGap),getData().getInfoFromSelected(i - (elemNum/2)));
    }
  }

  // *** Draw Guide ***
  getData().drawGuide.draw();
}


void MusicSelection::drawElem(int startX, int startY, MusicInfo& info) const{
  Rect(startX,startY,1920-startX,elemH)(TextureAsset(U"elem")).draw();
  font60(info.getTitle()).draw(startX + 50, startY + 20);
  font30(U"BPM:" + info.getBpm()).draw(startX + 40, startY + 120);
  font30(info.getArtist()).draw(startX + 300, startY + 120);

}