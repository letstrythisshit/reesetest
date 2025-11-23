#pragma once

#include <JuceHeader.h>
#include <cmath>

/**
 * WaveShapers - Wave Shaping and Saturation Module
 *
 * Implements multiple saturation and wave-shaping algorithms to add harmonic
 * richness and analog character to the Reese bass sound.
 *
 * Benefits of controlled distortion for Reese bass:
 * - Creates additional harmonics (thickens sound)
 * - Reduces dynamic range (sustains better)
 * - Adds "grit" and "weight" to low frequencies
 * - Mimics analog circuit non-linearities
 *
 * Saturation Types:
 * - Soft Clip: Smooth overdrive
 * - Diode Clip: Hardware-style diode clipping
 * - Asymmetric: Tube-style saturation
 * - Multi-stage: Cascaded saturation for complex character
 */
class WaveShapers
{
public:
    enum class SaturationType
    {
        SOFT_CLIP,      // Smooth tanh-based overdrive
        DIODE_CLIP,     // Silicon diode forward voltage simulation
        ASYMMETRIC,     // Tube-style asymmetric saturation
        MULTI_STAGE     // Cascaded saturation stages
    };

    WaveShapers();

    /**
     * Prepare the module for processing
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);

    /**
     * Process a single sample through the wave shaper
     * @param input Input audio sample
     * @return Shaped/saturated output sample
     */
    float process(float input);

    /**
     * Set the drive amount (pre-gain before saturation)
     * @param drive 0.0-2.0 (0% to 200%)
     */
    void setDrive(float drive);

    /**
     * Set the saturation type
     * @param type Saturation algorithm to use
     */
    void setSaturationType(SaturationType type);

    /**
     * Set the character (controls saturation shape)
     * @param character 0.0 = smooth, 1.0 = aggressive
     */
    void setCharacter(float character);

    /**
     * Reset internal state
     */
    void reset();

private:
    // Sample rate
    double sampleRate = 44100.0;

    // Parameters
    float drive = 1.0f;                  // Pre-gain (0.0-2.0)
    float character = 0.5f;              // Character control (0.0-1.0)
    float outputGain = 0.7f;             // Output compensation
    SaturationType currentType = SaturationType::MULTI_STAGE;

    // Saturation algorithms
    float softClip(float input);
    float diodeClipper(float input);
    float asymmetricSaturation(float input);
    float multiStage(float input);

    // Soft clipping with threshold
    float softClipWithThreshold(float input, float threshold);
};
