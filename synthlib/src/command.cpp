#include "synthlib/command.hpp"

namespace synthlib {

bool operator==(const Command &lhs, const Command &rhs) {
  if (lhs.kind() != rhs.kind()) {
    return false;
  }
  switch (lhs.kind()) {
  case CommandKind::PlayNote:
    return lhs.note() == rhs.note();
    break;
  case CommandKind::Delay:
  case CommandKind::IncreaseInstrument:
    return lhs.amount() == rhs.amount();
    break;
  case CommandKind::ChangeInstrument:
    return lhs.instrument().midi() == rhs.instrument().midi();
  default:
    return true;
  }
}

} // namespace synthlib
