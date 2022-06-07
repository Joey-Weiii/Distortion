/*
  ==============================================================================

    KnobLook.cpp
    Created: 24 May 2022 7:49:40pm
    Author:  JOEY

  ==============================================================================
*/

#include "KnobLook.h"

StringArray KnobLook::getKnobFileNames()
{
    juce::StringArray names;

    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        names.add(juce::String::fromUTF8(BinaryData::namedResourceList[i]));
    return names;
}

void KnobLook::getImage(const juce::StringArray& names)
{
    int dataSize ;
    int knob_idx = 0;
   // str = String("ASDFSDF");
    for (int i = 0; i < names.size(); ++i)
    {
        const char* data = BinaryData::getNamedResource(names[i].toRawUTF8(), dataSize);
        //const char* data = BinaryData::getNamedResource(names[i].toRawUTF8(), dataSize); 
        if (data != nullptr && names[i][0] == 'm')
        {
            //str = String(knob_idx);
            temp = ImageCache::getFromMemory(BinaryData::metal_0_png, BinaryData::metal_0_pngSize);
            knobImage[knob_idx]=ImageCache::getFromMemory(data, dataSize);
            knob_idx++;
        }
    }

}

KnobLook::KnobLook(DistortionAudioProcessor& p):processor(p)
{
    StringArray filenames;
    filenames = getKnobFileNames();
    getImage(filenames);
}

KnobLook::~KnobLook()
{
}

int KnobLook::getGainInterval(float current_val)
{
    return int((current_val + 30) / 2);
}



void KnobLook::drawRotarySlider(Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle,Slider& slider)
{
    auto gainVal=0.0;
    
    if(knob_type==0)
    {
        gainVal = processor.getInputGainKnob();
    }
    else
    {
        gainVal = processor.getOutputGainKnob();
    }
    int knob_idx = getGainInterval(gainVal);
    auto knobArea = slider.getLocalBounds();
    //auto str =String(knob_idx);
    //g.drawFittedText(str, knobArea, juce::Justification::centred, 1);

    g.drawImageWithin(knobImage[knob_idx],
        knobArea.getX()+10,
        knobArea.getY(),
        knobArea.getWidth()-20,
        knobArea.getHeight()-20,
        RectanglePlacement::centred);
}


void KnobLook::set_as_in_knob()
{
    knob_type = 0;
}


void KnobLook::set_as_out_knob()
{
    knob_type = 1;
}