/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AsLoudAsYouAreAudioProcessorEditor::AsLoudAsYouAreAudioProcessorEditor (AsLoudAsYouAreAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

AsLoudAsYouAreAudioProcessorEditor::~AsLoudAsYouAreAudioProcessorEditor()
{
}

//==============================================================================
void AsLoudAsYouAreAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));

    auto bounds = getLocalBounds().reduced (20); // 20 px Abstand rundherum
    g.drawFittedText (
        "Just set the Side Chain to the track/bus/input you want to follow and Be As Loud as You (Side Chain) are. \n\nDoJoy!",
        bounds,
        juce::Justification::centred,
        3 // mehr Zeilen erlauben
    );
}

void AsLoudAsYouAreAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
