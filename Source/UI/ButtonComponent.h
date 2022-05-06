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

using namespace juce;

class ButtonComponent :public Component, public Button::Listener, public LookAndFeel_V4
{
public:
    ButtonComponent(DistortionAudioProcessor&);
    ~ButtonComponent();

    void buttonClicked(juce::Button* button) override;
    void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
                                bool, bool isButtonDown) override;
    void paint(Graphics&) override;
    void resized() override;
    Rectangle<int> ButtonComponent::getButtonBounds();

private:
    LookAndFeel_V4 distortionButtonLook;
    TextButton distortionButton;
    Image on,off;
    DistortionAudioProcessor& processor;
};