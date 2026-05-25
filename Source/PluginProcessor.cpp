/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    , parameters(*this, nullptr,
        "PARAMS",
        {
            std::make_unique<juce::AudioParameterFloat>("oct1Vol", "Octave 1 Volume", 0.0f, 1.0f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("oct2Vol", "Octave 2 Volume", 0.0f, 1.0f, 0.75f),
            std::make_unique<juce::AudioParameterFloat>("oct3Vol", "Octave 3 Volume", 0.0f, 1.0f, 0.5f),

            std::make_unique<juce::AudioParameterFloat>("oct1Spread", "Octave 1 Spread", 0.0f, 1.0f, 0.3f),
            std::make_unique<juce::AudioParameterFloat>("oct2Spread", "Octave 2 Spread", 0.0f, 1.0f, 0.4f),
            std::make_unique<juce::AudioParameterFloat>("oct3Spread", "Octave 3 Spread", 0.0f, 1.0f, 0.5f)
        })
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

juce::Random SawOsc::rng;
//==============================================================================
void SupersweetAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	synth.setCurrentPlaybackSampleRate(sampleRate);
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
	// ^^ TODO check do i actually need any of this ? lol
	
	
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
