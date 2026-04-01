#pragma once

#include <string>



/// Provides a source of characters
class ITextProvider {

  public:
    virtual ~ITextProvider() {}
    virtual char next_char() = 0;
};


class StringTextProvider : public ITextProvider {
  std::string text;
  unsigned int idx = 0;
    
  public:
    StringTextProvider(std::string txt): text(txt), idx(0) {}

    virtual char next_char() {
      if (idx >= text.size()) {
        return '\0';
      } else {
        return text[idx++];
      }
    }
  
};
