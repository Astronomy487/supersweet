/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SupersweetAudioProcessorEditor::SupersweetAudioProcessorEditor (SupersweetAudioProcessor& p)
	: AudioProcessorEditor (&p), processorRef (p)
{
	auto& params = processorRef.parameters;

	addAndMakeVisible(oct1VolSlider);
	addAndMakeVisible(oct2VolSlider);
	addAndMakeVisible(oct3VolSlider);
	addAndMakeVisible(oct1SpreadSlider);
	addAndMakeVisible(oct2SpreadSlider);
	addAndMakeVisible(oct3SpreadSlider);

	oct1VolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct1Vol", oct1VolSlider);
	oct2VolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct2Vol", oct2VolSlider);
	oct3VolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct3Vol", oct3VolSlider);
	oct1SpreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct1Spread", oct1SpreadSlider);
	oct2SpreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct2Spread", oct2SpreadSlider);
	oct3SpreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oct3Spread", oct3SpreadSlider);

	for (auto* s : { &oct1VolSlider, &oct2VolSlider, &oct3VolSlider,
					 &oct1SpreadSlider, &oct2SpreadSlider, &oct3SpreadSlider })
	{
		s->setSliderStyle(juce::Slider::LinearBar);
		s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	}

	setSize (487, 301);
}


SupersweetAudioProcessorEditor::~SupersweetAudioProcessorEditor()
{
}

//==============================================================================
void SupersweetAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    /* g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1); */
}

void SupersweetAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(20);
	auto rowHeight = 80;

	auto row1 = area.removeFromTop(rowHeight);
	oct1VolSlider.setBounds(row1.removeFromLeft(150));
	oct1SpreadSlider.setBounds(row1.removeFromLeft(150));

	auto row2 = area.removeFromTop(rowHeight);
	oct2VolSlider.setBounds(row2.removeFromLeft(150));
	oct2SpreadSlider.setBounds(row2.removeFromLeft(150));

	auto row3 = area.removeFromTop(rowHeight);
	oct3VolSlider.setBounds(row3.removeFromLeft(150));
	oct3SpreadSlider.setBounds(row3.removeFromLeft(150));
}
