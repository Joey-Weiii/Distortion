/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor& p,  AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts) , distFunctionComponent(processor), inputKnobLook(processor), outputKnobLook(processor)
{   

     // input gain
    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    inputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    inputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Input Gain", inputGainSlider));
    inputKnobLook.set_as_in_knob();
    inputGainSlider.setLookAndFeel(&inputKnobLook);


    addAndMakeVisible (inputGainLabel);
    inputGainLabel.setText ("Input Gain", dontSendNotification);
    inputGainLabel.setJustificationType (Justification::centred);

    
    // output gain
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    outputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    outputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Output Gain", outputGainSlider));
    outputKnobLook.set_as_out_knob();
    outputGainSlider.setLookAndFeel(&outputKnobLook);

    addAndMakeVisible (outputGainLabel);
    outputGainLabel.setText ("Output Gain", dontSendNotification);
    outputGainLabel.setJustificationType (Justification::centred); 

    // monitoring
    addAndMakeVisible (bufferTimeLabel);
    bufferTimeLabel.setText ("Buffer Time: ", dontSendNotification);

    // WaveViewer
    addAndMakeVisible(processor.waveViewer_in);
    processor.waveViewer_in.setColours(juce::Colours::black, Colour(192, 245, 250));

    addAndMakeVisible(processor.waveViewer_out);
    processor.waveViewer_out.setColours(juce::Colours::black, Colour(172, 181, 165));
    
    // SpectrumAnalyzer
    addAndMakeVisible(processor.analyzerComponent);

    //Button of 3 function
    addAndMakeVisible(tanhButton);
    tanhButton.addListener(this);
    tanhButton.setLookAndFeel(&tanhButtonLook);

    addAndMakeVisible(softButton);
    softButton.addListener(this);
    softButton.setLookAndFeel(&softButtonLook);

    addAndMakeVisible(hardButton);
    hardButton.addListener(this);
    hardButton.setLookAndFeel(&hardButtonLook);

    //FunctionComponent
    addAndMakeVisible(distFunctionComponent);

    setSize (1000, 1000);
    startTimerHz (10);

}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
    tanhButton.setLookAndFeel(nullptr);
    softButton.setLookAndFeel(nullptr);
    hardButton.setLookAndFeel(nullptr);
    inputGainSlider.setLookAndFeel(nullptr);
    outputGainSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    auto bounds = getLocalBounds();

    // Spectrum area
    auto spectrumArea = Rectangle<int>(0,0,1000,150);
    processor.analyzerComponent.setBounds(spectrumArea);
   
    //// Waveform area 
    auto waveform_in_Area = Rectangle<int>(0, 150, 1000, 150);
    processor.waveViewer_in.setBounds(waveform_in_Area);
    auto waveform_out_Area = Rectangle<int>(0, 300, 1000, 150);
    processor.waveViewer_out.setBounds(waveform_out_Area);

    // input gain
    auto inputGainArea =  Rectangle<int>(bounds.getRight()-1000, bounds.getBottom() - 200, 100, 100);
    auto inputLabelArea = Rectangle<int>(bounds.getRight() -1000, bounds.getBottom() -230, 100, 30);
    inputGainSlider.setBounds(inputGainArea);
    inputGainLabel.setBounds(inputLabelArea);
    
    // output gain
    auto outputGainArea = Rectangle<int>(bounds.getRight() - 900, bounds.getBottom() - 200, 100, 100);
    auto outputLabelArea = Rectangle<int>(bounds.getRight() -900, bounds.getBottom() - 230, 80, 30);
    outputGainSlider.setBounds(outputGainArea);
    outputGainLabel.setBounds(outputLabelArea);

    // --- monitor buffer time--- //
    auto buffetTimeArea = Rectangle<int>(bounds.getRight() - 120, bounds.getBottom()-100 , 100, 100);
    bufferTimeLabel.setBounds (buffetTimeArea);

    // tanh_btn
    auto tanhBtnArea = Rectangle<int>(bounds.getRight() - 120, bounds.getBottom() - 300, 100, 50);
    tanhButton.setBounds(tanhBtnArea);

    // soft_btn
    auto softBtnArea = Rectangle<int>(bounds.getRight() - 120, bounds.getBottom() - 250, 100, 50);
    softButton.setBounds(softBtnArea);

    // hard_btn
    auto hardBtnArea = Rectangle<int>(bounds.getRight() - 120, bounds.getBottom() - 200, 100, 50);
    hardButton.setBounds(hardBtnArea);

    // distortion functions
    auto functionArea = Rectangle<int>(bounds.getRight() - 750, bounds.getBottom() - 480, 600, 400);
    distFunctionComponent.setBounds(functionArea);

}

void DistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

}


void DistortionAudioProcessorEditor::timerCallback()
{
    float ave = 0.0;
    for(auto i=0; i<100; ++i)
    {
        ave += processor.bufferTimeRecords[i];
    }
    ave /= 100;
    bufferTimeLabel.setText("Buffer Time: " + std::to_string(ave), dontSendNotification);
    
}


void DistortionAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &tanhButton)
    {
        if (tanhButtonLook.getStage() == 0)
        {
            tanhButtonLook.setStage(1);
            softButtonLook.setStage(0);
            hardButtonLook.setStage(0);
            processor.setDistortionType(1);
        }
        else
        {
            tanhButtonLook.setStage(0);
            processor.setDistortionType(0);
        }
    }

    if (button == &softButton)
    {
        if (softButtonLook.getStage() == 0)
        {
            tanhButtonLook.setStage(0);
            softButtonLook.setStage(1);
            hardButtonLook.setStage(0);
            processor.setDistortionType(2);
        }
        else
        {
            softButtonLook.setStage(0);
            processor.setDistortionType(0);
        }
    }

    if (button == &hardButton)
    {
        if (hardButtonLook.getStage() == 0)
        {
            tanhButtonLook.setStage(0);
            softButtonLook.setStage(0);
            hardButtonLook.setStage(1);
            processor.setDistortionType(3);
        }
        else
        {
            hardButtonLook.setStage(0);
            processor.setDistortionType(0);
        }
    }
    processor.updateDistortionType();

    repaint();
}