/*
  ==============================================================================

    SpectrumComponent.cpp
    Created: 22 Apr 2022 10:14:27pm
    Author:  JOEY

  ==============================================================================
*/

#include <JuceHeader.h>
#include <vector>

using namespace juce;

struct AnalyzerComponent : public Timer,
                           public juce::AudioAppComponent
{

    enum
    {
        fftOrder = 11,             // [1]
        fftSize = 1 << fftOrder,  // [2]
        scopeSize = 512             // [3]
    };

    AnalyzerComponent():
        forwardFFT(fftOrder),
        window(fftSize,juce::dsp::WindowingFunction<float>::hann)
    {
        setOpaque(true);
        setAudioChannels(1, 1); 
        startTimerHz(30);
    }


    ~AnalyzerComponent() override
    {
        shutdownAudio();
    }
    
    void prepareToPlay(int, double) override {}
    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (bufferToFill.buffer->getNumChannels() > 0)
        {
            auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

            for (auto i = 0; i < bufferToFill.numSamples; ++i)
                pushNextSampleIntoFifo(channelData[i]);
        }
    }

    void pushNextSampleIntoFifo(float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next frame should now be rendered..
        if (fifoIndex == fftSize)               
        {
            if (!nextFFTBlockReady)            
            {
                juce::zeromem(fftData, sizeof(fftData));
                memcpy(fftData, fifo, sizeof(fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }
        fifo[fifoIndex++] = sample;
    }


    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setOpacity(1.0f);
        g.setColour(juce::Colours::white);
        drawFrame(g);
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextFrameOfSpectrum();
            nextFFTBlockReady = false;
            repaint();
        }
    }

    void drawNextFrameOfSpectrum()
    {
        // first apply a windowing function to our data
        window.multiplyWithWindowingTable(fftData, fftSize);      

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform(fftData);  

        auto mindB = -100.0f;
        auto maxdB = 0.0f;

        for (int i = 0; i < scopeSize; ++i)                         
        {
            auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
            auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
            auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
                - juce::Decibels::gainToDecibels((float)fftSize)),
                mindB, maxdB, 0.0f, 1.0f);

            scopeData[i] = level;                                
        }
    }
    void drawFrame(juce::Graphics& g)
    {
        for (int i = 1; i < scopeSize; ++i)
        {
            auto width = getLocalBounds().getWidth();
            auto height = getLocalBounds().getHeight();

            g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                                  juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                          (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                                  juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
        }
    }

    juce::dsp::FFT forwardFFT;                      
    juce::dsp::WindowingFunction<float> window;   

    float fifo[fftSize];                           
    float fftData[2 * fftSize];                    
    int fifoIndex = 0;                              
    bool nextFFTBlockReady = false;                 
    float scopeData[scopeSize];                    

};
