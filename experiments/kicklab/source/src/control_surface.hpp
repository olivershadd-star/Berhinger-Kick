// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <algorithm>
#include <array>

namespace lab {

constexpr int SurfacePads = 16;
constexpr int SurfaceSteps = 16;
constexpr int SurfaceDisplays = 2;
constexpr int SurfaceQLinks = 4;

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
    Sample,
    Inserts,
    Browser,
    Mixer,
    Sequencer
};

// A display is only a view onto the shared engine state. Two bindings can show
// different pages of one effect, or two entirely different modules, without
// creating duplicate effects or audio processors.
struct DisplayBinding {
    DisplayModule module = DisplayModule::Inserts;
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
        displays[0] = {DisplayModule::Inserts, 0, 0, 0, true};
        displays[1] = {DisplayModule::Inserts, 0, 0, 1, true};
    }

    int selectedPad = 0;
    int menuSelection = 0;
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
