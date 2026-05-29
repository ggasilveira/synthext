#pragma once
#include "synthlib/event_consumer.hpp"
#include <exception>
#include <vector>

namespace synthlib::mock {

/// Mock Events stored in the MockEventConsumer history
enum class EventKind : uint8_t {
  ChangeTrack,
  PlayNote,
  WaitBeats,
  WaitTicks,
  ChangeInstrument,
  SetBpm,
};

class WrongEventKind : std::exception {
public:
  WrongEventKind(EventKind exp, EventKind got) : expected(exp), got(got) {}
  EventKind expected;
  EventKind got;
  const char *what() const noexcept override { return "wrong event kind"; }
};

struct ChangeTrack {
  unsigned int track_num;
};
struct PlayNote {
  Channel channel;
  Note note;
  Octave octave;
  Volume volume;
};
struct WaitBeats {
  unsigned int beats;
};
struct WaitTicks {
  unsigned int ticks;
};
struct ChangeInstrument {
  Channel channel;
  Instrument instr;
};
struct SetBpm {
  Bpm bpm;
};

/// Mock events stored in the MockEventConsumer history
class MockEvent {
public:
  MockEvent(ChangeTrack evt)
      : _kind(EventKind::ChangeTrack), _change_track(evt) {}
  MockEvent(PlayNote evt) : _kind(EventKind::PlayNote), _play_note(evt) {}
  MockEvent(WaitBeats evt) : _kind(EventKind::WaitBeats), _wait_beats(evt) {}
  MockEvent(WaitTicks evt) : _kind(EventKind::WaitTicks), _wait_ticks(evt) {}
  MockEvent(ChangeInstrument evt)
      : _kind(EventKind::ChangeInstrument), _change_instrument(evt) {}
  MockEvent(SetBpm evt) : _kind(EventKind::SetBpm), _set_bpm(evt) {}
  EventKind kind() { return _kind; }
  ChangeTrack as_change_track() {
    check_kind(EventKind::ChangeTrack);
    return _change_track;
  }
  PlayNote as_play_note() {
    check_kind(EventKind::PlayNote);
    return _play_note;
  }
  WaitBeats as_wait_beats() {
    check_kind(EventKind::WaitBeats);
    return _wait_beats;
  }
  WaitTicks as_wait_ticks() {
    check_kind(EventKind::WaitTicks);
    return _wait_ticks;
  }
  ChangeInstrument as_change_instrument() {
    check_kind(EventKind::ChangeInstrument);
    return _change_instrument;
  }
  SetBpm as_set_bpm() {
    check_kind(EventKind::SetBpm);
    return _set_bpm;
  }
  void check_kind(EventKind kind) {
    if (_kind != kind) {
      throw WrongEventKind(_kind, kind);
    }
  }

private:
  EventKind _kind;
  union {
    ChangeTrack _change_track;
    PlayNote _play_note;
    WaitBeats _wait_beats;
    WaitTicks _wait_ticks;
    ChangeInstrument _change_instrument;
    SetBpm _set_bpm;
  };
};
/// Mock event consumer for testing event producers
class MockEventConsumer : IEventConsumer {
public:
  void change_track(unsigned int track_num) override {
    ChangeTrack ct = {.track_num = track_num};
    events.push_back(MockEvent(ct));
  }
  void play_note(Channel channel, Note note, Octave octave,
                 Volume volume) override {
    PlayNote pn{
        .channel = channel, .note = note, .octave = octave, .volume = volume};
    events.push_back(MockEvent(pn));
  }

  void wait_beats(unsigned int n) override {
    WaitBeats wb = {.beats = n};
    events.push_back(MockEvent(wb));
  }
  void wait_ticks(unsigned int n) override {
    WaitTicks wt = {.ticks = n};
    events.push_back(MockEvent(wt));
  }
  void change_instrument(Channel channel, Instrument instr) override {
    ChangeInstrument ct{.channel = channel, .instr = instr};
    events.push_back(MockEvent(ct));
  }
  void set_bpm(Bpm bpm) override {
    SetBpm sb = {.bpm = bpm};
    events.push_back(MockEvent(sb));
  }

  MockEvent nth_event(unsigned int n) { return events.at(n); }

private:
  std::vector<MockEvent> events;
};
} // namespace synthlib::mock
