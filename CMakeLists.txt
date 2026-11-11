cmake_minimum_required(VERSION 3.16)
project(KickOriginalWindowsPort LANGUAGES NONE)

# This project deliberately does not link experiments/kicklab or synthesize a
# replacement for any missing KICK module.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(KICK_UPSTREAM "${CMAKE_CURRENT_SOURCE_DIR}/upstream/0700-AAM")
set(KICK_AUDIO_SOURCE "${KICK_UPSTREAM}/AudIO" CACHE PATH "Complete original AudIO source, including its JUCE modules")
set(KICK_SERIALPORT_SOURCE "${KICK_UPSTREAM}/Kick/modules/3rdParty/juce_serialport" CACHE PATH "Original serialport module with its redistribution rights")
set(KICK_JUCE_SOURCE "" CACHE PATH "Optional local JUCE 7.0.12 source")
option(KICK_AUDIT_ONLY "Inventory sources only; does not build KICK or a Windows app" OFF)

set(audit_args --source "${KICK_UPSTREAM}" --audio "${KICK_AUDIO_SOURCE}"
    --serialport "${KICK_SERIALPORT_SOURCE}" --verify-original
    --json "${CMAKE_CURRENT_BINARY_DIR}/source-audit.json")
if(NOT KICK_AUDIT_ONLY)
    list(APPEND audit_args --strict)
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_original.py" ${audit_args}
    RESULT_VARIABLE audit_result OUTPUT_VARIABLE audit_output ERROR_VARIABLE audit_error)
message(STATUS "${audit_output}")
if(NOT audit_result EQUAL 0)
    message(FATAL_ERROR "Original KICK build blocked. No application was built.\n${audit_error}\nSee source-audit.json and docs/PORTAGE.md.")
endif()
if(KICK_AUDIT_ONLY)
    message(STATUS "AUDIT ONLY: this configuration does not produce a KICK executable.")
    return()
endif()
if(NOT WIN32)
    message(FATAL_ERROR "The original application target is currently configured for native Windows only. Use KICK_AUDIT_ONLY on other hosts.")
endif()

# Run the author's CMake project as a top-level project in a disposable copy:
# its scripts use CMAKE_SOURCE_DIR and generate files in their source tree.
# Upstream remains byte-for-byte identical to the published source.
set(staging "${CMAKE_CURRENT_BINARY_DIR}/staging")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/prepare_original_build.py"
    --source "${KICK_UPSTREAM}" --destination "${staging}" --serialport "${KICK_SERIALPORT_SOURCE}"
    RESULT_VARIABLE prepare_result)
if(NOT prepare_result EQUAL 0)
    message(FATAL_ERROR "Unable to prepare original sources; no fallback implementation is allowed.")
endif()
include(ExternalProject)
set(original_args
    "-DCPM_AudIO_SOURCE:PATH=${KICK_AUDIO_SOURCE}"
    "-DCPM_COM_PROTOCOL_SOURCE:PATH=${KICK_UPSTREAM}/ComProtocol"
    "-DCPM_SOURCE_CACHE:PATH=${CMAKE_CURRENT_BINARY_DIR}/dependency-cache"
    -DTIMESTAMP=OFF -DKICK_SIMULATOR=OFF -DKICK_VST_PLUGIN=OFF)
if(KICK_JUCE_SOURCE)
    list(APPEND original_args "-DCPM_JUCE_SOURCE:PATH=${KICK_JUCE_SOURCE}")
endif()
ExternalProject_Add(KickOriginal
    SOURCE_DIR "${staging}/Kick"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/original-build"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND ""
    CMAKE_ARGS ${original_args}
    BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config $<CONFIG> --target Kick
    INSTALL_COMMAND "")
