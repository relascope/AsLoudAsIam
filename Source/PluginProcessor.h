/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class AsLoudAsYouAreAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AsLoudAsYouAreAudioProcessor();
    ~AsLoudAsYouAreAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    
    float attackTime, releaseTime, threshold, ratio;

    // Method to calculate the envelope (using RMS or peak detection)
    float calculateEnvelope(juce::AudioBuffer<float>& sidechainBuffer);

    // Apply the gain follower to the buffer (drum track)
    void applyGainFollower(juce::AudioBuffer<float>& buffer, float dynamicLevel);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AsLoudAsYouAreAudioProcessor)
};
