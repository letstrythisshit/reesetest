#pragma once

#include <JuceHeader.h>
#include <cmath>

/**
 * OctaveDivider - CD4013-style Digital Octave Divider
 *
 * Simulates a dual D-type flip-flop circuit that divides frequency by 2.
 * This creates sub-octaves (-1 octave, -2 octave) from the input bass signal.
 *
 * Algorithm:
 * 1. Convert input to square wave via zero-crossing detection with hysteresis
 * 2. Detect rising edges
 * 3. Toggle flip-flop state on each rising edge
 * 4. Convert boolean state to audio signal
 * 5. Smooth output with low-pass filter to add analog warmth
 */
class OctaveDivider
{
public:
    OctaveDivider();

    /**
     * Prepare the module for processing
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);

    /**
     * Process a single sample
     * @param input Input audio sample
     * @return Octave-divided output sample
     */
    float processSample(float input);

    /**
     * Reset internal state
     */
    void reset();

    /**
     * Set the tracking sensitivity (threshold adjustment)
     * @param sensitivity 0.0-1.0 (lower = tighter tracking, higher = looser)
     */
    void setSensitivity(float sensitivity);

private:
    // Sample rate
    double sampleRate = 44100.0;

    // Square wave converter state
    float threshold = 0.005f;           // Zero-crossing threshold
    float hysteresis = 0.002f;          // Hysteresis to prevent false triggers
    bool previousState = false;         // Previous sample state (HIGH/LOW)
    bool currentState = false;          // Current sample state

    // Flip-flop state (-1 octave)
    bool flipFlopState = false;

    // Second stage flip-flop state (-2 octave, optional)
    bool flipFlopState2 = false;

    // Output smoothing filter (1-pole low-pass at ~300Hz)
    // Mimics capacitor smoothing in analog circuit
    juce::dsp::IIR::Filter<float> smoothingFilter;

    // Previous input for edge detection
    float previousInput = 0.0f;

    // Update filter coefficients when sample rate changes
    void updateFilterCoefficients();
};
