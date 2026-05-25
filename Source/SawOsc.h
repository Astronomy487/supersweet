#pragma once
#include <JuceHeader.h>

class SawOsc
{
public:
    void setSampleRate(double sr);
    void setFrequency(double freq);
    static juce::Random rng;
    void reset();

    float getNextSampleLeft();
    float getNextSampleRight();

private:
    void updatePhaseDelta();

    double leftPhase = 0.0;
    double rightPhase = 0.0;
    double phaseDelta = 0.0;
    double frequency = 440.0;
    double sampleRate = 44100.0;
    static constexpr double twoPi = juce::MathConstants<double>::twoPi;
};
