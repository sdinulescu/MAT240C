#include "al/app/al_App.hpp"

using namespace al;

struct MyApp : App {
  void onSound(AudioIOData &io) override {
    while (io()) {
      //
    }
  }
};

int main() {
  MyApp app;
  app.start();
  return 0;
}
