/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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
	
	juce::Slider oct1VolSlider, oct2VolSlider, oct3VolSlider;
	juce::Slider oct1SpreadSlider, oct2SpreadSlider, oct3SpreadSlider;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct1VolAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct2VolAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct3VolAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct1SpreadAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct2SpreadAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oct3SpreadAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SupersweetAudioProcessor& processorRef;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SupersweetAudioProcessorEditor)
};
