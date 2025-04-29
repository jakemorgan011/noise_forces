/*
  ==============================================================================

    flockingLF.h
    Created: 26 Mar 2025 1:42:54pm
    Author:  Jake Morgan

  ==============================================================================
*/

// btw using a different tree structure and xml writer so that i can learn data structures for real.

#pragma once
#include <JuceHeader.h>


const size_t MAX_NUM_FORCES = 4;
const size_t NUM_BIRDS = 25;
const size_t MAX_SPEED = 8;


class customLookAndFeel : public juce::LookAndFeel_V4 {
public:
    
private:
    
};

// lets do this flocking with custom memory handling instead of std::vector<>
namespace termite {
struct vectorXY {
    float x,y;
    vectorXY(float x, float y) : x(x), y(y){}
    
    // i <3 operator funcions. they are so silly
    vectorXY operator+(const vectorXY& other)const{
        return vectorXY{x+other.x,y+other.y};
    }
    vectorXY operator*(const vectorXY& other)const{
        return vectorXY{x*other.x,y*other.y};
    }
    vectorXY operator-(const vectorXY& other)const{
        return vectorXY{x-other.x,y-other.y};
    }
    vectorXY operator/(const vectorXY& other)const{
        return vectorXY{x/other.x,y/other.y};
    }
    // ignore that there's only two of these.
    vectorXY operator/(const float& val)const{
        return vectorXY{x/val,y/val};
    }
    vectorXY operator*(const float& val)const{
        return vectorXY{x*val,y*val};
    }
};
struct mover {
    vectorXY pos{0,0};
    vectorXY vel{0,0};
    vectorXY accel{0,0};
    vectorXY drag{0,0};
    const float mass = 1;
    
};
// =================================
//
//
//        HOPEFULLY THIS WORKS
//
//
// =================================
class nest : public juce::Component {
public:
    // public cause mass needs to be accessed for gain;
    std::vector<mover> movers;
    
