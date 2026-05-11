#include "synthlib/text.hpp"
#include <FL/Fl_Window.H>
#include <FL/platform_types.h>
#include <iostream>
#include <memory>
// #include <wx/event.h>
// #include <wx/wx.h>

// enum ids {
//   ID_LOAD_FILE,
//   ID_SAVE_FILE,
// };

// class SynthFrame : public wxFrame {
// public:
//   SynthFrame();

// private:
//   void onExit(wxCommandEvent& event);

//   void onLoadFile(wxEvent& event);
//   void onSaveFile(wxEvent& event);

// };


// class SynthApp : public wxApp {
// public:
//   bool OnInit() override {
//     auto *frame = new SynthFrame();
//     frame->Show();
//     return true;
//   }
// };

// SynthFrame::SynthFrame() : wxFrame(nullptr, wxID_ANY, "Hello world") {


//   auto *sizer = new wxBoxSizer(wxVERTICAL);
//   sizer->Add(new wxButton(this, ID_LOAD_FILE, "Load File"), 0, 0);

//   auto *text_edit = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
//   sizer->Add(text_edit);
//   SetSizer(sizer);

//   Bind(wxEVT_BUTTON, &SynthFrame::onLoadFile, this, ID_LOAD_FILE);



// }
// void SynthFrame::onExit(wxCommandEvent &event) {
//   Close(true);
// }
// void SynthFrame::onLoadFile(wxEvent &event) {
//   std::cout << "LFKSJLKFJLKJ\n";
// }
// void SynthFrame::onSaveFile(wxEvent &event) {
//   std::cout << "LFKSJLKFJLKJ\n";
// }



// wxIMPLEMENT_APP(SynthApp);


#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

#include "ui.hpp"


auto main() -> int {
  auto *app_window = new Fl_Double_Window(800, 600, "Synthext");

  auto *app = new SynthApp();
  app->build(*app_window);

  app_window->show();
  return Fl::run();
}
