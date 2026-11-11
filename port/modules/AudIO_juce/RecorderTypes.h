// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
namespace mt {
// Interface enum values from the published AudIO/Objects/Recorder.h.
// This restores only the type contract. It does not supply the missing
// SampleRecorder or Looper implementations or pretend they are functional.
struct Recorder {
    enum Direction { FORWARD, REVERSE };
    enum Mode { L_MONO, R_MONO, LR_MONO, L_STEREO, R_STEREO, LR_STEREO };
};
}
