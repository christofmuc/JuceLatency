#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

//Inhereting from PluginHelpers::ProcessorBase, which is just inhereting from juce::AudioProcessor
//And adding some default implementations

class MTDM;

class MinimalAudioPlugin : public PluginHelpers::ProcessorBase
{
public:
    MinimalAudioPlugin();

    void processBlock(juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

private:
    void detectLatency();

    std::unique_ptr<MTDM> mtdm_;
    double sampleRate_;
    int samplesPerBlock_; 

    juce::AudioParameterFloat* delayInSamples_;
    juce::AudioParameterFloat* delayInMS_;
    juce::AudioParameterBool* noSignalDetected_;
    juce::AudioParameterBool* invertPhaseActive_;
    juce::AudioParameterBool* errorConditionTrue_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinimalAudioPlugin)
};

