/* SPDX-License-Identifier: AGPL-3.0-or-later
 BEGIN_JUCE_MODULE_DECLARATION
 ID: AudIO_constants
 vendor: KICK Windows port contributors
 version: 0.1.0
 name: KICK Windows compatibility constants
 description: Windows configuration and compatibility values used by original KICK
 license: AGPLv3
 dependencies:
 END_JUCE_MODULE_DECLARATION
*/
#pragma once
// The fixed block length is required by upstream DawEngine.cpp.
inline constexpr unsigned AUDIO_BLOCK_SIZE = 480;
// Windows device/bus profile. These are port configuration values, not a claim
// to have recovered the unpublished board configuration.
inline constexpr unsigned AUDIO_SAMPLE_RATE = 48000;
inline constexpr unsigned AUDIO_NUM_INPUTS = 2;
inline constexpr unsigned AUDIO_NUM_OUTPUTS = 2;
inline constexpr unsigned AUDIO_NUM_SEND_RETURNS = 4;
inline constexpr unsigned AUDIO_NUM_SUBMIX = 8;
inline constexpr unsigned AUDIO_NUM_AUX = 0; // DawEngine currently ignores this argument.
namespace MIDI {
inline constexpr int MIN=0, MAX=127, NUMERIC_MAX=128, STEP=1, DEFAULT=0;
inline constexpr int MIDDLE_C=60, NOTE_DEFAULT=60, LEVEL_DEFAULT=100;
inline constexpr float SCALER=1.f/127.f;
}
namespace MIDI_CH { inline constexpr int MIN=1, MAX=16, DEFAULT=1; }
namespace PAN { inline constexpr float MIN=-1.f, MAX=1.f, DEFAULT=0.f, STEP=0.01f; }
namespace GAIN {
inline constexpr float MIN=0.f, MAX=2.f, DEFAULT=1.f, STEP=0.01f;
inline constexpr float ROOT_2=1.4142135623730951f;
}
// Provisional compatibility limits/defaults: the published firmware does not
// contain their original definitions. See docs/WINDOWS-COMPATIBILITY.md.
namespace POLYPHONY { inline constexpr int MIN=1, MAX=64, DEFAULT=16, STEP=1; }
namespace DB { inline constexpr float MIN=-100.f; }
