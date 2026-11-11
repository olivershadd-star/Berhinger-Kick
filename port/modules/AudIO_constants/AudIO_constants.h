/* SPDX-License-Identifier: AGPL-3.0-or-later
 BEGIN_JUCE_MODULE_DECLARATION
 ID: AudIO_constants
 vendor: KICK Windows port contributors
 version: 0.1.0
 name: KICK Windows platform constants (partial)
 description: Verified graph block size; other firmware constants remain missing
 license: AGPLv3
 dependencies:
 END_JUCE_MODULE_DECLARATION
*/
#pragma once
// Verified against upstream AudIO/AudIO/Daw/DawEngine.cpp, BLOCK_SIZE = 480.
inline constexpr unsigned AUDIO_BLOCK_SIZE = 480;
// Deliberately no guessed device channel counts, routing counts, musical
// ranges or defaults. This is NOT a complete replacement of the missing module.
