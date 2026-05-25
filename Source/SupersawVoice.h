#pragma once
#include <JuceHeader.h>
#include "SawOsc.h"
#include "SupersawSound.h"

class SupersawVoice : public juce::SynthesiserVoice
{
public:
    static constexpr int numOsc = 24;

    SupersawVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int) override;
    void stopNote (float, bool) override;
    void renderNextBlock (juce::AudioBuffer<float>&, int, int) override;
    void pitchWheelMoved (int) override;
    void controllerMoved (int, int) override;

    juce::AudioProcessorValueTreeState* parameters = nullptr;
	
	void renderSubmixes(float* oct1L, float* oct1R,
					float* oct2L, float* oct2R,
					float* oct3L, float* oct3R,
					int numSamples);
					
	void prepare(double sampleRate, int samplesPerBlock); // called on prepareToPlay

private:
    SawOsc oscillators[numOsc];

    float baseFrequency = 0.0f;
    float oscDetune[numOsc] {};
    float pitchAmount = 0.0f;
    float pitchTime = 0.0f;
    float beenPlayingHowLong = 0.0f; // measured in seconds
	bool isReleasing = false;
	float releaseTime = 0.1f; // measured in seconds
	float releaseProgress = 0.0f; // stops once hits 1.0, regardless of releaseTime; measured as fraction of releaseTime
	
	juce::dsp::IIR::Filter<float> lowpassL;
	juce::dsp::IIR::Filter<float> lowpassR;
    juce::dsp::ProcessSpec spec;
};
