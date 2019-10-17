#ifndef MUSIC_INFO_HPP
#define MUSIC_INFO_HPP

#include<Siv3D.hpp>

class MusicInfo {
public:
  MusicInfo(String folderPath, String title, String artist, String bpm, double offset, Array<int> playLevels, String assetName);

  String getFolderPath() const { return folderPath; };
  String getTitle() const { return title; };
  String getArtist() const { return artist; };
  String getBpm() const { return bpm; };
  double getOffset() const { return offset; };
  Array<int> getPlayLevels() const { return playLevels; };
  String getAssetName() const { return assetName; }
  

private:
  String folderPath;
  String title;
  String artist;
  String bpm;
  double offset;
  Array<int> playLevels;
  String assetName;

};

#endif // !MUSIC_INFO_HPP
