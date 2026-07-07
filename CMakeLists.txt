cmake_minimum_required(VERSION 3.22)

project(BuffleAlign VERSION 0.3.0 LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment target")

set(BUFFLE_DEFAULT_JUCE_PATH "/Users/hostin/vibecoding/waveform-visualizer/JUCE")
set(JUCE_PATH "$ENV{JUCE_PATH}" CACHE PATH "Path to a local JUCE checkout")

if(NOT JUCE_PATH)
    if(EXISTS "${BUFFLE_DEFAULT_JUCE_PATH}/CMakeLists.txt")
        set(JUCE_PATH "${BUFFLE_DEFAULT_JUCE_PATH}" CACHE PATH "Path to a local JUCE checkout" FORCE)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/JUCE/CMakeLists.txt")
        set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/JUCE" CACHE PATH "Path to a local JUCE checkout" FORCE)
    endif()
endif()

if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE was not found. Set JUCE_PATH or place JUCE at third_party/JUCE.")
endif()

add_subdirectory("${JUCE_PATH}" "${CMAKE_BINARY_DIR}/JUCE")

juce_add_binary_data(BuffleAlignAssets
    SOURCES
        Resources/buffleaudio-logo.png)

add_library(BuffleAlignDSP STATIC
    BufflePlug-Analyzer/Source/DSP/AlignmentReport.cpp
    BufflePlug-Analyzer/Source/DSP/ConsonantTamer.cpp
    BufflePlug-Analyzer/Source/DSP/EnvelopeFeatureExtractor.cpp
    BufflePlug-Analyzer/Source/DSP/ManualNudgeDelay.cpp
    BufflePlug-Analyzer/Source/DSP/PreviewModeMixer.cpp
    BufflePlug-Analyzer/Source/DSP/RemovedMaterialMeter.cpp
    BufflePlug-Analyzer/Source/DSP/StackRolePreset.cpp
    BufflePlug-Analyzer/Source/DSP/TimingOffsetEstimator.cpp)

target_include_directories(BuffleAlignDSP
    PUBLIC
        BufflePlug-Analyzer/Source)

target_compile_features(BuffleAlignDSP PUBLIC cxx_std_17)

target_link_libraries(BuffleAlignDSP
    PUBLIC
        juce::juce_audio_basics
        juce::juce_core)

juce_add_plugin(BuffleAlign
    COMPANY_NAME "Buffle Audio"
    BUNDLE_ID "com.buffleaudio.align"
    PLUGIN_MANUFACTURER_CODE BfAu
    PLUGIN_CODE BfAl
    FORMATS Standalone VST3 AU
    PRODUCT_NAME "Buffle Audio Align"
    PLUGIN_NAME "Buffle Audio Align"
    DESCRIPTION "Vocal-stack alignment and articulation cleanup"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(BuffleAlign
    PRIVATE
        BufflePlug-Analyzer/Source/PluginProcessor.cpp
        BufflePlug-Analyzer/Source/PluginEditor.cpp)

target_include_directories(BuffleAlign
    PRIVATE
        BufflePlug-Analyzer/Source)

target_compile_definitions(BuffleAlign
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(BuffleAlign
    PRIVATE
        BuffleAlignDSP
        BuffleAlignAssets
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

option(BUFFLE_BUILD_TESTS "Build Buffle Align unit tests" ON)

if(BUFFLE_BUILD_TESTS)
enable_testing()

add_executable(BuffleAlignDSPTests
    tests/DSPCoreTests.cpp)

target_link_libraries(BuffleAlignDSPTests
    PRIVATE
        BuffleAlignDSP)

add_test(NAME BuffleAlignDSPTests COMMAND BuffleAlignDSPTests)
endif()
