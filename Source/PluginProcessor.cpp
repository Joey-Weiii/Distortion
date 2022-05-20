/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

using namespace juce;
//==============================================================================
DistortionAudioProcessor::DistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif

parameters(*this, nullptr, juce::Identifier("APVT"),
       {
           
           std::make_unique<AudioParameterFloat> (
                   "Input Gain",      // parameter ID
                   "Input Gain",      // parameter name
                   NormalisableRange<float>(
                       -30.0f,
                       30.0f,
                       0.01f),         // min, max,step
                    0.0f),
           std::make_unique<AudioParameterFloat> (
                   "Output Gain",      // parameter ID
                   "Output Gain",      // parameter name
                   NormalisableRange<float>(
                       -30.0f,
                       30.0f,
                       0.01f),         // min, max,step
                    0.0f),        // default value
       }),waveViewer_in(1), waveViewer_out(1),analyzerComponent()

{
    // knob values
    inputGainParameter  = parameters.getRawParameterValue ("Input Gain");
    outputGainParameter = parameters.getRawParameterValue ("Output Gain");
    
    // gains
    previousOutputGain = *outputGainParameter;
    previousInputGain  = *inputGainParameter ;

    // WaveViewer
    waveViewer_in.setRepaintRate(30);
    waveViewer_in.setBufferSize(256);
    waveViewer_out.setRepaintRate(30);
    waveViewer_out.setBufferSize(256);

    //distortion function
    distortion.functionToUse = [](float x) { return x; };
}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//==============================================================================
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int numSamples)
{
    // set host sampling env
    numInputChannels = getTotalNumInputChannels();
    numOutputChannels = getTotalNumOutputChannels();
    hostSampleRate = sampleRate;
    previousNumSamples = numSamples;
    bufferTimeFull = numSamples / sampleRate;
     
    std::cout << "========================" << std::endl;
    std::cout << "            sampleRate: " << sampleRate << std::endl;
    std::cout << "       samplesPerBlock: " << numSamples << std::endl;
    std::cout << " totalNumInputChannels: " << numInputChannels << std::endl;
    std::cout << "========================" << std::endl;
    // set info for analyzer
    
    info.numSamples = numSamples;
    info.startSample = 0;


    // spec for filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = hostSampleRate;
    spec.maximumBlockSize = numSamples;
    spec.numChannels = numInputChannels;
    
    std::cout << " > Filter Size: " << coeff.size() << std::endl;
    
    // -- Method 1 - Use Convolution -- //
    
    coeffBuffer = AudioBuffer<float>(1, (int) coeff.size());     /* Initialize the buffer */
    coeffBuffer.copyFrom(0, 0, coeff.data(), (int) coeff.size()); /* Vector to buffer */
    // coeffBuffer.reverse(0, 0, convInSize); /* Call this line when load params from pytorch */

    myfilter.reset(); /* Resets the processing pipeline ready to start a new stream of data */
    myfilter.loadImpulseResponse( /* Load coeff as IR */
               std::move (coeffBuffer),
               spec.sampleRate,
               dsp::Convolution::Stereo::yes,
               dsp::Convolution::Trim::no,
               dsp::Convolution::Normalise::no);
     myfilter.prepare(spec); /* Must be called before first calling process */
     
    // -- clear caches -- //
    // clear gain cache
    previousOutputGain = *outputGainParameter + 0.0;
    previousInputGain  = *inputGainParameter  + 0.0;
    
    // distortion function
    distortion.prepare(spec);

    // clear bufferTimeRecords
    memset(bufferTimeRecords, 0, sizeof(bufferTimeRecords));
    recordIndex = 0;

}

void DistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    auto begin = std::chrono::high_resolution_clock::now();


    // Main DSP procssing block
    if (numSamples > 0)
    {
        // We need reseting here beacuse DAWs usually sent testing signal after preparetoplay()
        if (previousNumSamples != numSamples)
        {
            prepareToPlay(hostSampleRate, numSamples); 
        }
        
        // ===== Start Coding ===== //
        // some DSP stuff

        waveViewer_in.pushBuffer(buffer);
        ApplyInputGain(buffer);
        //distortion funciton
        juce::dsp::AudioBlock<float> block(buffer);
        dsp::ProcessContextReplacing<float> context(block);
        distortion.process(context);

        //juce::dsp::ProcessContextReplacing<float> context(block);
        //myfilter.process(context);

        info.buffer = &buffer;
        analyzerComponent.getNextAudioBlock(info);

        ApplyOutputGain(buffer);
        waveViewer_out.pushBuffer(buffer);
        // ===== End Coding ===== //
        
    }
    else
    {
        return; //avoid exception when DAW sents zero length audio buffer
    }
    
    // update RTF
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() / 1e9;
    float bufferTime = duration / bufferTimeFull;
    updateRecord(bufferTime);
}

//==============================================================================


float DistortionAudioProcessor::getInputGain()
{
    return powf(10, (*inputGainParameter) / 20.0);
}

float DistortionAudioProcessor::getOutputGain()
{
    return powf(10, (*outputGainParameter) / 20.0);
}


void DistortionAudioProcessor::ApplyInputGain(AudioBuffer<float>& buffer)
{
    auto currentInputGain = getInputGain();
    if (currentInputGain == previousInputGain)
    {
        buffer.applyGain (currentInputGain);
    }
    else
    {
        auto numSamples = buffer.getNumSamples();
        buffer.applyGainRamp (0, numSamples, previousInputGain, currentInputGain);
        previousInputGain = currentInputGain;
    }
}

void DistortionAudioProcessor::ApplyOutputGain(AudioBuffer<float>& buffer)
{
   
    auto currentOutputGain = getOutputGain();
    if (currentOutputGain == previousOutputGain)
    {
        buffer.applyGain (currentOutputGain);
    }
    else
    {
        auto numSamples = buffer.getNumSamples();
        buffer.applyGainRamp (0, numSamples, previousOutputGain, currentOutputGain);
        previousOutputGain = currentOutputGain;
    }
}

//==============================================================================
void DistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}



//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}

juce::AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    return new DistortionAudioProcessorEditor (*this,parameters);
}

bool DistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
      // Ensure # input channel == #output channel
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    return true;
}


//==============================================================================
void DistortionAudioProcessor::updateRecord(float newValue)
{
    bufferTimeRecords[recordIndex] = newValue;
    ++recordIndex;
    recordIndex %= 100;
}


// Distortion functions
int DistortionAudioProcessor::getDistortionType()
{
    return distortionType;
}


void DistortionAudioProcessor::setDistortionType(int x)
{
    distortionType = x;
}


void DistortionAudioProcessor::updateDistortionType()
{
    // 0 : no distortion
    if(distortionType==0)
    {
        distortion.functionToUse = [](float x) { return x; };
    }
    // 1 : tanh
    else if (distortionType == 1)
    {
        distortion.functionToUse = [](float x) { return tanh(x); };
    }
    // 2 : soft clipping
    else if (distortionType == 2)
    {
        distortion.functionToUse = [](float x) 
        { 
            int sign = (x > 0) ? 1 : -1;
            x= sign * (1 - exp(-1 * abs(x)));
            return x; 
        };
    }
    // 3 : hard clipping
    else if (distortionType == 3)
    {
        distortion.functionToUse = [](float x) { return jlimit<float>(-0.7f, 0.7f, x); };
    }
}