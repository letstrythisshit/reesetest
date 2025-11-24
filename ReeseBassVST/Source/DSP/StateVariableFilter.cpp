#include "StateVariableFilter.h"

StateVariableFilter::StateVariableFilter()
{
}

void StateVariableFilter::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
    updateCoefficients();
}

void StateVariableFilter::reset()
{
    ic1eq = 0.0f;
    ic2eq = 0.0f;
}

void StateVariableFilter::setCutoff(float hz)
{
    hz = juce::jlimit(20.0f, 20000.0f, hz);

    if (std::abs(cutoffFreq - hz) > 0.01f)
    {
        cutoffFreq = hz;
        needsUpdate = true;
    }
}

void StateVariableFilter::setResonance(float res)
{
    res = juce::jlimit(0.0f, 0.95f, res);

    if (std::abs(resonance - res) > 0.001f)
    {
        resonance = res;
        needsUpdate = true;
    }
}

void StateVariableFilter::setDrive(float drv)
{
    drive = juce::jlimit(0.0f, 2.0f, drv);
}

void StateVariableFilter::updateCoefficients()
{
    if (!needsUpdate)
        return;

    // Frequency warping for better analog matching
    // Uses bilinear transform pre-warping
    float wd = 2.0f * juce::MathConstants<float>::pi * cutoffFreq;
    float T = 1.0f / static_cast<float>(sampleRate);
    float wa = (2.0f / T) * std::tan(wd * T / 2.0f);

    // Calculate g coefficient (cutoff)
    g = wa * T / 2.0f;

    // Calculate k coefficient (damping/resonance)
    // Convert resonance (0.0-0.95) to Q (0.5-20)
    // Higher resonance = higher Q = less damping = more peak
    float Q = 0.5f + (resonance * 19.5f);
    k = 1.0f / Q;

    needsUpdate = false;
}

float StateVariableFilter::saturateInput(float x)
{
    // Soft clip using tanh to mimic op-amp saturation
    // This adds analog character and prevents filter instability
    return std::tanh(x * 1.5f) / 1.5f;
}

StateVariableFilter::FilterOutput StateVariableFilter::processSample(float input)
{
    // Update coefficients if needed
    if (needsUpdate)
        updateCoefficients();

    // Apply input drive/saturation
    input *= drive;
    input = saturateInput(input);

    // State variable filter algorithm (trapezoidal integration)
    // Based on Vadim Zavalishin's "The Art of VA Filter Design"

    // Compute high-pass output
    float hp = (input - ic1eq * k - ic2eq) / (1.0f + g * k + g * g);

    // Compute band-pass output (first integrator)
    float bp = g * hp + ic1eq;
    ic1eq = g * hp + bp;  // Update integrator 1 state

    // Compute low-pass output (second integrator)
    float lp = g * bp + ic2eq;
    ic2eq = g * bp + lp;  // Update integrator 2 state

    // Prevent denormals (very small numbers that cause CPU issues)
    if (std::abs(ic1eq) < 1e-10f) ic1eq = 0.0f;
    if (std::abs(ic2eq) < 1e-10f) ic2eq = 0.0f;

    // Return all three filter outputs
    FilterOutput output;
    output.lowPass = lp;
    output.bandPass = bp;
    output.highPass = hp;

    return output;
}
