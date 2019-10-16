#include <Siv3D.hpp>
#include <Windows.h>
#include <assert.h>

#include "ShareData.hpp"
#include "State.hpp"

#include "MusicSelection.hpp"
#include "AdjustSpeed.hpp"
#include "Game.hpp"
#include "Result.hpp"

void Main() {

#ifdef _DEBUG
  //デバッグ時のみコンソール出力を有効にする
  AllocConsole();
  FILE *out = NULL, *in = NULL;
  freopen_s(&out, "CON", "w", stdout);
  freopen_s(&in, "CON", "r", stdin);
  assert(out != NULL);
  assert(in != NULL);
#endif // _DEBUG

  //----------
  //----------
  SceneManager<State, ShareData> sm;
  sm
    .add<MusicSelection>(State::MusicSelection)
    .add<AdjustSpeed>(State::AdjustSpeed)
    .add<Game>(State::Game)
    .add<Result>(State::Result);
  //----------  

  while (s3d::System::Update()) {
    sm.update();
    sm.drawScene();
  }


#ifdef _DEBUG
  fclose(out);
  fclose(in);
  FreeConsole();
#endif // _DEBUG

}