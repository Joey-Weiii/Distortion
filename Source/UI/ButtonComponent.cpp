/*
  ==============================================================================

    ButtonComponent.cpp
    Created: 6 May 2022 6:34:27pm
    Author:  JOEY

  ==============================================================================
*/
#include <JuceHeader.h>
#include <vector>
#include "ButtonComponent.h"

using namespace juce;

ButtonComponent::ButtonComponent(DistortionAudioProcessor& p):processor(p)
{
    on = ImageCache::getFromMemory(BinaryData::auto_gain_btn_on_png, BinaryData::auto_gain_btn_on_pngSize);
    off = ImageCache::getFromMemory(BinaryData::auto_gain_btn_off_png, BinaryData::auto_gain_btn_off_pngSize);

    addAndMakeVisible(distortionButton);
    distortionButton.setButtonText("tanh");
    distortionButton.addListener(this);
    distortionButton.setLookAndFeel(&distortionButtonLook);

    repaint();
}

void ButtonComponent::buttonClicked(juce::Button* button)
{
    if (button == &distortionButton)
    {
        processor.changeDistortionState();
    }

}
ButtonComponent::~ButtonComponent()
{
    distortionButton.removeListener(this);
}

void ButtonComponent::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,bool, bool isButtonDown)
{
    auto area= getButtonBounds();

    if (processor.isDistortionActivate() == true)
    {
        g.drawImageWithin(on,
            area.getX(),
            area.getY(),
            area.getWidth(),
            area.getHeight(),
            RectanglePlacement::centred);
    }
    else
    {
        g.drawImageWithin(off,
            area.getX(),
            area.getY(),
            area.getWidth(),
            area.getHeight(),
            RectanglePlacement::centred);
    }
        
}

void ButtonComponent::paint(Graphics&g)
{
    g.setColour(Colours::pink);
    auto bounds = Rectangle<int>(getButtonBounds().getX() - 1, getButtonBounds().getY() - 1, getButtonBounds().getWidth() + 2, getButtonBounds().getHeight() + 2);
    g.fillRoundedRectangle(bounds.toFloat(), 2.f);

    auto area = getButtonBounds();
    if (processor.isDistortionActivate() == true)
    {
        g.drawImageWithin(on,
            area.getX(),
            area.getY(),
            area.getWidth(),
            area.getHeight(),
            RectanglePlacement::centred);
    }
    else
    {
        g.drawImageWithin(off,
            area.getX(),
            area.getY(),
            area.getWidth(),
            area.getHeight(),
            RectanglePlacement::centred);
    }
}

void ButtonComponent::resized()
{
    auto bounds = getLocalBounds();
    distortionButton.setBounds(bounds.getWidth() * 0.5, 750, 100, 100);

}

Rectangle<int> ButtonComponent::getButtonBounds()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(500);
    bounds.removeFromBottom(100);
    bounds.removeFromRight(100);
    bounds.removeFromLeft(600);

    return bounds;
}
