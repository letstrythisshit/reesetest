#pragma once

#include <JuceHeader.h>
#include "DSP/OctaveDivider.h"
#include "DSP/RingModulator.h"
#include "DSP/StateVariableFilter.h"
#include "DSP/WaveShapers.h"
#include "DSP/NoiseGenerator.h"

//==============================================================================
/**
 * Reese Destroyer Plugin Processor
 *
 * Authentically simulates an analog Reese bass pedal effect using:
 * - CD4013-style octave division
 * - Sub-audio ring modulation (0.5-10Hz)
 * - Resonant state-variable filtering
 * - Analog-style saturation and wave shaping
 * - Subtle noise for analog character
 */
class ReeseDestroyerAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReeseDestroyerAudioProcessor();
    ~ReeseDestroyerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

private:
    //==============================================================================
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    // Parameters (8 main controls)
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* cutoffParam = nullptr;
    std::atomic<float>* resonanceParam = nullptr;
    std::atomic<float>* ringRateParam = nullptr;
    std::atomic<float>* ringDepthParam = nullptr;
    std::atomic<float>* octaveBlendParam = nullptr;
    std::atomic<float>* characterParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* volumeParam = nullptr;

    // Additional internal parameters
    std::atomic<float>* stereoWidthParam = nullptr;
    std::atomic<float>* noiseAmountParam = nullptr;

    //==============================================================================
    // DSP Modules (per-channel for stereo processing)
    struct ChannelDSP
    {
        OctaveDivider octaveDivider;
        RingModulator ringModulator;
        StateVariableFilter filter1;  // For octave path
        StateVariableFilter filter2;  // For ring mod path
        StateVariableFilter filter3;  // For combined path
        WaveShapers waveShaper;
        NoiseGenerator noiseGen;
    };

    ChannelDSP leftChannel;
    ChannelDSP rightChannel;

    //==============================================================================
    // Parameter smoothing to prevent zipper noise
    juce::SmoothedValue<float> smoothedCutoff;
    juce::SmoothedValue<float> smoothedResonance;
    juce::SmoothedValue<float> smoothedRingRate;
    juce::SmoothedValue<float> smoothedRingDepth;
    juce::SmoothedValue<float> smoothedOctaveBlend;
    juce::SmoothedValue<float> smoothedCharacter;
    juce::SmoothedValue<float> smoothedDrive;
    juce::SmoothedValue<float> smoothedVolume;
    juce::SmoothedValue<float> smoothedStereoWidth;
    juce::SmoothedValue<float> smoothedNoiseAmount;

    //==============================================================================
    // Mixing levels calculated from Character parameter
    struct MixLevels
    {
        float clean;
        float subOctave;
        float ringMod;
        float ringModOctave;
    };

    MixLevels calculateMixLevels(float characterPercent);

    //==============================================================================
    // Utility functions
    float softLimiter(float input);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReeseDestroyerAudioProcessor)
};
