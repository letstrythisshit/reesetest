#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReeseDestroyerAudioProcessorEditor::ReeseDestroyerAudioProcessorEditor(ReeseDestroyerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
}

ReeseDestroyerAudioProcessorEditor::~ReeseDestroyerAudioProcessorEditor()
{
}

//==============================================================================
void ReeseDestroyerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Reese Destroyer", getLocalBounds(), juce::Justification::centred, 1);
}

void ReeseDestroyerAudioProcessorEditor::resized()
{
}
