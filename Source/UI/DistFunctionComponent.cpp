/*
  ==============================================================================

    ButtonComponent.cpp
    Created: 6 May 2022 6:34:27pm
    Author:  JOEY

  ==============================================================================
*/
#include <JuceHeader.h>
#include <vector>
#include "DistFunctionComponent.h"

using namespace juce;

DistFunctionComponent::DistFunctionComponent(DistortionAudioProcessor& p):processor(p)
{
    func[0] = ImageCache::getFromMemory(BinaryData::no_png, BinaryData::no_pngSize);
    func[1] = ImageCache::getFromMemory(BinaryData::tanh_png, BinaryData::tanh_pngSize);
    func[2] = ImageCache::getFromMemory(BinaryData::soft_png, BinaryData::soft_pngSize);
    func[3] = ImageCache::getFromMemory(BinaryData::hard_png, BinaryData::hard_pngSize);

    addAndMakeVisible(func_tanh);
    func_tanh.setText("tanh", dontSendNotification);
    func_tanh.setFont(Font(14.0f, Font::bold));
    func_tanh.setJustificationType(Justification::centred);
    func_tanh.setColour(Label::textColourId, Colour(192, 245, 250));


    addAndMakeVisible(func_soft);
    func_soft.setText("soft", dontSendNotification);
    func_soft.setFont(Font(14.0f, Font::bold));
    func_soft.setJustificationType(Justification::centred);
    func_soft.setColour(Label::textColourId, Colour(192, 245, 250));

    addAndMakeVisible(func_hard);
    func_hard.setText("hard", dontSendNotification);
    func_hard.setFont(Font(14.0f, Font::bold));
    func_hard.setJustificationType(Justification::centred);
    func_hard.setColour(Label::textColourId, Colour(192, 245, 250));

    repaint();
}



//void DistFunctionComponent::buttonClicked(juce::Button* button)
//{
//
//}
DistFunctionComponent::~DistFunctionComponent()
{

}


void DistFunctionComponent::paint(Graphics&g)
{
    auto bounds = getLocalBounds();

    g.setColour(Colours::grey);
    //g.setColour(Colour((255, 255, 255, 0.6)));
    //g.fillAll(Colours::antiquewhite);
    auto functionArea = Rectangle<int>(bounds.getRight()-550 , bounds.getBottom()-350, 500, 300);
    
    g.fillRoundedRectangle(functionArea.toFloat(), 2.f);
    g.drawImageWithin(func[processor.getDistortionType()],
        functionArea.getX(),
        functionArea.getY(),
        functionArea.getWidth(),
        functionArea.getHeight(),
        RectanglePlacement::stretchToFit);

}

void DistFunctionComponent::resized()
{
    auto bounds = getLocalBounds();
    func_tanh.setBounds(Rectangle<int>(bounds.getRight()-50 , bounds.getBottom() - 110, 60, 30));
    func_soft.setBounds(Rectangle<int>(bounds.getRight()-50 , bounds.getBottom() - 160, 60, 30));
    func_hard.setBounds(Rectangle<int>(bounds.getRight()-50 , bounds.getBottom() - 210, 60, 30));

}

