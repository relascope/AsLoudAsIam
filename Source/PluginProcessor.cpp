/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AsLoudAsIamAudioProcessor::AsLoudAsIamAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
            #if ! JucePlugin_IsSynth
                .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
            #endif
            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
            .withInput  ("Sidechain", juce::AudioChannelSet::stereo(), false) // Add sidechain input here
        #endif
    )
#endif
    , attackTime(0.01), releaseTime(0.1), threshold(0.0f), ratio(1.0f)
{
}

AsLoudAsIamAudioProcessor::~AsLoudAsIamAudioProcessor()
{
}

//==============================================================================
const juce::String AsLoudAsIamAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AsLoudAsIamAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AsLoudAsIamAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AsLoudAsIamAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AsLoudAsIamAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AsLoudAsIamAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AsLoudAsIamAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AsLoudAsIamAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AsLoudAsIamAudioProcessor::getProgramName (int index)
{
    return {};
}

void AsLoudAsIamAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AsLoudAsIamAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AsLoudAsIamAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AsLoudAsIamAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AsLoudAsIamAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    // Get sidechain input (guitar track)
    // Access the sidechain input buffer (guitar track)
    
    // Access the sidechain input buffer (guitar track)
    auto sideChainBus = getBus(true, 1);  // Get the sidechain input bus (index 1 for sidechain)

    if (sideChainBus != nullptr)
    {
        // Access sidechain buffer
        auto sidechainBuffer = sideChainBus->getBusBuffer(buffer);

        // Calculate the dynamic level from the sidechain input (guitar track)
        float dynamicLevel = calculateEnvelope(sidechainBuffer);

        // Apply the dynamic level to the drum track signal (buffer)
        applyGainFollower(buffer, dynamicLevel);
    }
}

// Method to calculate the envelope (using RMS or peak detection)
float AsLoudAsIamAudioProcessor::calculateEnvelope(juce::AudioBuffer<float>& sidechainBuffer)
{
    float maxLevel = 0.0f;
    for (int channel = 0; channel < sidechainBuffer.getNumChannels(); ++channel)
    {
        maxLevel = std::max(maxLevel, sidechainBuffer.getMagnitude(channel, 0, sidechainBuffer.getNumSamples()));
    }
    return maxLevel;
}

// Apply the gain follower to the buffer (drum track)
void AsLoudAsIamAudioProcessor::applyGainFollower(juce::AudioBuffer<float>& buffer, float dynamicLevel)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float newGain = std::max(0.0f, dynamicLevel - threshold) * ratio;
            buffer.setSample(channel, sample, buffer.getSample(channel, sample) * newGain);
        }
    }
}

//==============================================================================
bool AsLoudAsIamAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AsLoudAsIamAudioProcessor::createEditor()
{
    return new AsLoudAsIamAudioProcessorEditor (*this);
}

//==============================================================================
void AsLoudAsIamAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AsLoudAsIamAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AsLoudAsIamAudioProcessor();
}
