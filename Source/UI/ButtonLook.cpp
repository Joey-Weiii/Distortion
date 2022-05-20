/*
  ==============================================================================

    ButtonLook.cpp
    Created: 13 May 2022 4:08:35pm
    Author:  JOEY

  ==============================================================================
*/

#include "ButtonLook.h"
/*
=================================================================================
ButtonLook
=================================================================================
*/

ButtonLook::ButtonLook()
{
    btn_on = ImageCache::getFromMemory(BinaryData::btn_on_png, BinaryData::btn_on_pngSize);
    btn_off = ImageCache::getFromMemory(BinaryData::btn_off_png, BinaryData::btn_off_pngSize);
}


ButtonLook::~ButtonLook()
{
}

//==============================================================================
void ButtonLook::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
    bool, bool isButtonDown)
{
    auto buttonArea = button.getLocalBounds();
    if (stage == 0) 
    {
        g.drawImageWithin(btn_off,
            buttonArea.getX(),
            buttonArea.getY(),
            buttonArea.getWidth(),
            buttonArea.getHeight(),
            RectanglePlacement::centred);
    }
    else
    {
        g.drawImageWithin(btn_on,
            buttonArea.getX(),
            buttonArea.getY(),
            buttonArea.getWidth(),
            buttonArea.getHeight(),
            RectanglePlacement::centred);
    }
    


}

//==============================================================================
void ButtonLook::setStage(int n)
{
    stage = n;
}

//==============================================================================
int ButtonLook::getStage()
{
    return stage;
}