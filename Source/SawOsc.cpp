#include "SawOsc.h"

juce::Random SawOsc::rng;

void SawOsc::setSampleRate(double sr) { sampleRate = sr; updatePhaseDelta(); }
void SawOsc::setFrequency(double freq) { frequency = freq; updatePhaseDelta(); }

void SawOsc::reset()
{
    leftPhase  = rng.nextFloat() * twoPi;
    rightPhase = rng.nextFloat() * twoPi;
}

float SawOsc::getNextSampleLeft()
{
    float value = (float)(2.0 * (leftPhase / twoPi) - 1.0);
	// value = std::sin(leftPhase);
    leftPhase += phaseDelta;
    if (leftPhase >= twoPi) leftPhase -= twoPi;
    return value;
}

float SawOsc::getNextSampleRight()
{
    float value = (float)(2.0 * (rightPhase / twoPi) - 1.0);
	// value = std::sin(rightPhase);
    rightPhase += phaseDelta;
    if (rightPhase >= twoPi) rightPhase -= twoPi;
    return value;
}

void SawOsc::updatePhaseDelta()
{
    phaseDelta = (twoPi * frequency) / sampleRate;
}
