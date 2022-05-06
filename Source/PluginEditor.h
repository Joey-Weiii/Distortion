/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <string>

using namespace juce;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class DistortionAudioProcessorEditor  : public juce::AudioProcessorEditor , public Timer
{
public:
    DistortionAudioProcessorEditor (DistortionAudioProcessor&,AudioProcessorValueTreeState& vts);
    ~DistortionAudioProcessorEditor() override;

    //==============================================================================

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    DistortionAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    // envs
    double hostSampleRate = 48000.;
    
    // input gain
    Slider inputGainSlider;
    Label inputGainLabel;

    // output gain
    Slider outputGainSlider;
    Label outputGainLabel;

    // -- monitor -- //
    Label bufferTimeLabel;

    // attachments
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};
