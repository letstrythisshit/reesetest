#include "ParameterManager.h"

ParameterManager::ParameterManager()
{
    loadFactoryPresets();
}

void ParameterManager::loadFactoryPresets()
{
    factoryPresets.clear();

    createClassicDnBPreset();
    createSubTerrorPreset();
    createIndustrialGrindPreset();
    createDubstepWobblePreset();
    createSmoothOperatorPreset();
    createNeurofunkMadnessPreset();
    createMinimalDeepPreset();
    createDistortedBeastPreset();
}

int ParameterManager::getNumFactoryPresets() const
{
    return static_cast<int>(factoryPresets.size());
}

const ParameterManager::Preset& ParameterManager::getFactoryPreset(int index) const
{
    jassert(index >= 0 && index < static_cast<int>(factoryPresets.size()));
    return factoryPresets[index];
}

void ParameterManager::applyPreset(const Preset& preset, juce::AudioProcessorValueTreeState& parameters)
{
    for (const auto& param : preset.parameters)
    {
        auto* parameter = parameters.getParameter(param.first);
        if (parameter != nullptr)
        {
            parameter->setValueNotifyingHost(param.second);
        }
    }
}

ParameterManager::Preset ParameterManager::createPresetFromCurrentState(
    const juce::AudioProcessorValueTreeState& parameters,
    const juce::String& name,
    const juce::String& description)
{
    Preset preset;
    preset.name = name;
    preset.description = description;
    preset.genre = "User";

    // Get all parameter IDs and values
    const juce::StringArray paramIDs = {"cutoff", "resonance", "ringRate", "ringDepth",
                                        "octaveBlend", "character", "drive", "volume"};

    for (const auto& paramID : paramIDs)
    {
        auto* param = parameters.getRawParameterValue(paramID);
        if (param != nullptr)
        {
            preset.parameters[paramID] = param->load();
        }
    }

    return preset;
}

bool ParameterManager::savePresetToFile(const Preset& preset, const juce::File& filepath)
{
    juce::XmlElement root("ReeseBassPreset");

    root.setAttribute("Name", preset.name);
    root.setAttribute("Description", preset.description);
    root.setAttribute("Genre", preset.genre);

    auto* paramsElement = root.createNewChildElement("Parameters");

    for (const auto& param : preset.parameters)
    {
        paramsElement->setAttribute(param.first, param.second);
    }

    return root.writeTo(filepath);
}

bool ParameterManager::loadPresetFromFile(const juce::File& filepath, Preset& preset)
{
    auto xml = juce::XmlDocument::parse(filepath);

    if (xml == nullptr)
        return false;

    if (!xml->hasTagName("ReeseBassPreset"))
        return false;

    preset.name = xml->getStringAttribute("Name");
    preset.description = xml->getStringAttribute("Description");
    preset.genre = xml->getStringAttribute("Genre");

    auto* paramsElement = xml->getChildByName("Parameters");
    if (paramsElement != nullptr)
    {
        const juce::StringArray paramIDs = {"cutoff", "resonance", "ringRate", "ringDepth",
                                            "octaveBlend", "character", "drive", "volume"};

        for (const auto& paramID : paramIDs)
        {
            preset.parameters[paramID] = static_cast<float>(paramsElement->getDoubleAttribute(paramID));
        }
    }

    return true;
}

//==============================================================================
// Factory Presets
//==============================================================================

