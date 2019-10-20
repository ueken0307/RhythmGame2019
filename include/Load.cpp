#include "Load.hpp"

Load::Load(const InitData& init) : IScene(init), font(20) {
  auto &infos = getData().infos;

  if (!std::filesystem::exists(DEVELOP_MUSICS_PATH)) {
    throw Error(U"Cannot Open" + Unicode::Widen(DEVELOP_MUSICS_PATH));
  }

  for (const auto& i : std::filesystem::directory_iterator(DEVELOP_MUSICS_PATH)) {
    String folderPath = Unicode::Widen(DEVELOP_MUSICS_PATH + i.path().filename().string());
    String infoPath = folderPath + U"/info.json";

    DEBUG_PRINTF("%s\n", infoPath.narrow().c_str());

    JSONReader reader(infoPath);
    if (reader){
      String title = reader[U"title"].get<String>();
      String artist = reader[U"artist"].get<String>();
      String bpm = reader[U"bpm"].get<String>();
      String musicFileName = reader[U"musicFileName"].get<String>();
      String jacketFileName = reader[U"jacketFileName"].get<String>();
      double offset = reader[U"offset"].get<double>();
      Array<int> playLevel = reader[U"playLevel"].getArray<int>();

      String assetName = Unicode::Widen(i.path().filename().string());
      AudioAsset::Register(assetName, folderPath + U"/" + musicFileName);
      TextureAsset::Register(assetName, folderPath + U"/" + jacketFileName);

      infos.push_back(MusicInfo(
        folderPath,
        title,
        artist,
        bpm,
        offset,
        playLevel,
        assetName
      ));

      DEBUG_PRINTF("  title  : %s\n"
        "  artist : %s\n"
        "  bpm    : %s\n"
        "  music  : %s\n"
        "  jacket : %s\n"
        "  offset : %lf\n"
        "  level  : %d,%d,%d\n\n",
        title.narrow().c_str(), artist.narrow().c_str(), bpm.narrow().c_str(), musicFileName.narrow().c_str(),
        jacketFileName.narrow().c_str(), offset, playLevel[0], playLevel[1], playLevel[2]);
    } else {
      failList.push_back(infoPath);

      DEBUG_PRINTF("\n  ******** Failed ********\n\n");
    }
  }

  DEBUG_PRINTF("%d musics was loaded.\n", (int)infos.size());

  if (!failList.empty()) {
     DEBUG_PRINTF("Failed to load. : \n");

    for (const auto& i : failList) {
      DEBUG_PRINTF((U"  " + i).narrow().c_str());
    }
  }

}

void Load::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::MusicSelection);
  }
}

void Load::draw() const {
  font(ToString(getData().infos.size()) + U"ã»ÇÃì«Ç›çûÇ›Ç…ê¨å˜").draw(0,0, Color(40, 200, 40));

  if (!failList.empty()) {
    int start = 25;
    font(ToString(failList.size()) + U"ã»ÇÃì«Ç›çûÇ›Ç…é∏îs").draw(0, start, Color(200, 40, 40));

    for (const auto &i : failList) {
      start += 20;
      font(i).draw(20, start, Color(200, 40, 40));
    }
  }

}

void Load::drawFadeIn(double t) const {
  draw();
}

void Load::drawFadeOut(double t) const {
  draw();
}