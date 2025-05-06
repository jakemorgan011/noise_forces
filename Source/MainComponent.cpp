#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    for(size_t i = 0; i < NUM_BIRDS; i++){
        cfs.emplace_back();
        qs.emplace_back();
    }
    //initialize_birds();
    setSize (550, 200);
    
    nest.init(NUM_BIRDS);
    
    nt.set_nest(nest);
    nt.set_force(wind);
    nt.startTimer(25);
    
    addAndMakeVisible(nest);
    

    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    multi_biquad.prepareToPlay(sampleRate);
    for(size_t i = 0; i < NUM_BIRDS; i++){
        cfs[i].reset(sampleRate, 0.0001f);
        qs[i].reset(sampleRate,0.0001f);
    }
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    bufferToFill.buffer->clear();
    // initaliaze for buffer.
    for(int i = 0; i < NUM_BIRDS; i++){
        cfs[i].setCurrentAndTargetValue(freq_from_norm(nest.get_normalized_y(i)));
        qs[i].setCurrentAndTargetValue(nest.get_normalized_x(i));
    }
    for(auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel){
        auto* buffer = bufferToFill.buffer->getWritePointer(channel,bufferToFill.startSample);
        for(auto index = 0; index < bufferToFill.numSamples; ++index){
            sig_accumulator = 0;
            
            float rand = (juce::Random::getSystemRandom().nextFloat()-0.5)*2;
            
            
            if(index < 1){
                for(int i = 0; i < NUM_BIRDS; i++){
                    multi_biquad.make_bandpass(cfs[i].getCurrentValue(), 5, qs[i].getCurrentValue(), i);
                    sig_accumulator += (multi_biquad.filterSample(rand, i))/NUM_BIRDS;
                }
                buffer[index] = std::tanh(sig_accumulator/NUM_BIRDS); // tanh to keep it scoped...
            }else{
                for(int i = 0; i < NUM_BIRDS; i++){
                    multi_biquad.make_bandpass(cfs[i].getCurrentValue(), nest.movers[i].mass * 5, qs[i].getCurrentValue(), i);
                    sig_accumulator += (multi_biquad.filterSample(rand, i))/NUM_BIRDS;
                }
                buffer[index] = std::tanh(sig_accumulator/NUM_BIRDS);
            }
        }
    }
}

void MainComponent::releaseResources()
{
    sig_accumulator = 0;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.fillAll();
}

void MainComponent::resized()
{
    nest.setBounds(0,0,550,200);
}
