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

	for (size_t i = 0; i < std::size(paramList); ++i)
	{
		auto& slider = sliders[i];
		auto& label  = labels[i];

		addAndMakeVisible(slider);
		addAndMakeVisible(label);

		slider.setSliderStyle(juce::Slider::LinearBar);
		slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

		label.setText(paramList[i].name, juce::dontSendNotification);
		label.setJustificationType(juce::Justification::centredLeft);

		attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
			params,
			paramList[i].id,
			slider
		);
	}

	setSize(700, 300);
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
	auto rowHeight = 40;
	int sliderIndex = 0;

	for (int row = 0; true; ++row)
	{
		auto rowArea = area.removeFromTop(rowHeight);

		for (int col = 0; col < 3 && sliderIndex < std::size(paramList); ++col)
		{
			auto cell = rowArea.removeFromLeft(200);

			// Label on the left
			labels[sliderIndex].setBounds(cell.removeFromLeft(90));

			// Slider on the right
			sliders[sliderIndex].setBounds(cell);

			sliderIndex++;
		}
	}
}
