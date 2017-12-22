#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "MIDI_CONVERSION.h"


// -----------------------------------------------------------------------------

// MIDI controlled synthesizer with delay and filter.

// -----------------------------------------------------------------------------

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=94.00001907348633,417.00005865097046
AudioSynthWaveform       voice3; //xy=100.00002670288086,292.0000400543213
AudioSynthWaveform       voice1; //xy=101.00000762939453,194.0000295639038
AudioSynthWaveform       voice2; //xy=101.00002670288086,243.00003910064697
AudioSynthWaveform       voice4; //xy=101.00000381469727,350.99999713897705
AudioEffectEnvelope      envelope3; //xy=264.00000381469727,301.99999713897705
AudioEffectEnvelope      envelope4; //xy=264.00000381469727,350.99999713897705
AudioEffectEnvelope      envelope2; //xy=265.0000114440918,249.00000476837158
AudioEffectEnvelope      envelope5; //xy=265.00000381469727,414.99999713897705
AudioEffectEnvelope      envelope1;      //xy=267.00001525878906,198.00000667572021
AudioMixer4              mixer1;         //xy=435.00001525878906,251.00000762939453
AudioMixer4              mixer2; //xy=576.555534362793,273.5555896759033
AudioSynthWaveform       filterLFO;      //xy=613.9444885253906,453.52782249450684
AudioFilterStateVariable filter1;        //xy=724.9444732666016,280.5278205871582
AudioOutputAnalog        dac1;           //xy=854.0000457763672,275.1667547225952
AudioConnection          patchCord1(noise1, envelope5);
AudioConnection          patchCord2(voice3, envelope3);
AudioConnection          patchCord3(voice1, envelope1);
AudioConnection          patchCord4(voice2, envelope2);
AudioConnection          patchCord5(voice4, envelope4);
AudioConnection          patchCord6(envelope3, 0, mixer1, 2);
AudioConnection          patchCord7(envelope4, 0, mixer1, 3);
AudioConnection          patchCord8(envelope2, 0, mixer1, 1);
AudioConnection          patchCord9(envelope5, 0, mixer2, 1);
AudioConnection          patchCord10(envelope1, 0, mixer1, 0);
AudioConnection          patchCord11(mixer1, 0, mixer2, 0);
AudioConnection          patchCord12(mixer2, 0, filter1, 0);
// AudioConnection          patchCord13(filterLFO, 0, filter1, 1);
AudioConnection          patchCord14(filter1, 0, dac1, 0);
// GUItool: end automatically generated code


// indicator stuff
#define LED 13                   // LED pin on Teensy/Arduino Uno
const int BLINK_TIME = 50;
int lastLED = 0;

// times in ms, to be read from analog in
float attackTime = 1;
float holdTime = 0;
float decayTime = 250;
float sustainLevel = 0.25;
float releaseTime = 1000000;

// timing stuff
unsigned long previousTime = 0;
unsigned long currentTime = 0;

// polyphony stuff
int currentVoice = 0;
AudioSynthWaveform *voices[4] = {&voice1, &voice2, &voice3, &voice4};
AudioEffectEnvelope *envelopes[4] = {&envelope1, &envelope2, &envelope3, &envelope4};
int voicePitches[4];

// -----------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
    
    pinMode(LED, OUTPUT);

    // MIDI Setup
    MIDI.begin(MIDI_CHANNEL_OMNI);           // Launch MIDI, default listening to channel 1.
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);

    // Audio Setup
    AudioMemory(150);

    mixer1.gain(0, 0.25);
    mixer1.gain(1, 0.25);
    mixer1.gain(2, 0.25);
    mixer1.gain(3, 0.25);

    mixer2.gain(0, 0.7);
    mixer2.gain(1, 0.7);

    filter1.frequency(1000);

    // ADSR Setup
    for(int i = 0; i <= 3; i++) {
      voices[i]->begin(1.0, 440.0, WAVEFORM_SINE);
      envelopes[i]->delay(0);
      envelopes[i]->attack(attackTime);
      envelopes[i]->hold(holdTime);
      envelopes[i]->decay(decayTime);
      envelopes[i]->sustain(sustainLevel);
      envelopes[i]->release(releaseTime);
    }
    
    // Done
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
}

void loop()
{
  // Monitor incoming MIDI
  MIDI.read();
}

// callback for when a MIDI note on message is received
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  voicePitches[currentVoice] = pitch;
  voices[currentVoice]->frequency(midi_to_freq[pitch]);
  envelopes[currentVoice]->noteOn();

  currentVoice++;
  
  if (currentVoice >= 4) {
    currentVoice = 0;
  }
  digitalWrite(LED, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  // get index of voice to start release
  for (int i = 0; i <= 3; i++) {
    if (voicePitches[i] == pitch) {
      envelopes[i]->noteOff();
    }
  }
  // didn't find the right pitch, nothing to turn off?
  digitalWrite(LED, LOW);
}


