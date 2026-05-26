#include "SupersawVoice.h"

SupersawVoice::SupersawVoice()
{
	for (auto& osc : oscillators)
		osc.reset();
}

float sgn(float val) {
	return (0 < val) - (val < 0);
}
float applyGrit(float x, float amount) {
	// "Local function declarations are illegal" have you considered that i'm pure of heart
	float gritted = std::pow(std::abs(x), 0.3) * sgn(x);
	return x + (gritted-x) * amount;
}


bool SupersawVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SupersawSound*>(sound) != nullptr;
}

void SupersawVoice::startNote (int midiNoteNumber, float velocity,
                               juce::SynthesiserSound*, int)
{
    baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) * parameters->getRawParameterValue("masterTune")->load() / 440.0;
    beenPlayingHowLong = 0.0f;
	isReleasing = false;

    float octSpread[3] = {
        parameters->getRawParameterValue("oct1Spread")->load(),
        parameters->getRawParameterValue("oct2Spread")->load(),
        parameters->getRawParameterValue("oct3Spread")->load()
    };
    pitchAmount = parameters->getRawParameterValue("pitchAmount")->load();
    pitchTime = parameters->getRawParameterValue("pitchTime")->load();
	
    int index = 0;
    for (auto& osc : oscillators)
    {
        int group = index / 8;

        float spread = (SawOsc::rng.nextFloat() - 0.5f)
                       * 0.05f * octSpread[group] + 1.0f;

        oscDetune[index] = spread;
        osc.setSampleRate(getSampleRate());
        osc.reset();
        ++index;
    }
}

void SupersawVoice::stopNote (float, bool)
{
	isReleasing = true;
	releaseProgress = 0.0f;
}

void SupersawVoice::renderNextBlock (juce::AudioBuffer<float>& buffer,
                                     int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    float octVol[3] = {
        parameters->getRawParameterValue("oct1Vol")->load(),
        parameters->getRawParameterValue("oct2Vol")->load(),
        parameters->getRawParameterValue("oct3Vol")->load()
    };
	float paramCutoff = parameters->getRawParameterValue("cutoff")->load();
	float paramCutoffAmount = parameters->getRawParameterValue("cutoffAmount")->load();
	float paramCutoffTime = parameters->getRawParameterValue("cutoffTime")->load();
	float paramGrit = parameters->getRawParameterValue("grit")->load();

	float cutoffBase = (0.01 + 0.99 * paramCutoff) * 22000.0;
	float cutoffTopMinusBase = 22000.0 * paramCutoffAmount * (1-paramCutoff);
	float cutoffT = std::exp(-beenPlayingHowLong * 100.0 * std::pow(100, -paramCutoffTime)); // TODO math simplify ?
	float cutoff = cutoffBase + cutoffT * cutoffTopMinusBase;
	auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
		getSampleRate(),
		cutoff
	);
	
    for (int s = 0; s < numSamples; ++s)
    {
        beenPlayingHowLong += 1.0f / getSampleRate();
        float pitchBend = 1.0f
			+ std::exp(
				-beenPlayingHowLong * 222.0f
                * std::pow(10.0f, -pitchTime)
			) * (1.0 + 3.0 * pitchAmount);
		float multiplierForRelease = 1.0;
		if (isReleasing) {
			releaseProgress += 1.0f / (getSampleRate() * releaseTime);
			multiplierForRelease = 1.0 - releaseProgress;
			if (releaseProgress > 1.0) {
				clearCurrentNote();
				isReleasing = false;
				return;
			}
		}

        float left = 0.0f;
        float right = 0.0f;

        int index = 0;
        for (auto& osc : oscillators)
        {
            int group = index / 8;

            float oscFreq = baseFrequency * (group + 1)
                            * oscDetune[index] * pitchBend;
            osc.setFrequency(oscFreq);

            left  += osc.getNextSampleLeft()  * octVol[group];
            right += osc.getNextSampleRight() * octVol[group];

            ++index;
        }
		
		left /= numOsc;
		right /= numOsc;
		
		lowpassL.coefficients = coeffs;
		lowpassR.coefficients = coeffs;
		float filteredL = lowpassL.processSample(left) * multiplierForRelease;
		float filteredR = lowpassR.processSample(right) * multiplierForRelease;
		
		
		filteredL = applyGrit(filteredL, paramGrit);
		filteredR = applyGrit(filteredR, paramGrit);
		
		buffer.addSample(0, startSample + s, filteredL);
		buffer.addSample(1, startSample + s, filteredR);

    }
}

void SupersawVoice::pitchWheelMoved (int) {}
void SupersawVoice::controllerMoved (int, int) {}

void SupersawVoice::prepare(double sampleRate, int samplesPerBlock)
{
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = 2;

	lowpassL.reset();
	lowpassL.prepare(spec);
	lowpassR.reset();
	lowpassR.prepare(spec);

	auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 3000.0f); // 3khz
	lowpassL.coefficients = coeffs;
	lowpassR.coefficients = coeffs;
}
