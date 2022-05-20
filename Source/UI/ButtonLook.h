#pragma once
#include "JuceHeader.h"
#include "../PluginProcessor.h"

using namespace juce;

/*
=================================================================================
ButtonLook
=================================================================================
*/

class ButtonLook : public LookAndFeel_V4
{
public:
    ButtonLook();
    ~ButtonLook();

    void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
        bool, bool isButtonDown) override;

    void setStage(int n);
    int getStage();

private:
    int stage = 0;
    Image btn_on, btn_off;
};
