/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

// TODO holy moly why did you put everything here

#pragma once

#include <JuceHeader.h>

class SupersawSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override      { return true; }
    bool appliesToChannel (int) override   { return true; }
};

class SawOsc
{
public:
	void setSampleRate(double sr) {
		sampleRate = sr;
		updatePhaseDelta();
	}
	void setFrequency(double freq) {
		frequency = freq;
		updatePhaseDelta();
	}
	static juce::Random rng;
	void reset() {
		leftPhase  = rng.nextFloat() * juce::MathConstants<double>::twoPi;
		rightPhase = rng.nextFloat() * juce::MathConstants<double>::twoPi;
	}

	float getNextSampleLeft() {
		float value = (float)(2.0 * (leftPhase / twoPi) - 1.0);
		leftPhase += phaseDelta;
		if (leftPhase >= twoPi) leftPhase -= twoPi;
		return value;
	}
	float getNextSampleRight() {
		float value = (float)(2.0 * (rightPhase / twoPi) - 1.0);
		rightPhase += phaseDelta;
		if (rightPhase >= twoPi) rightPhase -= twoPi;
		return value;
	}
private:
	void updatePhaseDelta() {
		phaseDelta = (twoPi * frequency) / sampleRate;
	}
	double leftPhase = 0.0;
	double rightPhase = 0.0;
	double phaseDelta = 0.0;
	double frequency = 440.0;
	double sampleRate = 44100.0;
	static constexpr double twoPi = juce::MathConstants<double>::twoPi;
};

class SupersawVoice : public juce::SynthesiserVoice
{
public:
    static constexpr int numOsc = 24;
	juce::AudioProcessorValueTreeState* parameters = nullptr;
	float groupVolume[3] = {1.0f, 1.0f, 1.0f};

    SupersawVoice()
    {
        for (auto& osc : oscillators)
            osc.reset();
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SupersawSound*>(sound) != nullptr;
    }

	void startNote (int midiNoteNumber, float velocity,
					juce::SynthesiserSound*, int) override
	{
		auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // TODO check a432 stuff

		float octSpread[3] = {
			parameters->getRawParameterValue("oct1Spread")->load(),
			parameters->getRawParameterValue("oct2Spread")->load(),
			parameters->getRawParameterValue("oct3Spread")->load()
		};

		int index = 0;
		for (auto& osc : oscillators)
		{
			int group = index / 8;
			index++;

			float spread = (SawOsc::rng.nextFloat() - 0.5f)
						   * 0.05f * octSpread[group] + 1.0f;

			float oscFreq = freq * (group + 1) * spread;

			osc.setSampleRate(getSampleRate());
			osc.setFrequency(oscFreq);
			osc.reset();
		}
	}


    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        clearCurrentNote();
    }

    void renderNextBlock (juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
		if (! isVoiceActive()) return;
		
		
		float octVol[3] = {
			parameters->getRawParameterValue("oct1Vol")->load(),
			parameters->getRawParameterValue("oct2Vol")->load(),
			parameters->getRawParameterValue("oct3Vol")->load()
		};
    
		for (int s = 0; s < numSamples; ++s)
        {
            float left = 0.0f;
            float right = 0.0f;

			int index = 0;

			for (auto& osc : oscillators)
			{
				int group = index / 8;
				index++;
				left  += osc.getNextSampleLeft()  * octVol[group];
				right += osc.getNextSampleRight() * octVol[group];
			}


            left  /= numOsc;
            right /= numOsc;

            buffer.addSample(0, startSample + s, left);
            buffer.addSample(1, startSample + s, right);
        }
    }
	void pitchWheelMoved (int newPitchWheelValue) override
	{
		// ignore for now
	}

	void controllerMoved (int controllerNumber, int newControllerValue) override
	{
		// ignore for now
	}


private:
    SawOsc oscillators[numOsc];
};


//==============================================================================
/**
*/
class SupersweetAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SupersweetAudioProcessor();
    ~SupersweetAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	
	juce::AudioProcessorValueTreeState parameters;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SupersweetAudioProcessor)
	
	juce::Synthesiser synth;
};