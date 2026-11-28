cmake_minimum_required(VERSION 3.16)
project(KickLab VERSION 0.24.3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)
add_library(kicklab_engine STATIC src/engine.cpp)
target_include_directories(kicklab_engine PUBLIC src PRIVATE vendor/AudIO)
target_link_libraries(kicklab_engine PUBLIC Threads::Threads)
if(WIN32)
    add_executable(KickLab WIN32 src/windows.cpp)
    target_link_libraries(KickLab PRIVATE kicklab_engine winmm avrt comdlg32 shell32 gdi32 gdiplus)
    target_compile_definitions(KickLab PRIVATE UNICODE _UNICODE)
    if(MINGW)
        target_link_options(KickLab PRIVATE -static -municode)
    endif()
    add_custom_command(TARGET KickLab POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/demo-kit $<TARGET_FILE_DIR:KickLab>/demo-kit)
endif()
if(MSVC)
    target_compile_options(kicklab_engine PRIVATE /utf-8)
    target_compile_definitions(kicklab_engine PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(TARGET KickLab)
        target_compile_options(KickLab PRIVATE /utf-8)
        # Snapshot contains all 16 sequences and their per-step metadata. UI
        # command handlers keep some snapshots by value, so the 1 MiB default
        # Windows GUI stack can overflow as soon as a mouse click is routed.
        target_link_options(KickLab PRIVATE /STACK:8388608)
    endif()
endif()
add_executable(KickLab-Test tests/engine_test.cpp)
target_link_libraries(KickLab-Test PRIVATE kicklab_engine)
if(MSVC)
    # The exhaustive single-process test keeps several large Snapshot values in
    # one frame. KICKLAB 12 adds per-step event metadata, so reserve enough test
    # stack for MSVC's non-reused fixture locals.
    # Gain-stage tests keep additional complete Snapshot values alive in main.
    # MSVC reserves all of those large fixtures in one frame, so keep this
    # exhaustive test executable comfortably above that frame size.
    # FX Automation adds sparse per-pattern lanes to Snapshot. MSVC reserves
    # all exhaustive-test locals in main's frame instead of reusing their
    # scoped storage, so the test runner needs more stack than the GUI.
    target_link_options(KickLab-Test PRIVATE /STACK:134217728)
endif()
if(MINGW)
    target_link_options(KickLab-Test PRIVATE -static)
endif()
enable_testing()
add_test(NAME AudioEngine COMMAND KickLab-Test ${CMAKE_CURRENT_BINARY_DIR}/test-output)
