cmake_minimum_required(VERSION 3.15)

project(ReeseDestroyer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE setup - assuming JUCE is available via FetchContent or installed
# Option 1: FetchContent (downloads JUCE automatically)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

# Plugin configuration
juce_add_plugin(ReeseDestroyer
    COMPANY_NAME "AudioDev"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE ADev
    PLUGIN_CODE RsDe
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Reese Destroyer"
    VST3_CATEGORIES Fx Distortion
    AU_MAIN_TYPE kAudioUnitType_Effect
)

# Source files
target_sources(ReeseDestroyer
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/OctaveDivider.cpp
        Source/DSP/RingModulator.cpp
        Source/DSP/StateVariableFilter.cpp
        Source/DSP/WaveShapers.cpp
        Source/DSP/NoiseGenerator.cpp
        Source/Parameters/ParameterManager.cpp
)

# Compile definitions
target_compile_definitions(ReeseDestroyer
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

# JUCE modules
target_link_libraries(ReeseDestroyer
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
