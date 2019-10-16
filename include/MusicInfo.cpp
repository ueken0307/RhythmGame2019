#include "MusicInfo.hpp"

MusicInfo::MusicInfo(String folderName,String musicFileName,String title,String artist,String bpm,double offset,std::vector<int> playLevels, Texture texture) {
  this->folderName = folderName;
  this->musicFileName = musicFileName;
  this->title = title;
  this->artist = artist;
  this->bpm = bpm;
  this->offset = offset;

  for (const auto & i : playLevels) {
    this->playLevels.push_back(i);
  }

  this->texture = texture;
}