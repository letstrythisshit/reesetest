#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * Reese Destroyer Plugin Editor
 *
 * Future: Will implement skeuomorphic analog pedal GUI
 * Current: Using generic editor for development
 */
class ReeseDestroyerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ReeseDestroyerAudioProcessorEditor(ReeseDestroyerAudioProcessor&);
    ~ReeseDestroyerAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ReeseDestroyerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReeseDestroyerAudioProcessorEditor)
};
