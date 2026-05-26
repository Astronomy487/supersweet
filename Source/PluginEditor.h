/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"

//==============================================================================
/**
*/
class SupersweetAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SupersweetAudioProcessorEditor (SupersweetAudioProcessor&);
    ~SupersweetAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	
	juce::Slider sliders[std::size(paramList)];
    juce::Label labels[std::size(paramList)];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[std::size(paramList)];

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SupersweetAudioProcessor& processorRef;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SupersweetAudioProcessorEditor)
};
