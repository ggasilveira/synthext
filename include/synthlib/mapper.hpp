#pragma once

#include "synthlib/command.hpp"
#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace synthlib {

class ICommandIssuer {

public:
  virtual ~ICommandIssuer() = default;
  virtual auto process(char ascii) -> std::unique_ptr<ICommand> = 0;
};

class ParseMapper : public ICommandIssuer {
  char last_char = 0;

public:
  auto process(char ascii) -> std::unique_ptr<ICommand>;
};

class MapperState {
  char _last_char = 0;

public:
  friend class TableMapper;
  char last_char() const { return _last_char; }
};

using CommandCreator =
    std::function<std::unique_ptr<ICommand>(const MapperState &)>;

/// This class is able to define Mappings through a HashTable
class TableMapper : public ICommandIssuer {
  MapperState state;
  std::unordered_map<char, CommandCreator> mapping;
  CommandCreator default_command;

public:
  /// Creates a mapper with an empty rule table.
  TableMapper() = default;
  /// Creates a mapper with the rule table from the specification.
  static auto default_table() -> TableMapper;

  template <typename T> void add_simple(std::string_view keys, T command) {
    for (char key : keys) {
      CommandCreator creator = [command](const MapperState &stt) {
        return std::make_unique<T>(command);
      };
      mapping.emplace(key, creator);
    }
  }
  template <typename T> void add_simple(char key, T command) {
    CommandCreator creator = [command](const MapperState &stt) {
      return std::make_unique<T>(command);
    };
    mapping.emplace(key, creator);
  }
  void add_rule(char key, CommandCreator creator) {
    mapping.emplace(key, creator);
  }
  void add_rule(std::string_view keys, CommandCreator creator) {
    for (char key : keys) {
      mapping.emplace(key, creator);
    }
  }
  void set_default(CommandCreator creator) {
    default_command = std::move(creator);
  }
  auto process(char ascii) -> std::unique_ptr<ICommand> override;
};

} // namespace synthlib
