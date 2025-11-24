#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator()
    : rng(std::random_device{}()),
      distribution(-1.0f, 1.0f)
{
}

void NoiseGenerator::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void NoiseGenerator::reset()
{
    // Reset pink noise filter state
    b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.0f;
}

void NoiseGenerator::setAmount(float amount)
{
    noiseAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void NoiseGenerator::setColor(NoiseColor color)
{
    currentColor = color;
}

float NoiseGenerator::generateSample()
{
    switch (currentColor)
    {
        case NoiseColor::WHITE:
            return generateWhiteNoise();

        case NoiseColor::PINK:
            return generatePinkNoise();

        default:
            return generatePinkNoise();
    }
}

float NoiseGenerator::apply(float cleanSignal)
{
    // Generate noise
    float noise = generateSample();

    // Scale to appropriate level (-60dB to -40dB)
    float noiseGain = amountToGain(noiseAmount);
    noise *= noiseGain;

    // Mix with clean signal
    return cleanSignal + noise;
}

float NoiseGenerator::generateWhiteNoise()
{
    // Pure white noise: uniform random distribution
    return distribution(rng);
}

float NoiseGenerator::generatePinkNoise()
{
    // Paul Kellett's pink noise algorithm
    // This creates 1/f spectrum (3dB/octave rolloff)
    // More natural sounding than white noise

    float white = generateWhiteNoise();

    // Apply cascaded filters to shape white noise into pink noise
    b0 = 0.99886f * b0 + white * 0.0555179f;
    b1 = 0.99332f * b1 + white * 0.0750759f;
    b2 = 0.96900f * b2 + white * 0.1538520f;
    b3 = 0.86650f * b3 + white * 0.3104856f;
    b4 = 0.55000f * b4 + white * 0.5329522f;
    b5 = -0.7616f * b5 - white * 0.0168980f;

    float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
    b6 = white * 0.115926f;

    // Normalize output (pink noise has different RMS than white)
    pink *= 0.11f;

    return pink;
}

float NoiseGenerator::amountToGain(float amount)
{
    // Map amount (0.0-1.0) to dB range (-60dB to -40dB)
    // amount=0.0 -> -60dB (barely audible)
    // amount=1.0 -> -40dB (subtle but present)

    float dB = -60.0f + (amount * 20.0f);

    // Convert dB to linear gain
    return std::pow(10.0f, dB / 20.0f);
}
