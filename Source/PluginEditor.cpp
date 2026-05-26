/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
		slider.setColour(juce::Slider::trackColourId, juce::Colour::fromString("ffddff00"));
		slider.setColour(juce::Slider::thumbColourId, juce::Colour::fromString("ffddff00"));

		label.setText(paramList[i].name, juce::dontSendNotification);
		label.setJustificationType(juce::Justification::centredLeft);
		label.setColour(juce::Label::textColourId, juce::Colour::fromString("ffddff00"));

		attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
			params,
			paramList[i].id,
			slider
		);
	}

	setSize(700, 400);
}




SupersweetAudioProcessorEditor::~SupersweetAudioProcessorEditor()
{
}

//==============================================================================
void SupersweetAudioProcessorEditor::paint (juce::Graphics& g)
{
	// Dark background
	g.fillAll(juce::Colour::fromRGB(10, 10, 10));
}


void SupersweetAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(20);

	const int rows = 5;
	const int cols = 3;

	const int cellWidth  = area.getWidth()  / cols;
	const int cellHeight = area.getHeight() / rows;

	int index = 0;

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			if (index >= std::size(paramList))
				return;

			// Compute the cell for this parameter
			auto cell = area.withTrimmedTop(row * cellHeight)
							.withTrimmedLeft(col * cellWidth)
							.removeFromTop(cellHeight)
							.removeFromLeft(cellWidth)
							.reduced(10); // padding inside each cell

			// Label at top
			labels[index].setBounds(cell.removeFromTop(20));

			// Slider fills the rest
			sliders[index].setBounds(cell);

			++index;
		}
	}
}
