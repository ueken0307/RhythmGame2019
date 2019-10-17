#include "Load.hpp"

Load::Load(const InitData& init) : IScene(init), font(10) {
  auto &infos = getData().infos;

  for (const auto& i : std::filesystem::directory_iterator(DEVELOP_MUSICS_PATH)) {
    String folderPath = Unicode::Widen(DEVELOP_MUSICS_PATH + i.path().filename().string());
    String infoPath = folderPath + U"/info.json";

    printf("%s\n", infoPath.narrow().c_str());

    JSONReader reader(infoPath);

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

    printf("  title  : %s\n"
           "  artist : %s\n"
           "  bpm    : %s\n"
           "  music  : %s\n"
           "  jacket : %s\n"
           "  offset : %lf\n"
           "  level  : %d,%d,%d\n\n",
      title.narrow().c_str(), artist.narrow().c_str(), bpm.narrow().c_str(), musicFileName.narrow().c_str(), 
      jacketFileName.narrow().c_str(), offset, playLevel[0], playLevel[1], playLevel[2]);
  }

  printf("%d musics was loaded.\n", (int)infos.size());
}

void Load::update() {
  if (KeyEnter.pressed()) {
    this->changeScene(State::Result);
  }
}

void Load::draw() const {
  font(U"ÉçÅ[Éh").draw();
}

void Load::drawFadeIn(double t) const {
  draw();
}

void Load::drawFadeOut(double t) const {
  draw();
}