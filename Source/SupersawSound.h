#pragma once
#include <JuceHeader.h>

class SupersawSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override      { return true; }
    bool appliesToChannel (int) override   { return true; }
};
