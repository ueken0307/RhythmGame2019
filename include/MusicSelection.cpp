#include "MusicSelection.hpp"

MusicSelection::MusicSelection(const InitData& init) : IScene(init), font(30), downPlusKey(false), downMinusKey(false) {

}


void MusicSelection::update() {
  if (KeyEnter.down()) {
    if (getData().infos.at(getData().getSelected()).getPlayLevels().at(getData().getLevelNum()) != 0) {
      this->changeScene(State::AdjustSpeed);
    }
  }

  if (KeyDown.pressed()) {
    if (!downPlusKey) {
      getData().incSelected();
      DEBUG_PRINTF("%zd\n", getData().getSelected());
    } else {
      DEBUG_PRINTF("%lf\n", KeyDown.pressedDuration().count());
    }
    
    downPlusKey = true;
  } else {
    downPlusKey = false;
  }

  if (KeyUp.pressed()) {
    if (!downMinusKey) {
      getData().decSelected();
      DEBUG_PRINTF("%zd\n", getData().getSelected());
    }
    else {
      DEBUG_PRINTF("%lf\n", KeyUp.pressedDuration().count());
    }

    downMinusKey = true;
  } else {
    downMinusKey = false;
  }

  if (KeyLeft.down()) {
    getData().decLeveNum();
  }

  if (KeyRight.down()) {
    getData().incLevelNum();
  }
}

void MusicSelection::draw() const {
  const auto info = getData().infos.at(getData().getSelected());

  Rect(200, 200, 200, 200)(TextureAsset(info.getAssetName())).draw();
  font(info.getTitle()).draw(420, 200);
  font(info.getArtist()).draw(420, 240);
  font(info.getBpm()).draw(420, 280);
  for (int i = 0; i < info.getPlayLevels().size(); ++i) {
    font(info.getPlayLevels().at(i)).draw(420 + i*40, 320,(i == getData().getLevelNum())? Color(255,0,0): Color(255,255,255));
  }
  font(info.getFolderPath()).draw(420, 360);

  font(U"‘I‹È").draw();
}

void MusicSelection::drawFadeIn(double t) const {
  draw();
}

void MusicSelection::drawFadeOut(double t) const {
  draw();
}