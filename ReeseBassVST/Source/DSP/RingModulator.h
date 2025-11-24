#pragma once

#include <juce_core/juce_core.h>
#include <cmath>
#include <random>

/**
 * RingModulator - Sub-Audio Ring Modulator
 *
 * Implements ring modulation with sub-audio frequencies (0.5-10Hz) to create
 * the characteristic "detuned oscillator" effect of Reese bass.
 *
 * Ring modulation: output = carrier × modulator
 * - Carrier: Input bass signal
 * - Modulator: Sub-audio LFO (0.5-10Hz)
 *
 * This creates:
 * - Slow amplitude modulation (tremolo-like)
 * - Phase cancellation artifacts (hollow quality)
 * - Harmonic instability (sounds like multiple detuned oscillators)
 * - Beating frequencies that create tension and movement
 *
 * Frequency ranges:
 * - 0.5-2Hz: Slow, heavy breathing (industrial)
 * - 2-5Hz: Classic Reese movement (DnB standard)
 * - 5-10Hz: Fast warbling (aggressive dubstep)
 */
class RingModulator
{
public:
    enum class WaveShape
    {
        SINE,           // Pure sine wave
        MODIFIED_SINE,  // Sine with added harmonics (analog character)
        TRIANGLE        // Triangle wave
    };

    RingModulator();

    /**
     * Prepare the module for processing
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);

    /**
     * Process a single sample
     * @param input Input audio sample
     * @return Ring-modulated output sample
     */
    float processSample(float input);

    /**
     * Set the LFO frequency
     * @param hz Frequency in Hz (0.5-10Hz)
     */
    void setFrequency(float hz);

    /**
     * Set the modulation depth
     * @param depth 0.0-1.0 (0% to 100%)
     */
    void setDepth(float depth);

    /**
     * Set the waveform shape
     * @param shape Waveform type
     */
    void setWaveShape(WaveShape shape);

    /**
     * Reset internal state
     */
    void reset();

private:
    // Sample rate
    double sampleRate = 44100.0;

    // LFO state
    float lfoPhase = 0.0f;          // Current phase (0.0-1.0)
    float lfoFrequency = 3.0f;      // LFO frequency in Hz
    float modulationDepth = 0.5f;   // Modulation depth (0.0-1.0)
    WaveShape currentShape = WaveShape::MODIFIED_SINE;

    // Chaos/randomness for organic movement
    std::mt19937 rng;
    std::uniform_real_distribution<float> distribution;
    float phaseOffset = 0.0f;       // Random phase offset per cycle

    // Generate LFO output for current phase
    float generateLFO();

    // Waveform generators
    float generateSine();
    float generateModifiedSine();
    float generateTriangle();
};
