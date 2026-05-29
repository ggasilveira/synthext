#pragma once
#include "synthlib/instrument.hpp"
#include "synthlib/primitives.hpp"
namespace synthlib {

/// Interface for consuming MIDI events.
class IEventConsumer {
public:
  virtual ~IEventConsumer() = default;

  /// Set the consumer to the specified track.
  /// @param track_num track to enter
  virtual void change_track(unsigned int track_num);

  /// Plays the note for one beat in the current track.
  /// @param channel which channel to play the note
  /// @param note the note to play
  /// @param octave the note's octave
  /// @param the note's volume
  virtual void play_note(Channel channel, Note note, Octave octave,
                         Volume volume);

  /// Pauses/waits for n beats
  /// @param n number of beats
  virtual void wait_beats(unsigned int n);
  /// Pauses/waits for n ticks.
  /// @param n number of ticks
  virtual void wait_ticks(unsigned int n);

  /// Changes the channel's current instrument.
  /// @param channel the channel to change the instrument
  /// @param instr the target instrument
  virtual void change_instrument(Channel channel, Instrument instr);
  /// Changes the global bpm.
  /// @param bpm the target bpm
  virtual void set_bpm(Bpm bpm);

  /// Return the number of ticks contained in a beat.
  /// @return the number of ticks per beat
  // virtual int ticks_per_beat();
};

} // namespace synthlib
