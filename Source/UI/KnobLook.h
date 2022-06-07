/*
  ==============================================================================

    KnobLook.h
    Created: 24 May 2022 7:49:54pm
    Author:  JOEY

  ==============================================================================
*/
#include "JuceHeader.h"
#include "../PluginProcessor.h"

#pragma once

using namespace juce;

class KnobLook : public LookAndFeel_V4
{
public:
    KnobLook(DistortionAudioProcessor&);
    ~KnobLook();

    void drawRotarySlider(
        Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
        Slider& slider) override;

    void setSliderColour(Colour colourFront, Colour colourBack);
    int getGainInterval(float current_val);
    StringArray getKnobFileNames();
    void getImage(const juce::StringArray& name);
    void set_as_in_knob();
    void set_as_out_knob();
private:
    DistortionAudioProcessor& processor;
    Image knobImage[31];
    Image temp;
    // knob type 0 for inputGain , 1 for outputGain
    int knob_type=0;
};