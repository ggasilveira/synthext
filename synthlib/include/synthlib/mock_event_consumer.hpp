#pragma once
#include "synthlib/event_consumer.hpp"
#include <exception>
#include <string>
#include <vector>

namespace synthlib::mock {

/// The type of mock event consumed.
enum class EventKind : uint8_t {
  PlayNote,
  WaitBeats,
  // WaitTicks,
  ChangeInstrument,
  SetBpm,
};

static inline const char *event_kind_name(EventKind kind) {
  switch (kind) {
  case EventKind::PlayNote:
    return "PlayNote";
  case EventKind::WaitBeats:
    return "WaitBeats";
  case EventKind::ChangeInstrument:
    return "ChangeInstrument";
  case EventKind::SetBpm:
    return "SetBpm";
  }
  return "";
}

/// Exception thrown when accessing data from a MockEvent with the wrong
/// EventKind
struct WrongEventKind : std::exception {
public:
  WrongEventKind(EventKind exp, EventKind got) : expected(exp), got(got) {
    error_msg += "expected: ";
    error_msg += event_kind_name(expected);
    error_msg += ", got: ";
    error_msg += event_kind_name(got);
  }
  EventKind expected;
  EventKind got;
  const char *what() const noexcept override { return error_msg.c_str(); }

private:
  std::string error_msg;
};

struct PlayNote {
  PlayNote(Channel channel, Note note, Octave octave, Volume volume)
      : channel(channel), note(note), octave(octave), volume(volume) {}
  Channel channel;
  Note note;
  Octave octave;
  Volume volume;
};
bool operator==(const PlayNote &lhs, const PlayNote &rhs) {
  return lhs.channel == rhs.channel && lhs.note == rhs.note &&
         lhs.octave == rhs.octave && lhs.volume == rhs.volume;
}

struct WaitBeats {
  WaitBeats(unsigned int beats) : beats(beats) {}
  unsigned int beats;
};
bool operator==(const WaitBeats &lhs, const WaitBeats &rhs) {
  return lhs.beats == rhs.beats;
}

struct ChangeInstrument {
  ChangeInstrument(Channel channel, Instrument instr)
      : channel(channel), instr(instr) {}
  Channel channel;
  Instrument instr;
};
bool operator==(const ChangeInstrument &lhs, const ChangeInstrument &rhs) {
  return lhs.channel == rhs.channel && lhs.instr.midi() == rhs.instr.midi();
}
struct SetBpm {
  SetBpm(Bpm bpm) : bpm(bpm) {}
  Bpm bpm;
};
bool operator==(const SetBpm &lhs, const SetBpm &rhs) {
  return lhs.bpm == rhs.bpm;
}

/// Mock events stored in the MockEventConsumer history
class MockEvent {
public:
  /// Create a PlayNote event
  MockEvent(PlayNote evt) : _kind(EventKind::PlayNote), _play_note(evt) {}
  /// Create a WaitBeats event
  MockEvent(WaitBeats evt) : _kind(EventKind::WaitBeats), _wait_beats(evt) {}
  /// Create a WaitTicks event
  // MockEvent(WaitTicks evt) : _kind(EventKind::WaitTicks), _wait_ticks(evt) {}
  /// Create a ChangeInstrument event
  MockEvent(ChangeInstrument evt)
      : _kind(EventKind::ChangeInstrument), _change_instrument(evt) {}
  /// Create a SetBpm event
  MockEvent(SetBpm evt) : _kind(EventKind::SetBpm), _set_bpm(evt) {}
  /// Returns the event type
  EventKind kind() { return _kind; }
  /// Return PlayNote event data
  PlayNote as_play_note() {
    check_kind(EventKind::PlayNote);
    return _play_note;
  }
  /// Return WaitBeats event data
  WaitBeats as_wait_beats() {
    check_kind(EventKind::WaitBeats);
    return _wait_beats;
  }
  /// Return WaitTicks event data
  // WaitTicks as_wait_ticks() {
  //   check_kind(EventKind::WaitTicks);
  //   return _wait_ticks;
  // }
  /// Return ChangeInstrument event data
  ChangeInstrument as_change_instrument() {
    check_kind(EventKind::ChangeInstrument);
    return _change_instrument;
  }
  /// Return SetBpm event data
  SetBpm as_set_bpm() {
    check_kind(EventKind::SetBpm);
    return _set_bpm;
  }
  void check_kind(EventKind expected_kind) {
    if (_kind != expected_kind) {
      throw WrongEventKind(expected_kind, _kind);
    }
  }

private:
  EventKind _kind;
  union {
    PlayNote _play_note;
    WaitBeats _wait_beats;
    // WaitTicks _wait_ticks;
    ChangeInstrument _change_instrument;
    SetBpm _set_bpm;
  };
};

/// Mock track to store events sent to it
class MockTrack {
public:
  /// Returns the nth event sent to this track
  MockEvent nth_event(unsigned int nth) const { return _events.at(nth); }
  /// Pushes an event
  void push_event(MockEvent evt) { _events.push_back(evt); }

private:
  std::vector<MockEvent> _events;
};
/// Mock event consumer for testing event producers
class MockEventConsumer : public IEventConsumer {
public:
  /// Creates an empty MockEventConsumer
  MockEventConsumer() {}
  void change_track(unsigned int track_num) override {
    _curr_track = track_num;
    // extend the track vector to fit the new track
    if (_curr_track >= _tracks.size()) {
      _tracks.resize(_curr_track + 1);
    }
  }
  void play_note(Channel channel, Note note, Octave octave,
                 Volume volume) override {
    _tracks.at(_curr_track)
        .push_event(MockEvent(PlayNote(channel, note, octave, volume)));
  }

  void wait_beats(unsigned int n) override {
    _tracks.at(_curr_track).push_event(MockEvent(WaitBeats(n)));
  }
  void change_instrument(Channel channel, Instrument instr) override {
    _tracks.at(_curr_track)
        .push_event(MockEvent(ChangeInstrument(channel, instr)));
  }
  void set_bpm(Bpm bpm) override {
    _tracks.at(_curr_track).push_event(MockEvent(SetBpm(bpm)));
  }

  /// Returns the nth track
  const MockTrack &nth_track(unsigned int track_num) const {
    return _tracks.at(track_num);
  }

private:
  std::vector<MockTrack> _tracks;
  unsigned int _curr_track = -1;
};
} // namespace synthlib::mock
