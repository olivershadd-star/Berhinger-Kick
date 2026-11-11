cmake_minimum_required(VERSION 3.22)
project(KickOriginalWindowsPort LANGUAGES NONE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(KICK_UPSTREAM "${CMAKE_CURRENT_SOURCE_DIR}/upstream/0700-AAM")
set(KICK_JUCE_SOURCE "" CACHE PATH "Optional local JUCE 7.0.12 source")
option(KICK_AUDIT_ONLY "Inventory immutable sources without building KICK" OFF)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_original.py"
    --source "${KICK_UPSTREAM}" --verify-original
    --json "${CMAKE_CURRENT_BINARY_DIR}/source-audit.json"
    RESULT_VARIABLE audit_result OUTPUT_VARIABLE audit_output ERROR_VARIABLE audit_error)
message(STATUS "${audit_output}")
if(NOT audit_result EQUAL 0)
    message(FATAL_ERROR "Source integrity verification failed. No application was built.\n${audit_error}")
endif()
if(KICK_AUDIT_ONLY)
    message(STATUS "AUDIT ONLY: no executable is produced.")
    return()
endif()
if(NOT WIN32)
    message(FATAL_ERROR "The application port requires Windows. No application was built. Use KICK_AUDIT_ONLY on other hosts.")
endif()
set(staging "${CMAKE_CURRENT_BINARY_DIR}/staging")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/stage_windows_port.py"
    --destination "${staging}" RESULT_VARIABLE prepare_result)
if(NOT prepare_result EQUAL 0)
    message(FATAL_ERROR "Unable to stage Windows adaptations; use a fresh build directory if sources changed.")
endif()
include(ExternalProject)
set(original_args
    "-DCPM_AudIO_SOURCE:PATH=${staging}/AudIO"
    "-DCPM_COM_PROTOCOL_SOURCE:PATH=${staging}/ComProtocol"
    "-DCPM_SOURCE_CACHE:PATH=${CMAKE_CURRENT_BINARY_DIR}/dependency-cache"
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    -DTIMESTAMP=OFF -DKICK_SIMULATOR=OFF -DKICK_VST_PLUGIN=OFF)
if(KICK_JUCE_SOURCE)
    list(APPEND original_args "-DCPM_JUCE_SOURCE:PATH=${KICK_JUCE_SOURCE}")
endif()
ExternalProject_Add(KickOriginal
    SOURCE_DIR "${staging}/Kick"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/original-build"
    DOWNLOAD_COMMAND "" UPDATE_COMMAND ""
    CMAKE_ARGS ${original_args}
    BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config $<CONFIG> --target Kick --parallel 2
    INSTALL_COMMAND "")
