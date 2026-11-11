// SPDX-License-Identifier: AGPL-3.0-or-later
// Compile the actual JUCE/AudIO bridge, without a replacement KICK interface.
#include <AudIO_juce/AudIO_juce.h>
#include <type_traits>
static_assert(std::is_base_of_v<AudIO::IAudioDevice, mt::AudioDevice>);
static_assert(!std::is_abstract_v<mt::AudioDevice>);
