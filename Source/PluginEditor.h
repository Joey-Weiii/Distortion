/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/DistFunctionComponent.h"
#include "UI/ButtonLook.h"
#include "UI/KnobLook.h"
#include <string>

using namespace juce;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class DistortionAudioProcessorEditor  : public juce::AudioProcessorEditor , public Timer, public Button::Listener
{
public:
    DistortionAudioProcessorEditor (DistortionAudioProcessor&,AudioProcessorValueTreeState& vts);
    ~DistortionAudioProcessorEditor() override;

    //==============================================================================

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void buttonClicked(Button* button) override;


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
    
    //KnobLook
    KnobLook inputKnobLook, outputKnobLook;
    // -- monitor -- //
    Label bufferTimeLabel;


    // button of 3 function
    TextButton tanhButton{ "" },softButton{ "" },hardButton{ "" };
    ButtonLook tanhButtonLook, softButtonLook, hardButtonLook;

    // attachments
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    
    DistFunctionComponent distFunctionComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};