    void init(size_t num_movers){
        for(size_t i = 0; i < num_movers; i++){
            auto rand_val = juce::Random::getSystemRandom().nextInt(juce::Range<int>(-4,4));
            auto rand_scale = juce::Random::getSystemRandom().nextDouble();
            float rand_y = rand_val * rand_scale;
            float rand_m_scale = juce::Random::getSystemRandom().nextDouble();
            float rand_mass = (juce::Random::getSystemRandom().nextInt(juce::Range<int>(2,4)) * rand_m_scale);
            auto rand_x = juce::Random::getSystemRandom().nextInt(juce::Range<int>(0,400));
            movers.emplace_back(mover{{static_cast<float>(rand_x),100},{0,rand_y},{0,0},{0,0},rand_mass});
        }
        initialized = true;
    }
    void paint(juce::Graphics& g){
        if(initialized){
            for(mover& m : movers){
                g.setColour(shadow);
                g.fillEllipse(m.pos.x-((getWidth()-m.pos.x)/40),m.pos.y+2, w*(m.mass/1.85), h*(m.mass/1.85));
                g.setColour(bird_color);
                g.fillEllipse(m.pos.x, m.pos.y, w*(m.mass/2), h*(m.mass/2));
                g.setColour(juce::Colours::white);
                g.drawEllipse(m.pos.x,m.pos.y, w*(m.mass/2),h*(m.mass/2),5);
            }
        }
        if(!initialized){}
    }
    void apply_force(vectorXY force){
        if(initialized){
            for(mover& m : movers){
                force = force / m.mass;
                m.accel = m.accel + force;
            }
        }
        if(!initialized){}
    }
    void update(){
        if(initialized){
            for(mover& m : movers){
                m.vel = m.vel + m.accel;
                //m.vel = m.vel * m.drag;
                if(m.vel.x > MAX_SPEED){
                    m.vel.x = 8;
                }
                m.pos = m.pos + m.vel;
                
                m.accel = m.accel * 0;
                edges(m);
                repaint();
            }
        }
        if(!initialized){}
    }
    void edges(mover& m){
        auto bounds = getLocalBounds();
        int width = bounds.getWidth();
        int height = bounds.getHeight();
        if(m.pos.x > width){
            m.pos.x = 0;
        }
        if(m.pos.x < 0){
            m.pos.x = 0;
            m.vel.x *= -1;
        }
        if(m.pos.y > height){
            m.pos.y = height;
            m.vel.y *= -1;
            m.vel.y *= y_dampen;
        }
        if(m.pos.y < 0){
            m.pos.y = 0;
            m.vel.y *= -1;
            m.vel.y *= y_dampen;
        }
    }
    std::vector<mover>* get_birds(){
        return &movers;
    }
    float get_normalized_y(int index){
        auto height = movers[index].pos.y;
        auto output = height/getHeight();
        return (output*2);
    }
    float get_normalized_x(int index){
        auto width = movers[index].pos.x;
        return (width/getWidth())+2;
    }
    
private:
    juce::Colour shadow = juce::Colour(1006634260);
    juce::Colour bird_color = juce::Colour(255,110,145);
    const int w = 50;
    const int h = 50;
    bool initialized = false;
    const float y_dampen = 1;
};

class nest_timer : public juce::Timer {
    public:
    void set_nest(nest& n){
        nest = &n;
    }
    void set_force(vectorXY& force){
        f = &force;
    }
    void timerCallback() override {
        nest->apply_force(*f);
        nest->update();
    }
    private:
    nest* nest;
    vectorXY* f;
};


// =================================================

struct dropper_data {
    
};
class mouse_force : public juce::Component {
    
};
class mouse_dropper : public juce::Component {
    
};
// for the birds just use std::vector<> because that really is the best way to do it.
// I DON'T HAVE ENOUGH TIME TO DESIGN A COOL BIRD BUT THE CIRCLE WILL DO FOR NOW
class bird : public juce::Component {
public:
    void set_params(float xPos, float yPos, int width, int height, float m){
        pos = {xPos,yPos};
        width_px = width;
        height_px = height;
        mass = m;
    }
    void paint (juce::Graphics& g) override {
        auto radius = (float) juce::jmin (width_px / 2, height_px / 2) - 4.0f;
        auto centre = (float) pos.x + (float) width_px * 0.5f; // centre because its a perfect circle
        auto rw = radius * 2.0f;
        g.setColour(bird_color);
        g.fillEllipse(pos.x,pos.y,rw,rw);
    }
    void apply_force(vectorXY force){
        auto new_f = force / 0.85f; // hell yea dawg operator function.
        accel = accel + new_f; // ahhh shit transcending types.
    }
    void update(){
        vel = vel + accel;
        vel = vel * drag; // should sorta cap the velocity.
        // wind will always be on so don't need to worry about turning off drag
        pos = pos + vel;
        accel.x = 0; // re-init so that force always needs to be applied
        accel.y = 0;
        edges();
        repaint();
    }
    void edges(){
        auto bounds = getLocalBounds();
        int width = bounds.getWidth();
        int height = bounds.getHeight();
        if(pos.x > width){
            pos.x = 0;
        }
        if(pos.x < 0){
            pos.x = 0;
            vel.x *= -1;
        }
        if(pos.y > height){
            pos.y = height;
            vel.y *= -1;
            vel.y *= y_dampen;
        }
        if(pos.y < 0){
            pos.y = 0;
            vel.y *= -1;
            vel.y *= y_dampen;
        }
    }
    vectorXY get_pos(){
        return pos;
    }
private:
    juce::Colour bird_color = juce::Colour(56,97,140); // navy
    vectorXY pos{0,0};
    vectorXY accel{0,0};
    vectorXY vel{0,0};
    const vectorXY drag {-0.021,0}; // drag needs to be a derivative of vel technically but I don't have enough time to get that working.
    int width_px = 50;
    int height_px = 50;
    float mass = 1;
    const float y_dampen = 0.065;
};
}

class bird_timer : public juce::Timer {
    public:
    void set_birds(std::vector<std::unique_ptr<termite::bird>>* b){
        birds = b;
    }
    void set_force(termite::vectorXY force){
        f = force;
    }
    void timerCallback() override{
        for(std::unique_ptr<termite::bird>& bird : *birds){ // shared_ptr can be de-referenced via *
            bird->apply_force(f); // this is so ass if it works...
            bird->update();
        }
    }
    private:
    std::vector<std::unique_ptr<termite::bird>>* birds;
    termite::vectorXY f{0,0};
};

class test_timer : public juce::Timer {
    public:
    void set_birds(termite::bird* b){
        bird = b;
    }
    void set_force(termite::vectorXY force){
        f = force;
    }
    void timerCallback() override{
        bird->apply_force(f);
        bird->update();
    }
    private:
    termite::bird* bird;
    termite::vectorXY f{0,0};
};
