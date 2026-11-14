// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <algorithm>
#include <array>

namespace lab {

constexpr int SurfacePads = 16;
constexpr int SurfaceSteps = 16;
constexpr int SurfaceDisplays = 2;
constexpr int SurfaceQLinks = 4;
constexpr int SurfaceMidiQLinks = 16;
constexpr int SurfaceGroups = 8;
constexpr int SurfaceMidiPads = SurfaceGroups * SurfacePads;

enum class MidiCcMode { Absolute, Relative1, Relative65 };

struct MidiCcBinding {
    int channel = -1;
    int cc = -1;
    MidiCcMode mode = MidiCcMode::Absolute;
};

struct MidiPadBinding {
    int channel = -1;
    int note = -1;

    bool matches(int incomingChannel, int incomingNote) const {
        return channel == incomingChannel && note == incomingNote;
    }
};

enum class MidiMessageKind { None, Note, ControlChange };

enum class MidiCommandTarget {
    PlayPause,
    Stop,
    Shift,
    Enter,
    Up,
    Down,
    Left,
    Right,
    ScreenA,
    ScreenB,
    Turn,
    Back,
    Soft1,Soft2,Soft3,Soft4,Soft5,Soft6,Soft7,Soft8,
    GroupA,GroupB,GroupC,GroupD,GroupE,GroupF,GroupG,GroupH,
    PadMode,Keyboard,Chords,Step,
    Restart,Erase,Tap,Follow,Record,
    FixedVelocity,Scene,Pattern,Events,Variation,Duplicate,Select,Solo,Mute,
    Volume,Swing,Tempo,NoteRepeat,Lock,Pitch,Mod,Perform,Notes,
    Channel,Plugin,Ideas,Mixer,Browser,Sampling,FileSave,Settings,Auto,Macro,
    // Maschine+ exposes a second control page with distinct MIDI messages.
    // Appending these targets preserves every KICKMIDI 4 command index.
    Soft9,Soft10,Soft11,Soft12,Soft13,Soft14,Soft15,Soft16,
    Count
};

constexpr int MidiCommandCount = static_cast<int>(MidiCommandTarget::Count);

struct MidiCommandBinding {
    MidiMessageKind kind = MidiMessageKind::None;
    int channel = -1;
    int number = -1;
    MidiCcMode mode = MidiCcMode::Absolute;
    int lastValue = -1;
};

inline int midiCcDelta(MidiCcMode mode, int value) {
    value = std::clamp(value, 0, 127);
    if (mode == MidiCcMode::Absolute) return 0;
    int delta = mode == MidiCcMode::Relative1
        ? (value == 0 || value == 64 ? 0 : (value < 64 ? value : value - 128))
        : value - 64;
    return std::clamp(delta, -16, 16);
}

inline int midiAbsoluteDelta(int previous, int value) {
    if (previous < 0) return 0;
    // Maschine's controller template exposes the 4D encoder as a bounded
    // absolute value. It keeps sending 0 or 127 for further physical ticks at
    // the end stops, so keep navigating instead of becoming unresponsive.
    if (value == previous && value == 0) return -1;
    if (value == previous && value == 127) return 1;
    return std::clamp(value - previous, -16, 16);
}

// Maschine templates commonly emit either 1/127 or 65/63 for one encoder
// tick. Decode both conventions before falling back to the selected mode.
inline int midiEncoderDelta(MidiCcMode mode, int previous, int value) {
    value = std::clamp(value, 0, 127);
    if (mode == MidiCcMode::Absolute) return midiAbsoluteDelta(previous, value);
    if (value == 1 || value == 65) return 1;
    if (value == 127 || value == 63) return -1;
    return midiCcDelta(mode, value);
}

// Logical KICK commands. Windows mouse, keyboard and MIDI adapters all emit
// these events instead of talking directly to the audio engine.
enum class SurfaceAction {
    PadPress,
    StepToggle,
    PlayPause,
    Stop,
    TempoDelta,
    CenterTurn,
    CenterPush,
    CenterUp,
    CenterDown,
    CenterLeft,
    CenterRight,
    QLinkTurn,
    QLinkSet,
    FocusNext,
    PagePrevious,
    PageNext,
    Shift,
    Back
};

struct SurfaceEvent {
    SurfaceAction action = SurfaceAction::PadPress;
    int index = 0;
    int value = 0;
    int display = 0;
    bool pressed = true;
};

enum class DisplayModule {
    Menu,
    Sample,
    Inserts,
    Browser,
    Mixer,
    TrackMute,
    Sequencer,
    PadPerform,
    NextSequence,
    Song,
    MidiControl
};

// A display is only a view onto the shared engine state. Two bindings can show
// different pages of one effect, or two entirely different modules, without
// creating duplicate effects or audio processors.
struct DisplayBinding {
    DisplayModule module = DisplayModule::Menu;
    int pad = 0;
    int slot = 0;
    int page = 0;
    bool followsSelectedPad = true;

    int parameterForKnob(int knob) const {
        return knob >= 0 && knob < SurfaceQLinks ? page * SurfaceQLinks + knob : -1;
    }
};

class SurfaceState {
public:
    SurfaceState() {
        displays[0] = {DisplayModule::Menu, 0, 0, 0, true};
        displays[1] = {DisplayModule::Menu, 0, 0, 0, true};
    }

    int selectedPad = 0;
    // Each Windows display keeps its own cursor. Changing the keyboard focus
    // must not move or reset the other display's selection.
    std::array<int, SurfaceDisplays> menuSelections{0, 0};
    int focusedDisplay = 0;
    bool shift = false;
    std::array<DisplayBinding, SurfaceDisplays> displays{};

    void selectPad(int pad) {
        selectedPad = std::clamp(pad, 0, SurfacePads - 1);
        for (auto& binding : displays)
            if (binding.followsSelectedPad) binding.pad = selectedPad;
    }

    void focusDisplay(int display) {
        focusedDisplay = std::clamp(display, 0, SurfaceDisplays - 1);
    }

    void setDisplayPage(int display, int page) {
        displays[std::clamp(display, 0, SurfaceDisplays - 1)].page = std::max(0, page);
    }
};

} // namespace lab
