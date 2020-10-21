/* Stejara Dinulescu
 * Assignment 2: Create a simple audio program. The program should play the sound of a sawtooth wave and 
 * present and interface with two sliders, one for controlling frequency and another for controlling amplitude.
 * Due Oct 13 2020
*/

// This code is based off of in-class allolib example. Template was copy-pasted and then modified for the assignment.
#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_Parameter.hpp"

using namespace al;

struct MyApp : public App {
  Parameter frequency{"frequency", "", 440.0f}; // adjusts frequency of saw osc
  Parameter amplitude{"amplitude", "", 0.5}; // adjusts amplitude of saw osc
  gam::Saw<> saw; // creates saw oscillator, using Gamma -> default freq and phase params are 440 and 0, respectively (from Gamma Oscillator.h file)

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
      saw.freq(frequency); // adjust the freq of the saw oscillator based on frequency slider
      io.out(0) = saw() * amplitude; // adjust the amp of the saw oscillator based on amplitude slider
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