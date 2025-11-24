#include "WaveShapers.h"

WaveShapers::WaveShapers()
{
}

void WaveShapers::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void WaveShapers::reset()
{
    // No state to reset for stateless saturation
}

void WaveShapers::setDrive(float drv)
{
    drive = juce::jlimit(0.0f, 2.0f, drv);
}

void WaveShapers::setSaturationType(SaturationType type)
{
    currentType = type;
}

void WaveShapers::setCharacter(float char_)
{
    character = juce::jlimit(0.0f, 1.0f, char_);
}

float WaveShapers::process(float input)
{
    // Apply drive (pre-gain)
    float x = input * drive;

    // Apply selected saturation algorithm
    float output;

    switch (currentType)
    {
        case SaturationType::SOFT_CLIP:
            output = softClip(x);
            break;

        case SaturationType::DIODE_CLIP:
            output = diodeClipper(x);
            break;

        case SaturationType::ASYMMETRIC:
            output = asymmetricSaturation(x);
            break;

        case SaturationType::MULTI_STAGE:
            output = multiStage(x);
            break;

        default:
            output = multiStage(x);
            break;
    }

    // Apply output compensation
    return output * outputGain;
}

float WaveShapers::softClip(float input)
{
    // Simple tanh soft clipping
    // Smooth, symmetric saturation
    return std::tanh(input * 1.5f);
}

float WaveShapers::diodeClipper(float input)
{
    // Simulates silicon diode forward voltage drop
    // Based on Shockley diode equation (simplified)

    const float Vf = 0.7f;   // Forward voltage (silicon diode)
    const float Eta = 1.5f;  // Ideality factor
    const float Vt = 0.026f; // Thermal voltage at room temp

    if (input > 0.0f)
    {
        return Vf * std::tanh(input / (Eta * Vt));
    }
    else
    {
        return -Vf * std::tanh(-input / (Eta * Vt));
    }
}

float WaveShapers::asymmetricSaturation(float input)
{
    // Asymmetric saturation (tube-style)
    // Positive and negative halves saturate differently
    // Creates even-order harmonics

    if (input > 0.0f)
    {
        // Positive half: harder saturation
        return std::tanh(input * 1.8f);
    }
    else
    {
        // Negative half: softer saturation
        return std::tanh(input * 1.2f) * 0.9f;
    }
}

float WaveShapers::multiStage(float input)
{
    // Multi-stage saturation with character control
    // Stage 1: Pre-saturation (light)
    float x = input;

    // Light soft clipping (adds warmth)
    x = std::tanh(x * 1.5f) / 1.5f;

    // Stage 2: Character-dependent saturation
    // character = 0.0: smooth (soft clip)
    // character = 1.0: aggressive (harder clip)
    float saturationAmount = 1.0f + (character * 2.0f); // 1.0 to 3.0
    x = std::tanh(x * saturationAmount) / saturationAmount;

    // Stage 3: Asymmetric clipping for harmonics
    if (character > 0.3f)
    {
        x = asymmetricSaturation(x * 1.2f) / 1.2f;
    }

    return x;
}

float WaveShapers::softClipWithThreshold(float input, float threshold)
{
    // Soft clipping that only engages above threshold
    // Below threshold: clean signal
    // Above threshold: soft saturation

    if (std::abs(input) < threshold)
    {
        return input;
    }
    else
    {
        float sign = (input > 0.0f) ? 1.0f : -1.0f;
        float excess = std::abs(input) - threshold;
        return sign * (threshold + std::tanh(excess * 2.0f) * 0.3f);
    }
}
