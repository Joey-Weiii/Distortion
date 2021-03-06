/*
  ==============================================================================

    ButtonComponent.h
    Created: 6 May 2022 7:05:01pm
    Author:  JOEY

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../PluginProcessor.h"
//#include "ButtonLook.h"

using namespace juce;

class DistFunctionComponent :public Component
{
public:
    DistFunctionComponent(DistortionAudioProcessor&);
    ~DistFunctionComponent();

   // void buttonClicked(juce::Button* ) override;
    void paint(Graphics&) override;
    void resized() override;

private:

    Label func_no,func_tanh, func_soft, func_hard;
    DistortionAudioProcessor& processor;
    Image func[4];

};