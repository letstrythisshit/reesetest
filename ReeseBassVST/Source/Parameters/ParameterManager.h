#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>
#include <map>

/**
 * ParameterManager - Preset Management System
 *
 * Manages factory presets and user presets for the Reese Destroyer plugin.
 * Handles loading, saving, and importing/exporting preset files.
 */
class ParameterManager
{
public:
    /**
     * Structure representing a single preset
     */
    struct Preset
    {
        juce::String name;
        juce::String description;
        juce::String genre;
        std::map<juce::String, float> parameters;
    };

    ParameterManager();

    /**
     * Initialize factory presets
     */
    void loadFactoryPresets();

    /**
     * Get the number of factory presets
     */
    int getNumFactoryPresets() const;

    /**
     * Get a factory preset by index
     * @param index Preset index (0-7)
     * @return Preset structure
     */
    const Preset& getFactoryPreset(int index) const;

    /**
     * Apply a preset to the parameter tree
     * @param preset The preset to apply
     * @param parameters The parameter tree state
     */
    void applyPreset(const Preset& preset, juce::AudioProcessorValueTreeState& parameters);

    /**
     * Create a preset from current parameter values
     * @param parameters The parameter tree state
     * @param name Preset name
     * @param description Preset description
     * @return New preset
     */
    Preset createPresetFromCurrentState(
        const juce::AudioProcessorValueTreeState& parameters,
        const juce::String& name,
        const juce::String& description);

    /**
     * Save a preset to XML file
     * @param preset The preset to save
     * @param filepath Output file path
     * @return True if successful
     */
    bool savePresetToFile(const Preset& preset, const juce::File& filepath);

    /**
     * Load a preset from XML file
     * @param filepath Input file path
     * @param preset Output preset structure
     * @return True if successful
     */
    bool loadPresetFromFile(const juce::File& filepath, Preset& preset);

private:
    std::vector<Preset> factoryPresets;

    // Factory preset data
    void createClassicDnBPreset();
    void createSubTerrorPreset();
    void createIndustrialGrindPreset();
    void createDubstepWobblePreset();
    void createSmoothOperatorPreset();
    void createNeurofunkMadnessPreset();
    void createMinimalDeepPreset();
    void createDistortedBeastPreset();
};
