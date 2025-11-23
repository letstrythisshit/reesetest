#include "RingModulator.h"

RingModulator::RingModulator()
    : rng(std::random_device{}()),
      distribution(-0.05f, 0.05f)
{
}

void RingModulator::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void RingModulator::reset()
{
    lfoPhase = 0.0f;
    phaseOffset = 0.0f;
}

void RingModulator::setFrequency(float hz)
{
    lfoFrequency = juce::jlimit(0.5f, 10.0f, hz);
}

void RingModulator::setDepth(float depth)
{
    modulationDepth = juce::jlimit(0.0f, 1.0f, depth);
}

void RingModulator::setWaveShape(WaveShape shape)
{
    currentShape = shape;
}

float RingModulator::processSample(float input)
{
    // Generate LFO output for current phase
    float lfoOutput = generateLFO();

    // Scale LFO from bipolar (-1 to +1) to unipolar (0 to 1)
    // This creates amplitude modulation rather than pure ring modulation
    float modulator = (lfoOutput + 1.0f) * 0.5f;

    // Apply depth control
    // depth=0: modulator stays at 1.0 (no effect)
    // depth=1: modulator varies 0.0-1.0 (full effect)
    modulator = 1.0f - (modulationDepth * (1.0f - modulator));

    // Ring modulation: multiply input by modulator
    float output = input * modulator;

    // Advance LFO phase
    lfoPhase += (lfoFrequency / static_cast<float>(sampleRate));

    // Wrap phase to 0.0-1.0 range
    if (lfoPhase >= 1.0f)
    {
        lfoPhase -= 1.0f;

        // Add random phase offset each cycle for organic movement
        // This prevents static, repetitive modulation
        phaseOffset = distribution(rng);
    }

    return output;
}

float RingModulator::generateLFO()
{
    // Apply phase offset for chaos
    float phase = lfoPhase + phaseOffset;
    if (phase > 1.0f)
        phase -= 1.0f;
    if (phase < 0.0f)
        phase += 1.0f;

    // Generate waveform based on current shape
    switch (currentShape)
    {
        case WaveShape::SINE:
            return generateSine();

        case WaveShape::MODIFIED_SINE:
            return generateModifiedSine();

        case WaveShape::TRIANGLE:
            return generateTriangle();

        default:
            return generateModifiedSine();
    }
}

float RingModulator::generateSine()
{
    // Pure sine wave
    return std::sin(2.0f * juce::MathConstants<float>::pi * lfoPhase);
}

float RingModulator::generateModifiedSine()
{
    // Modified sine with added harmonics (mimics analog VCO imperfections)
    // Adds 3rd harmonic at 15% amplitude for richer character
    float fundamental = std::sin(2.0f * juce::MathConstants<float>::pi * lfoPhase);
    float harmonic = std::sin(6.0f * juce::MathConstants<float>::pi * lfoPhase);

    float output = fundamental + (0.15f * harmonic);

    // Soft clipping to prevent exceeding ±1.0 range
    output = std::tanh(output * 1.3f);

    return output;
}

float RingModulator::generateTriangle()
{
    // Triangle wave (linear ramps)
    float output;

    if (lfoPhase < 0.5f)
    {
        // Rising edge: -1 to +1
        output = (lfoPhase * 4.0f) - 1.0f;
    }
    else
    {
        // Falling edge: +1 to -1
        output = 3.0f - (lfoPhase * 4.0f);
    }

    return output;
}
