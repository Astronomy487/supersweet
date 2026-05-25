/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SupersawSound.h"
#include "SupersawVoice.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	for (const auto& p : paramList)
	{
		layout.add(std::make_unique<juce::AudioParameterFloat>(
			p.id, p.name,
			juce::NormalisableRange<float>(0.0f, 1.0f),
			p.defaultValue
		));
	}

	return layout;
}


//==============================================================================
SupersweetAudioProcessor::SupersweetAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , parameters(*this, nullptr, "PARAMS", createParameterLayout())
#else
    : parameters(*this, nullptr, "PARAMS", { ... })
#endif
{
    synth.clearVoices();
    for (int i = 0; i < 16; ++i)
    {
        auto* v = new SupersawVoice();
        v->parameters = &parameters;
        synth.addVoice(v);
    }

    synth.clearSounds();
    synth.addSound(new SupersawSound());
}

SupersweetAudioProcessor::~SupersweetAudioProcessor()
{
}

//==============================================================================
const juce::String SupersweetAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SupersweetAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SupersweetAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SupersweetAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SupersweetAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SupersweetAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SupersweetAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SupersweetAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SupersweetAudioProcessor::getProgramName (int index)
{
    return {};
}

void SupersweetAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SupersweetAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	synth.setCurrentPlaybackSampleRate(sampleRate);

	for (int i = 0; i < synth.getNumVoices(); ++i)
	{
		if (auto* v = dynamic_cast<SupersawVoice*>(synth.getVoice(i)))
			v->prepare(sampleRate, samplesPerBlock);
	}
}


void SupersweetAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SupersweetAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SupersweetAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
	
	buffer.clear();
	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SupersweetAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SupersweetAudioProcessor::createEditor()
{
    return new SupersweetAudioProcessorEditor (*this);
}

//==============================================================================
void SupersweetAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml (state.createXml());
	copyXmlToBinary (*xml, destData);
}


void SupersweetAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

	if (xml.get() != nullptr && xml->hasTagName (parameters.state.getType()))
	{
		parameters.replaceState (juce::ValueTree::fromXml (*xml));
	}
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SupersweetAudioProcessor();
}
