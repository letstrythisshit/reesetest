#pragma once

#include <juce_core/juce_core.h>
#include <cmath>

/**
 * StateVariableFilter - Resonant State Variable Filter
 *
 * Implements a 2-pole (12dB/octave) state-variable filter with simultaneous
 * low-pass, band-pass, and high-pass outputs.
 *
 * Based on the trapezoidal integration method (mimics analog capacitor charging).
 * This filter is modeled after Sallen-Key topology used in analog active filters.
 *
 * Characteristics:
 * - Type: 2-pole (12dB/octave)
 * - Cutoff: 200-2000Hz range
 * - Resonance: 0-95% (approaches self-oscillation at 100%)
 * - Response: Smooth, musical (analog character)
 */
class StateVariableFilter
{
public:
    /**
     * Filter output structure containing all three filter types
     */
    struct FilterOutput
    {
        float lowPass = 0.0f;
        float bandPass = 0.0f;
        float highPass = 0.0f;
    };

    StateVariableFilter();

    /**
     * Prepare the module for processing
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);

    /**
     * Process a single sample
     * @param input Input audio sample
     * @return FilterOutput containing LP, BP, and HP outputs
     */
    FilterOutput processSample(float input);

    /**
     * Set the cutoff frequency
     * @param hz Frequency in Hz (20-20000Hz)
     */
    void setCutoff(float hz);

    /**
     * Set the resonance (Q factor)
     * @param resonance 0.0-0.95 (0% to 95%)
     */
    void setResonance(float resonance);

    /**
     * Set input drive/saturation
     * @param drive 0.0-2.0 (0% to 200%)
     */
    void setDrive(float drive);

    /**
     * Reset internal state
     */
    void reset();

private:
    // Sample rate
    double sampleRate = 44100.0;

    // Filter state variables (integrators)
    float ic1eq = 0.0f;  // Integrator 1 state
    float ic2eq = 0.0f;  // Integrator 2 state

    // Filter parameters
    float cutoffFreq = 500.0f;    // Cutoff frequency in Hz
    float resonance = 0.5f;       // Resonance (0.0-0.95)
    float drive = 1.0f;           // Input drive (0.0-2.0)

    // Pre-calculated coefficients
    float g = 0.0f;  // Cutoff coefficient
    float k = 0.0f;  // Resonance coefficient (damping)

    // Coefficient update flag
    bool needsUpdate = true;

    /**
     * Update filter coefficients when parameters change
     */
    void updateCoefficients();

    /**
     * Soft saturation to mimic op-amp saturation in analog filters
     */
    float saturateInput(float x);
};
