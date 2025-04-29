/*
  ==============================================================================

    filters.h
    Created: 21 Mar 2025 5:21:44pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
namespace termite {
class biquad_filter {
public:
    
    biquad_filter(){}
    ~biquad_filter(){}
    
    void prepareToPlay(float sr){
        sampleRate = sr;
    }
    
    void makeLowShelf(float cutoffFrequency,float gainDB, float Q = 1.0f/std::sqrt(2.0f)){
        
        float A = std::pow(10.f, gainDB/40.f);
        
        float omega0 = 2.0f * M_PI * cutoffFrequency / sampleRate;
        float alpha = std::sin(omega0)/(2.0f*Q);
        
        float cosOmega0 = std::cos(omega0);
        float sqrtA = std::sqrt(A);
        
        b0 = A * ((A + 1)-(A-1) * cosOmega0 + 2 * sqrtA * alpha);
        b1 = 2 * A * ((A - 1) - (A + 1) * cosOmega0);
        b2 = A * ((A + 1) - (A - 1) * cosOmega0 - 2 * sqrtA * alpha);
        float a0 = (A + 1) + (A - 1) * cosOmega0 + 2 * sqrtA * alpha;
        a1 = -2 * ((A - 1) + (A + 1) * cosOmega0);
        a2 = (A + 1) + (A - 1) * cosOmega0 - 2 * sqrtA * alpha;
        
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
        
        z1 = 0.0f;
        z2 = 0.0f;
    }
    
    // haven't done this because i'm tired of reading documentation
    void makeLowPass(float cutoffFrequency, float gainDB, float Q = 1.0f/std::sqrt(2.0f)){
        
    }
    
    void makeHighShelf(float cutoffFrequency, float gainDB, float Q = 1.0f/std::sqrt(2.0f)){
        
    }
    
    void makeHighPass(float cutoffFrequency, float gainDB, float Q = 1.0f/std::sqrt(2.0f)){
        
    }
    
    void makeDiode(float cutoffFrequency, float gainDB, float Q = 1.0f/std::sqrt(2.0f)){
        
    }
    void makeBandPass(float cutoffFrequency, float gainDB, float Q){
        float A = std::pow(10.f, gainDB/40.f);//don't remember why this is in the past functions...
        float w0 = 2.f * M_PI * cutoffFrequency / sampleRate;
        float alpha = sin(w0) / (2.f * Q);
        float a0 = 1.f + alpha;
        
        b0 = alpha / a0;
        b1 = 0.f;
        b2 = -alpha/a0;
        a1 = (-2.f * cos(w0))/a0;
        a2 = (1.f - alpha)/a0;
    }
    
    float filterSample(float inSample){
        float output = b0 * inSample + b1 * z1 + b2 * z2 - a1 * z1 - a2 * z2;
        
        //history cascade
        z2 = z1;
        z1 = output;
        
        return output;
    }
private:
    float sampleRate = 44100.f;
    float b0,b1,b2;
    float a1,a2;
    float z1,z2;
};

class multi_biquad {
public:
    multi_biquad(){
        for(size_t i = 0; i<NUM_FILTERS; i++){
            biquads.emplace_back();
        }
    }
    ~multi_biquad(){}
    void prepareToPlay(float sr){
        sample_rate = sr;
        for(biquad_filter& bq : biquads){
            bq.prepareToPlay(sample_rate);
        }
    }
    void make_bandpass(float cf,float gainDB,float q, int index){
        biquads[index].makeBandPass(cf, gainDB, q);
    }
    // need a cool way to process samples.
    float filterSample(float inSample, int index){
        return biquads[index].filterSample(inSample);
    }
    
private:
    const size_t NUM_FILTERS = NUM_BIRDS;
    float sample_rate = 44100.f;
    std::vector<biquad_filter> biquads;
};


}
