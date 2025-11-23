#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReeseDestroyerAudioProcessor::ReeseDestroyerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
                     #if !JucePlugin_IsMidiEffect
                      #if !JucePlugin_IsSynth
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      parameters(*this, nullptr, juce::Identifier("ReeseDestroyer"), createParameterLayout())
{
    // Get parameter pointers for real-time access
    cutoffParam = parameters.getRawParameterValue("cutoff");
    resonanceParam = parameters.getRawParameterValue("resonance");
    ringRateParam = parameters.getRawParameterValue("ringRate");
    ringDepthParam = parameters.getRawParameterValue("ringDepth");
    octaveBlendParam = parameters.getRawParameterValue("octaveBlend");
    characterParam = parameters.getRawParameterValue("character");
    driveParam = parameters.getRawParameterValue("drive");
    volumeParam = parameters.getRawParameterValue("volume");
    stereoWidthParam = parameters.getRawParameterValue("stereoWidth");
    noiseAmountParam = parameters.getRawParameterValue("noiseAmount");
}

ReeseDestroyerAudioProcessor::~ReeseDestroyerAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ReeseDestroyerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // CUTOFF: 200-2000Hz (logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "cutoff",
        "Cutoff",
        juce::NormalisableRange<float>(200.0f, 2000.0f,
            [](float start, float end, float normalized) {
                return start * std::pow(end / start, normalized);
            },
            [](float start, float end, float value) {
                return std::log(value / start) / std::log(end / start);
            }),
        500.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " Hz"; }
    ));

    // RESONANCE: 0-95%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "resonance",
        "Resonance",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
        0.60f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // RING MOD RATE: 0.5-10Hz (logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ringRate",
        "Ring Mod Rate",
        juce::NormalisableRange<float>(0.5f, 10.0f,
            [](float start, float end, float normalized) {
                return start * std::pow(end / start, normalized);
            },
            [](float start, float end, float value) {
                return std::log(value / start) / std::log(end / start);
            }),
        3.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 2) + " Hz"; }
    ));

    // RING MOD DEPTH: 0-100%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ringDepth",
        "Ring Mod Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.50f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // OCTAVE BLEND: 0-100%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "octaveBlend",
        "Octave Blend",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.40f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // CHARACTER: 0-100%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "character",
        "Character",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.50f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // DRIVE: 0-200%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "drive",
        "Drive",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f),
        1.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // VOLUME: -∞ to +6dB
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "volume",
        "Volume",
        juce::NormalisableRange<float>(-60.0f, 6.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {
            if (value <= -60.0f) return juce::String("-∞ dB");
            return juce::String(value, 1) + " dB";
        }
    ));

    // STEREO WIDTH: 0-100% (internal parameter)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "stereoWidth",
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.50f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    // NOISE AMOUNT: 0-100% (internal parameter)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "noiseAmount",
        "Noise Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.20f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(int(value * 100.0f)) + " %"; }
    ));

    return layout;
}

//==============================================================================
const juce::String ReeseDestroyerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReeseDestroyerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReeseDestroyerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReeseDestroyerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReeseDestroyerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0; // No tail (not a reverb/delay)
}

int ReeseDestroyerAudioProcessor::getNumPrograms()
{
    return 8; // 8 factory presets
}

int ReeseDestroyerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReeseDestroyerAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ReeseDestroyerAudioProcessor::getProgramName(int index)
{
    const char* presetNames[] = {
        "Classic DnB Reese",
        "Sub Terror",
        "Industrial Grind",
        "Dubstep Wobble",
        "Smooth Operator",
        "Neurofunk Madness",
        "Minimal Deep",
        "Distorted Beast"
    };

    if (index >= 0 && index < 8)
        return presetNames[index];

    return {};
}

void ReeseDestroyerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void ReeseDestroyerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize all DSP modules with sample rate
    leftChannel.octaveDivider.prepare(sampleRate);
    rightChannel.octaveDivider.prepare(sampleRate);

    leftChannel.ringModulator.prepare(sampleRate);
    rightChannel.ringModulator.prepare(sampleRate);

    leftChannel.filter1.prepare(sampleRate);
    rightChannel.filter1.prepare(sampleRate);
    leftChannel.filter2.prepare(sampleRate);
    rightChannel.filter2.prepare(sampleRate);
    leftChannel.filter3.prepare(sampleRate);
    rightChannel.filter3.prepare(sampleRate);

    leftChannel.waveShaper.prepare(sampleRate);
    rightChannel.waveShaper.prepare(sampleRate);

    leftChannel.noiseGen.prepare(sampleRate);
    rightChannel.noiseGen.prepare(sampleRate);

    // Initialize parameter smoothing (50ms ramp time)
    const double rampTime = 0.05;
    smoothedCutoff.reset(sampleRate, rampTime);
    smoothedResonance.reset(sampleRate, rampTime);
    smoothedRingRate.reset(sampleRate, rampTime);
    smoothedRingDepth.reset(sampleRate, rampTime);
    smoothedOctaveBlend.reset(sampleRate, rampTime);
    smoothedCharacter.reset(sampleRate, rampTime);
    smoothedDrive.reset(sampleRate, rampTime);
    smoothedVolume.reset(sampleRate, rampTime);
    smoothedStereoWidth.reset(sampleRate, rampTime);
    smoothedNoiseAmount.reset(sampleRate, rampTime);

    // Set initial values
    smoothedCutoff.setCurrentAndTargetValue(cutoffParam->load());
    smoothedResonance.setCurrentAndTargetValue(resonanceParam->load());
    smoothedRingRate.setCurrentAndTargetValue(ringRateParam->load());
    smoothedRingDepth.setCurrentAndTargetValue(ringDepthParam->load());
    smoothedOctaveBlend.setCurrentAndTargetValue(octaveBlendParam->load());
    smoothedCharacter.setCurrentAndTargetValue(characterParam->load());
    smoothedDrive.setCurrentAndTargetValue(driveParam->load());
    smoothedVolume.setCurrentAndTargetValue(volumeParam->load());
    smoothedStereoWidth.setCurrentAndTargetValue(stereoWidthParam->load());
    smoothedNoiseAmount.setCurrentAndTargetValue(noiseAmountParam->load());

    // Report zero latency
    setLatencySamples(0);
}

