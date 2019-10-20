#include "MusicSelection.hpp"

MusicSelection::MusicSelection(const InitData& init) : IScene(init), font(30), downPlusKey(false), downMinusKey(false) {

}


void MusicSelection::update() {
  if (KeyEnter.down()) {
    this->changeScene(State::AdjustSpeed);
  }

  if (KeyDown.pressed()) {
    if (!downPlusKey) {
      getData().selected = (getData().selected + 1) % getData().infos.size();
      DEBUG_PRINTF("%d\n", getData().selected);
    } else {
      DEBUG_PRINTF("%lf\n", KeyDown.pressedDuration().count());
    }
    
    downPlusKey = true;
  } else {
    downPlusKey = false;
  }

  if (KeyUp.pressed()) {
    if (!downMinusKey) {
      getData().selected = (getData().selected == 0) ? (getData().selected + getData().infos.size() - 1) : (getData().selected - 1);
      DEBUG_PRINTF("%d\n", getData().selected);
    }
    else {
      DEBUG_PRINTF("%lf\n", KeyUp.pressedDuration().count());
    }

    downMinusKey = true;
  } else {
    downMinusKey = false;
  }

}

void MusicSelection::draw() const {
  const auto& info = getData().infos[getData().selected];

  Rect(200, 200, 200, 200)(TextureAsset(info.getAssetName())).draw();
  font(info.getTitle()).draw(420, 200);
  font(info.getArtist()).draw(420, 240);
  font(info.getBpm()).draw(420, 280);
  font(info.getPlayLevels()).draw(420, 320);
  font(info.getFolderPath()).draw(420, 360);

  font(U"‘I‹È").draw();
}

void MusicSelection::drawFadeIn(double t) const {
  draw();
}

void MusicSelection::drawFadeOut(double t) const {
  draw();
}