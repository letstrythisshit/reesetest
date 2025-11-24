#include "OctaveDivider.h"

OctaveDivider::OctaveDivider()
{
}

void OctaveDivider::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateFilterCoefficients();
    reset();
}

void OctaveDivider::reset()
{
    previousState = false;
    currentState = false;
    flipFlopState = false;
    flipFlopState2 = false;
    previousInput = 0.0f;
    smoothingFilter.reset();
}

void OctaveDivider::updateFilterCoefficients()
{
    // Low-pass filter at 300Hz for output smoothing
    // This adds analog "warmth" and removes harsh digital artifacts
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(
        sampleRate,
        300.0  // Cutoff frequency
    );
    smoothingFilter.coefficients = coeffs;
}

void OctaveDivider::setSensitivity(float sensitivity)
{
    // sensitivity: 0.0 = tight (0.002), 1.0 = loose (0.01)
    threshold = 0.002f + (sensitivity * 0.008f);
    hysteresis = threshold * 0.4f;
}

float OctaveDivider::processSample(float input)
{
    // Step 1: Convert input to square wave with hysteresis
    // This prevents false triggering on noise

    if (input > threshold + hysteresis)
    {
        currentState = true;   // HIGH
    }
    else if (input < -(threshold + hysteresis))
    {
        currentState = false;  // LOW
    }
    // else: maintain previous state (hysteresis zone)
    else
    {
        currentState = previousState;
    }

    // Step 2: Detect rising edge
    // Rising edge occurs when: current is HIGH and previous was LOW
    bool risingEdge = currentState && !previousState;

    // Step 3: Toggle flip-flop on rising edge (-1 octave)
    if (risingEdge)
    {
        flipFlopState = !flipFlopState;

        // Optional: Second stage for -2 octave
        // Toggle second flip-flop when first goes from HIGH to LOW
        if (!flipFlopState)
        {
            flipFlopState2 = !flipFlopState2;
        }
    }

    // Update previous state for next sample
    previousState = currentState;
    previousInput = input;

    // Step 4: Convert flip-flop boolean state to audio signal
    // HIGH = +1.0, LOW = -1.0 (creates perfect square wave)
    float output = flipFlopState ? 1.0f : -1.0f;

    // Step 5: Apply smoothing filter to add analog character
    // This mimics the RC filter in analog circuits
    output = smoothingFilter.processSample(output);

    return output;
}
