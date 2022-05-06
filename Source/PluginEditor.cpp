/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor& p,  AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{   

     // input gain
    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    inputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    inputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Input Gain", inputGainSlider));

    addAndMakeVisible (inputGainLabel);
    inputGainLabel.setText ("Input Gain", dontSendNotification);
    inputGainLabel.setJustificationType (Justification::centred);
    
    // output gain
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    outputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    outputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Output Gain", outputGainSlider));

    addAndMakeVisible (outputGainLabel);
    outputGainLabel.setText ("Output Gain", dontSendNotification);
    outputGainLabel.setJustificationType (Justification::centred); 

    // monitoring
    addAndMakeVisible (bufferTimeLabel);
    bufferTimeLabel.setText ("Buffer Time: ", dontSendNotification);

    // WaveViewer
    addAndMakeVisible(processor.waveViewer);
    processor.waveViewer.setColours(juce::Colours::black, juce::Colours::whitesmoke.withAlpha(0.5f));
    
    // SpectrumAnalyzer
    addAndMakeVisible(processor.analyzerComponent);



    setSize (1000, 1000);
    startTimerHz (10);

}

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    auto bounds = getLocalBounds();

    // Spectrum area
    auto spectrumArea = bounds.removeFromTop(bounds.getHeight() * 0.2);
    processor.analyzerComponent.setBounds(spectrumArea);
   
    // Waveform area 
    auto waveformArea = bounds.removeFromTop(bounds.getHeight() * 0.4);
    processor.waveViewer.setBounds(waveformArea);

    // Gains Area
    auto inputGainArea = bounds.removeFromLeft(bounds.getWidth()*0.33);
    auto outputGainArea = bounds.removeFromLeft(bounds.getWidth()*0.33);
    
     // input gain
    inputGainSlider.setBounds(bounds.getWidth()*0.15,600,150,150);
    inputGainLabel.setBounds(bounds.getWidth()*0.15, 570, 80, 30);
    
    // output gain
    outputGainSlider.setBounds(bounds.getWidth() * 0.15, 800, 150, 150);
    outputGainLabel.setBounds(bounds.getWidth() * 0.15, 770, 80, 30);

    // --- monitor buffer time--- //
    auto buffetTimeArea = bounds.removeFromRight(bounds.getWidth() * 0.35);
    bufferTimeLabel.setBounds (buffetTimeArea.removeFromTop(bounds.getHeight() * 0.75));

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
