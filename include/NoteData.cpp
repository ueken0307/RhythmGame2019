#include"NoteData.hpp"

NoteData::NoteData(int count, double second, int lane, int length, bool isEndEffect, bool isLong,bool isEndLong) {
  this->count = count;
  this->second = second;
  this->lane = lane;
  this->length = length;
  this->isEndEffect = isEndEffect;
  this->isLong = isLong;
  this->isEndLong = isEndLong;
}