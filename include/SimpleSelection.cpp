#include"SimpleSelection.hpp"

void SimpleSelection::init(){
  gui = GUI(GUIStyle::Default);
  gui.setTitle(L"読み込み");
  gui.add(GUIText::Create(L"フォルダ名"));
  gui.addln(L"folderName", GUITextField::Create(20));
  gui.add(GUIText::Create(L"ファイル名"));
  gui.addln(L"fileName", GUITextField::Create(20));
  gui.add(GUIText::Create(L"何小節から？"));
  gui.addln(L"measure", GUITextField::Create(4));
  gui.add(L"ok", GUIButton::Create(L"決定"));
  gui.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  gui.show(true);

  gui.textField(L"folderName").setText(m_data->folderName);
  gui.textField(L"fileName").setText(m_data->fileName);
  gui.textField(L"measure").setText(m_data->startMeasure>=1 ? ToString(m_data->startMeasure + 1) : L"1");
}

void SimpleSelection::update(){
  if (gui.button(L"ok").pushed) {
    if (JSONReader(L"Musics/" + gui.textField(L"folderName").text + L"/info.json").isOpened() &&
      JSONReader(L"Musics/" + gui.textField(L"folderName").text + L"/" + gui.textField(L"fileName").text + L".json")) {
      String folderName = gui.textField(L"folderName").text;
      String fileName = gui.textField(L"fileName").text;

      JSONReader reader(L"Musics/" + folderName + L"/info.json");

      m_data->folderName = folderName;
      m_data->fileName = fileName;
      m_data->offset = reader[L"offset"].get<double>();
      m_data->musicFileName = reader[L"musicFileName"].get<String>();
      m_data->judgeOffset = 0.0;
      m_data->speed = 10;
      m_data->autoFlag = true;
      m_data->nextScene = L"SimpleSelection";
      m_data->startMeasure = (Parse<int>(gui.textField(L"measure").text) >= 1) ? Parse<int>(gui.textField(L"measure").text)-1 : 0;

      changeScene(L"Game");
    }
  }
}

void SimpleSelection::draw() const{

}

void SimpleSelection::drawFadeIn(double t) const {
  draw();
  Transformer2D tr(Graphics2D::GetTransform().inverse());
  Window::ClientRect().draw(ColorF(0.0).setAlpha(1.0 - t));
}

void SimpleSelection::drawFadeOut(double t) const {
  draw();
  Transformer2D tr(Graphics2D::GetTransform().inverse());
  Window::ClientRect().draw(ColorF(0.0).setAlpha(t));
}