void ReeseDestroyerAudioProcessor::releaseResources()
{
    // Release any resources when playback stops
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReeseDestroyerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output layout must match
   #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReeseDestroyerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update target values for parameter smoothing
    smoothedCutoff.setTargetValue(cutoffParam->load());
    smoothedResonance.setTargetValue(resonanceParam->load());
    smoothedRingRate.setTargetValue(ringRateParam->load());
    smoothedRingDepth.setTargetValue(ringDepthParam->load());
    smoothedOctaveBlend.setTargetValue(octaveBlendParam->load());
    smoothedCharacter.setTargetValue(characterParam->load());
    smoothedDrive.setTargetValue(driveParam->load());
    smoothedVolume.setTargetValue(volumeParam->load());
    smoothedStereoWidth.setTargetValue(stereoWidthParam->load());
    smoothedNoiseAmount.setTargetValue(noiseAmountParam->load());

    // Process each sample
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // Get smoothed parameter values for this sample
        float cutoff = smoothedCutoff.getNextValue();
        float resonance = smoothedResonance.getNextValue();
        float ringRate = smoothedRingRate.getNextValue();
        float ringDepth = smoothedRingDepth.getNextValue();
        float octaveBlend = smoothedOctaveBlend.getNextValue();
        float character = smoothedCharacter.getNextValue();
        float drive = smoothedDrive.getNextValue();
        float volumeDb = smoothedVolume.getNextValue();
        float stereoWidth = smoothedStereoWidth.getNextValue();
        float noiseAmount = smoothedNoiseAmount.getNextValue();

        // Convert volume from dB to linear
        float volumeLinear = juce::Decibels::decibelsToGain(volumeDb);

        // Calculate mix levels from character parameter
        MixLevels mixLevels = calculateMixLevels(character);

        // Update DSP module parameters
        leftChannel.filter1.setCutoff(cutoff);
        leftChannel.filter1.setResonance(resonance);
        rightChannel.filter1.setCutoff(cutoff);
        rightChannel.filter1.setResonance(resonance);

        leftChannel.filter2.setCutoff(cutoff);
        leftChannel.filter2.setResonance(resonance);
        rightChannel.filter2.setCutoff(cutoff);
        rightChannel.filter2.setResonance(resonance);

        leftChannel.filter3.setCutoff(cutoff);
        leftChannel.filter3.setResonance(resonance);
        rightChannel.filter3.setCutoff(cutoff);
        rightChannel.filter3.setResonance(resonance);

        leftChannel.ringModulator.setFrequency(ringRate);
        leftChannel.ringModulator.setDepth(ringDepth);
        // Slight detune for stereo width
        rightChannel.ringModulator.setFrequency(ringRate * (1.0f + stereoWidth * 0.05f));
        rightChannel.ringModulator.setDepth(ringDepth);

        leftChannel.waveShaper.setDrive(drive);
        rightChannel.waveShaper.setDrive(drive);

        leftChannel.noiseGen.setAmount(noiseAmount);
        rightChannel.noiseGen.setAmount(noiseAmount);

        // Process left channel
        float leftInput = totalNumInputChannels > 0 ? buffer.getSample(0, sample) : 0.0f;
        float leftClean = leftInput;

        // Octave divider path
        float leftOctaved = leftChannel.octaveDivider.processSample(leftInput);
        leftOctaved = leftChannel.filter1.processSample(leftOctaved).lowPass;

        // Ring mod path
        float leftRinged = leftChannel.ringModulator.processSample(leftInput);
        leftRinged = leftChannel.filter2.processSample(leftRinged).lowPass;

        // Combined path (ring mod + octave)
        float leftCombined = leftChannel.ringModulator.processSample(leftOctaved);
        leftCombined = leftChannel.filter3.processSample(leftCombined).lowPass;

        // Mix all paths
        float leftMixed = (leftClean * mixLevels.clean) +
                         (leftOctaved * mixLevels.subOctave * octaveBlend) +
                         (leftRinged * mixLevels.ringMod) +
                         (leftCombined * mixLevels.ringModOctave);

        // Apply saturation
        leftMixed = leftChannel.waveShaper.process(leftMixed);

        // Add noise
        leftMixed = leftChannel.noiseGen.apply(leftMixed);

        // Apply volume and soft limiting
        leftMixed *= volumeLinear;
        leftMixed = softLimiter(leftMixed);

        buffer.setSample(0, sample, leftMixed);

        // Process right channel (if stereo)
        if (totalNumInputChannels > 1 && totalNumOutputChannels > 1)
        {
            float rightInput = buffer.getSample(1, sample);
            float rightClean = rightInput;

            float rightOctaved = rightChannel.octaveDivider.processSample(rightInput);
            rightOctaved = rightChannel.filter1.processSample(rightOctaved).lowPass;

            float rightRinged = rightChannel.ringModulator.processSample(rightInput);
            rightRinged = rightChannel.filter2.processSample(rightRinged).lowPass;

            float rightCombined = rightChannel.ringModulator.processSample(rightOctaved);
            rightCombined = rightChannel.filter3.processSample(rightCombined).lowPass;

            float rightMixed = (rightClean * mixLevels.clean) +
                              (rightOctaved * mixLevels.subOctave * octaveBlend) +
                              (rightRinged * mixLevels.ringMod) +
                              (rightCombined * mixLevels.ringModOctave);

            rightMixed = rightChannel.waveShaper.process(rightMixed);
            rightMixed = rightChannel.noiseGen.apply(rightMixed);
            rightMixed *= volumeLinear;
            rightMixed = softLimiter(rightMixed);

            buffer.setSample(1, sample, rightMixed);
        }
        else if (totalNumOutputChannels > 1)
        {
            // Duplicate mono to stereo
            buffer.setSample(1, sample, leftMixed);
        }
    }
}

//==============================================================================
ReeseDestroyerAudioProcessor::MixLevels ReeseDestroyerAudioProcessor::calculateMixLevels(float characterPercent)
{
    // Character: 0% = clean bass, 100% = maximum chaos
    MixLevels levels;

    levels.clean = 0.5f - (characterPercent * 0.3f);           // Decreases slightly
    levels.subOctave = 0.2f + (characterPercent * 0.4f);       // Increases
    levels.ringMod = characterPercent * 0.6f;                  // Increases strongly
    levels.ringModOctave = characterPercent * characterPercent * 0.4f; // Quadratic increase

    return levels;
}

float ReeseDestroyerAudioProcessor::softLimiter(float input)
{
    // Prevent harsh clipping at output
    const float threshold = 0.9f;

    if (std::abs(input) < threshold)
        return input;

    float sign = (input > 0.0f) ? 1.0f : -1.0f;
    float excess = std::abs(input) - threshold;
    return sign * (threshold + std::tanh(excess * 5.0f) * 0.1f);
}

//==============================================================================
bool ReeseDestroyerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ReeseDestroyerAudioProcessor::createEditor()
{
    // For now, use generic editor - custom GUI will be implemented later
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ReeseDestroyerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameter state
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReeseDestroyerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameter state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReeseDestroyerAudioProcessor();
}
