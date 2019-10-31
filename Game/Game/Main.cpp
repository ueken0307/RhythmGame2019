#include <Siv3D.hpp>
#include <Windows.h>
#include <assert.h>

#include "ShareData.hpp"
#include "State.hpp"

#include "Load.hpp"
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
  Window::SetStyle(WindowStyle::Sizable);
  Scene::Resize({1920,1080});
  Window::Resize({1280,720}, WindowResizeOption::KeepSceneSize);
  //----------
  SceneManager<State, ShareData> sm;
  sm
    .add<Load>(State::Load)
    .add<MusicSelection>(State::MusicSelection)
    .add<AdjustSpeed>(State::AdjustSpeed)
    .add<Game>(State::Game)
    .add<Result>(State::Result);
  //----------  
  JSONReader reader(U"config.json");
  if (!reader) {
    throw Error(U"Cannot load config.json");
  }
  sm.get()->setMusicFolderPath(reader[U"MusicFolderPath"].get<String>());
  sm.get()->setGlobalOffset((1.0/60) * (reader[U"OffsetFrame"].get<int>()));

  if (reader[U"isFullScreen"].get<bool>()) {
    auto resolutions = Graphics::GetFullscreenResolutions();
    Window::SetFullscreen(true , resolutions.back(), WindowResizeOption::KeepSceneSize);
  }



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