void ParameterManager::createClassicDnBPreset()
{
    Preset preset;
    preset.name = "Classic DnB Reese";
    preset.description = "The quintessential drum & bass Reese sound. Medium tension, organic movement, perfect for rolling basslines.";
    preset.genre = "DnB";

    preset.parameters["cutoff"] = 600.0f;
    preset.parameters["resonance"] = 0.70f;
    preset.parameters["ringRate"] = 3.5f;
    preset.parameters["ringDepth"] = 0.50f;
    preset.parameters["octaveBlend"] = 0.40f;
    preset.parameters["character"] = 0.60f;
    preset.parameters["drive"] = 1.20f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createSubTerrorPreset()
{
    Preset preset;
    preset.name = "Sub Terror";
    preset.description = "Deep, ominous sub-bass emphasis. Slow modulation creates breathing effect. Ideal for half-time and liquid DnB.";
    preset.genre = "DnB/Halftime";

    preset.parameters["cutoff"] = 300.0f;
    preset.parameters["resonance"] = 0.80f;
    preset.parameters["ringRate"] = 1.5f;
    preset.parameters["ringDepth"] = 0.30f;
    preset.parameters["octaveBlend"] = 0.70f;
    preset.parameters["character"] = 0.40f;
    preset.parameters["drive"] = 1.00f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createIndustrialGrindPreset()
{
    Preset preset;
    preset.name = "Industrial Grind";
    preset.description = "Aggressive, harsh, grinding Reese. Fast modulation and heavy saturation. Perfect for dark techno and industrial.";
    preset.genre = "Industrial/Techno";

    preset.parameters["cutoff"] = 800.0f;
    preset.parameters["resonance"] = 0.85f;
    preset.parameters["ringRate"] = 7.0f;
    preset.parameters["ringDepth"] = 0.75f;
    preset.parameters["octaveBlend"] = 0.35f;
    preset.parameters["character"] = 0.80f;
    preset.parameters["drive"] = 1.80f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createDubstepWobblePreset()
{
    Preset preset;
    preset.name = "Dubstep Wobble";
    preset.description = "High cutoff with extreme resonance creates wobble-like movement. Brutal and in-your-face.";
    preset.genre = "Dubstep";

    preset.parameters["cutoff"] = 1200.0f;
    preset.parameters["resonance"] = 0.90f;
    preset.parameters["ringRate"] = 4.0f;
    preset.parameters["ringDepth"] = 0.85f;
    preset.parameters["octaveBlend"] = 0.20f;
    preset.parameters["character"] = 0.70f;
    preset.parameters["drive"] = 1.50f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createSmoothOperatorPreset()
{
    Preset preset;
    preset.name = "Smooth Operator";
    preset.description = "Subtle Reese effect, adds slight detuning and warmth without overwhelming the clean bass. Great for funk and house.";
    preset.genre = "House/Funk";

    preset.parameters["cutoff"] = 700.0f;
    preset.parameters["resonance"] = 0.40f;
    preset.parameters["ringRate"] = 2.0f;
    preset.parameters["ringDepth"] = 0.25f;
    preset.parameters["octaveBlend"] = 0.30f;
    preset.parameters["character"] = 0.35f;
    preset.parameters["drive"] = 0.80f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createNeurofunkMadnessPreset()
{
    Preset preset;
    preset.name = "Neurofunk Madness";
    preset.description = "Chaotic, complex Reese with fast modulation. Creates unpredictable harmonic content. Pure neurofunk energy.";
    preset.genre = "Neurofunk";

    preset.parameters["cutoff"] = 500.0f;
    preset.parameters["resonance"] = 0.75f;
    preset.parameters["ringRate"] = 8.5f;
    preset.parameters["ringDepth"] = 0.90f;
    preset.parameters["octaveBlend"] = 0.50f;
    preset.parameters["character"] = 0.90f;
    preset.parameters["drive"] = 1.60f;
    preset.parameters["volume"] = -2.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createMinimalDeepPreset()
{
    Preset preset;
    preset.name = "Minimal Deep";
    preset.description = "Very subtle effect, mostly sub-octave reinforcement. Adds weight without obvious modulation. Minimal techno friendly.";
    preset.genre = "Minimal/Techno";

    preset.parameters["cutoff"] = 400.0f;
    preset.parameters["resonance"] = 0.30f;
    preset.parameters["ringRate"] = 1.0f;
    preset.parameters["ringDepth"] = 0.20f;
    preset.parameters["octaveBlend"] = 0.60f;
    preset.parameters["character"] = 0.30f;
    preset.parameters["drive"] = 0.90f;
    preset.parameters["volume"] = 0.0f;

    factoryPresets.push_back(preset);
}

void ParameterManager::createDistortedBeastPreset()
{
    Preset preset;
    preset.name = "Distorted Beast";
    preset.description = "Maximum saturation creates gritty, distorted character. Aggressive and punchy. Crossbreed/darkstep specialist.";
    preset.genre = "Crossbreed/Darkstep";

    preset.parameters["cutoff"] = 900.0f;
    preset.parameters["resonance"] = 0.65f;
    preset.parameters["ringRate"] = 5.0f;
    preset.parameters["ringDepth"] = 0.60f;
    preset.parameters["octaveBlend"] = 0.45f;
    preset.parameters["character"] = 0.75f;
    preset.parameters["drive"] = 2.00f;
    preset.parameters["volume"] = -3.0f;

    factoryPresets.push_back(preset);
}
