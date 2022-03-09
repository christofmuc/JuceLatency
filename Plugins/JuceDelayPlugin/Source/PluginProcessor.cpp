#include "PluginProcessor.h"

#include "mtdm.h"

MinimalAudioPlugin::MinimalAudioPlugin()
    : sampleRate_(48000.0)
    , samplesPerBlock_(128)
{
    delayInSamples_ = new juce::AudioParameterFloat ("delay", "Delay in Samples", 0.0f, 50000.0f, 0.0f);
    delayInMS_ = new juce::AudioParameterFloat("delay", "Delay in ms", 0.0f, 1000.0f, 0.0f);
    noSignalDetected_ =new juce::AudioParameterBool("noSignal", "No Signal present", true);
    invertPhaseActive_ = new juce::AudioParameterBool  ("invertPhase", "Invert Phase", false);
    errorConditionTrue_ = new juce::AudioParameterBool("errorCondition", "Error Condition", false);

    addParameter(delayInSamples_);
    addParameter(delayInMS_);
    addParameter(noSignalDetected_);
    addParameter(invertPhaseActive_);
    addParameter(errorConditionTrue_);
}

void MinimalAudioPlugin::processBlock(juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    if (buffer.getNumChannels() > 0) {
        // Only process the first channel
        mtdm_->process(buffer.getNumSamples(), buffer.getReadPointer(0), buffer.getWritePointer(0));
        detectLatency(); // I think we can affort to do this at packet rate. The original code sleeps 250ms between calculations
    }
}

void MinimalAudioPlugin::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mtdm_ = std::make_unique<MTDM>(static_cast<int>(round(sampleRate)));
    sampleRate_ = sampleRate;
    samplesPerBlock_ = samplesPerBlock;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MinimalAudioPlugin();
}

void MinimalAudioPlugin::detectLatency()
{
    if (mtdm_->resolve() < 0)
    {
        *noSignalDetected_ = true;
    }
    else
    {
        *noSignalDetected_ = false;
        if (mtdm_->err() > 0.35f)
        {
            mtdm_->invert();
            mtdm_->resolve();
        }
        *delayInSamples_ = mtdm_->del();
        *errorConditionTrue_ = (mtdm_->err() > 0.30f);
        *invertPhaseActive_ = mtdm_->inv();
        double t = 1000.0 / sampleRate_;
        *delayInMS_ = delayInSamples_->get() * t;
    }
}
