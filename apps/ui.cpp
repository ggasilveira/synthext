#include "ui.hpp"
#include <FL/Fl_Flex.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string_view>


void SynthApp::on_save_text(Fl_Widget *widget) {
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Save File As...");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if (file_chooser.show() == 0) {
    text_buffer->savefile(file_chooser.filename());
  }
}
void SynthApp::on_load_text(Fl_Widget *widget) {
  std::cout << "LOAD TEXT\n";
}
void SynthApp::on_save_midi(Fl_Widget *widget) {
  std::cout << "SAVE MIDI\n";
}


void SynthApp::build(Fl_Window &window) {

  window.begin();

  // Putting the menu bar in the flex wasn't working for
  // some reason, so we're putting outside and calculating
  // the sizes manually. 
  build_menu_bar(window);

  // the dimensions of the frame which will hold all
  // the other widgets.
  int frame_x = 0;
  int frame_y = menu_bar->h();
  int frame_w = window.w();
  int frame_h = window.h() - menu_bar->h();

  frame = new Fl_Flex(frame_x, frame_y, frame_w, frame_h, Fl_Flex::ROW);

  build_text_editor();
  controls->build();
  

  frame->fixed(controls->root(), CONTROLS_WIDTH);
  frame->resizable(text_editor);
  frame->end();

  window.resizable(frame);
  window.end();

}


void SynthApp::build_menu_bar(Fl_Window &window) {
  menu_bar = new Fl_Menu_Bar(0, 0, window.w(), MENU_BAR_HEIGHT);
  menu_bar->add("File/Load Text", 0, SynthApp::on_load_text_cb, this);
  menu_bar->add("File/Save Text", 0, SynthApp::on_save_text_cb, this);
  menu_bar->add("File/Save MIDI", 0, SynthApp::on_save_midi_cb, this);
  
}

void SynthApp::build_text_editor() {
  text_buffer = new Fl_Text_Buffer();
  text_editor = new Fl_Text_Editor(0, 0, 0, 0);
  text_editor->buffer(text_buffer);
  text_editor->textfont(FL_COURIER);
  text_editor->textsize(EDITOR_FONT_SIZE);
  text_editor->cursor_style(Fl_Text_Display::SIMPLE_CURSOR);

  // set line numbering
  text_editor->linenumber_width(EDITOR_LINE_NUMBER_SIZE);
  text_editor->linenumber_format("%d");
}




