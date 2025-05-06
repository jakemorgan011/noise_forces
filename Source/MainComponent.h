#pragma once

#include <JuceHeader.h>
#include "flockingLF.h"
#include "filters.h"

class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    termite::nest nest;
    termite::nest_timer nt;
    termite::vectorXY wind{0.5,0};
    termite::multi_biquad multi_biquad;
    std::vector<juce::LinearSmoothedValue<float>> cfs;
    std::vector<juce::LinearSmoothedValue<float>> qs;
    
    float last_rand =0.f;
    
    float sig_accumulator = 0;
    
    float freq_from_norm(float in_norm){
        return juce::jlimit(100.f, 10000.f, 10 * powf(2200.f, in_norm)); // 10000 because uhhhh
    }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
