/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct ParamInfo
{
	const char* id;
	const char* name;
	float minValue;
	float maxValue;
	float defaultValue;
};
static constexpr ParamInfo paramList[] =
{
	{ "oct1Vol", "O1 Vol", 0.0, 1.0, 1.0f },
	{ "oct2Vol", "O2 Vol", 0.0, 1.0, 0.75f },
	{ "oct3Vol", "O3 Vol", 0.0, 1.0, 0.5f },
	
	{ "oct1Spread", "O1 Spread", 0.0, 1.0, 0.3f },
	{ "oct2Spread", "O2 Spread", 0.0, 1.0, 0.4f },
	{ "oct3Spread", "O3 Spread", 0.0, 1.0, 0.5f },
	
	{ "masterTune", "Master Tune", 400.0, 500.0, 440.0 },
	{ "pitchAmount", "Pitch Amount", 0.0, 1.0, 0.333f }, // 1x mult to 4x mult
	{ "pitchTime", "Pitch Time", 0.0, 1.0, 0.3f }
};
/**
O1 Vol			O2 Vol				O3 Vol
O1 Spread		O2 Spread			O3 Spread
Master Tune		Pitch Amount		Pitch Time
Cutoff			Cutoff Amount		Cutoff Time
Release			Organ				Grit
*/

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
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[std::size(paramList)];
	juce::Label labels[std::size(paramList)];

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SupersweetAudioProcessor& processorRef;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SupersweetAudioProcessorEditor)
};
