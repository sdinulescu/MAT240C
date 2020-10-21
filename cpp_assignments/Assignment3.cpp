/* Stejara Dinulescu
 * Assignment 2: Create a simple audio program. The program should play the sound of a sawtooth wave and 
 * present and interface with two sliders, one for controlling frequency and another for controlling amplitude.
 * Assignment 3: 
 * Part 2 due Oct 13 2020, Part 3 due Oct 22, 2020
*/

// This code is based off of in-class allolib example. Template was copy-pasted and then modified for the assignment.
#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_Parameter.hpp"
//#include <cmath>
#include <iostream>

using namespace al;

# define PI 3.14159  /* pi */

struct MyApp : public App {
  Parameter frequency{"frequency", "", 440.0f}; // adjusts frequency of saw osc
  Parameter amplitude{"amplitude", "", 0.5}; // adjusts amplitude of saw osc
  //gam::Saw<> saw; // creates saw oscillator, using Gamma -> default freq and phase params are 440 and 0, respectively (from Gamma Oscillator.h file)

  struct BLSaw {
    //pseudocode variables and constants
    float osc; // output of the saw oscillator
    float osc2; // output of the saw oscillator 2
    float phase; // phase accumulator
    float w; // normalized frequency
    float scaling; // scaling amount
    float DC; // DC compensation
    //float *output; // pointer to array of floats
    float pw; // pulse width of the pulse, 0..1
    float norm; // normalization amount
    float const a0 = 2.5f; // precalculated coeffs
    float const a1 = -1.5f; // for HF compensation
    float in_hist; // delay for the HF filter

    float vf = 0.5; //virtual filter to get rid of nyquist ringing -> added from class 10.21
    
    void set(float freq, float samplerate) { // calculate w and scaling
      w = freq/samplerate; // normalized frequency
      float n = 0.5f-w;
      scaling = vf * 13.0f * n*n*n*n; // calculate scaling -> add vf here
      DC = 0.376f - w*0.752f; // calculate DC compensation
      osc = 0.f; phase = 0.f; // reset oscillator and phase
      norm = 1.0f - 2.0f*w; // calculate normalization
    }
    
    // process loop for creating a bandlimited saw wave
    float operator()() { 
        // increment accumulator
        phase += 2.0f*w; 
        //wrap
        if (phase >= 1.0f) phase -= 2.0f;
        // calculate next sample
        osc = (osc + sin(2*PI*(phase + osc*scaling)))*0.5f;
        // compensate HF rolloff
        float out = a0*osc + a1*in_hist; 
        in_hist = osc;
        out = out + DC; // compensate DC offset
        //return next sample value
        return out*norm;
        //*output++ = out*norm; // store normalized result
    }
    
    // // process loop for creating a bandlimited PWM pulse
    // void bandlimitedPulse() {
    //   //while(--sampleFrames >= 0){
    //     // increment accumulator
    //     phase += 2.0f*w; 
    //     if (phase >= 1.0f) phase -= 2.0f;
    //     // calculate saw1
    //     osc = (osc + sin(2*PI*(phase + osc*scaling)))*0.5f;
    //     // calculate saw2
    //     osc2 = (osc2 + sin(2*PI*(phase + osc2*scaling + pw)))*0.5f;
    //     float out = osc-osc2; // subtract two saw waves
    //     // compensate HF rolloff
    //     out = a0*out + a1*in_hist; 
    //     in_hist = out;
    //     *output++ = out*norm; // store normalized result
    //   //}
    // }
  };
  
  BLSaw saw;

  void onCreate() override {
    imguiInit();
    // Set min and max for parameters
    frequency.min(0.0);
    frequency.max(1000.0);
    amplitude.min(0.0);
    amplitude.max(1.0);
  }

  void onAnimate(double dt) override {
    imguiBeginFrame();
    ParameterGUI::beginPanel("Audio");
    ParameterGUI::drawParameterMeta(&frequency);
    ParameterGUI::drawParameterMeta(&amplitude);
    ParameterGUI::drawAudioIO(audioIO());
    ParameterGUI::endPanel();
    imguiEndFrame();
  }

  void onDraw(Graphics &g) override {
    g.clear(0.1);
    imguiDraw();
  }

  void onSound(AudioIOData &io) override {
    while (io()) {
      saw.set(frequency, 44100); // adjust the freq of the saw oscillator based on frequency slider
      float f = saw() * amplitude;
      //std::cout << f << std::endl;
      io.out(0) = f;  // adjust the amp of the saw oscillator based on amplitude slider
      io.out(1) = f;
    }
  }

  void onExit() override { imguiShutdown(); }
};

int main(int argc, char *argv[]) {
  MyApp app; // initialize app

  // Configure audio settings
  AudioDevice dev = AudioDevice::defaultOutput();
  float sampleRate = 44100;
  int bufferSize = 256;
  int numInputChannels = 2;
  int numOutputChannels = 2;
  app.configureAudio(dev, sampleRate, bufferSize, numOutputChannels,
                     numInputChannels);

  app.start(); //start app

  return 0;
}