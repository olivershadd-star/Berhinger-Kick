/* SPDX-License-Identifier: AGPL-3.0-or-later
 BEGIN_JUCE_MODULE_DECLARATION
 ID: AudIO_juce
 vendor: KICK Windows port contributors
 version: 0.1.0
 name: KICK Windows platform adapter
 description: Platform plumbing for the published KICK sources
 license: AGPLv3
 dependencies: AudIO, AudIO_constants, juce_audio_utils
 END_JUCE_MODULE_DECLARATION
*/
#pragma once
#include <thread>
#include <AudIO/3rdparty/AudioFile.h>
#include <AudIO/AudIO.h>
#include <AudIO_constants/AudIO_constants.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioDevice.h"
#include "VoiceDsp.h"

#include "RecorderTypes.h"

namespace EMPTY {
inline const juce::AudioBuffer<float> SampleBuffer{};
inline const juce::String String{};
}
