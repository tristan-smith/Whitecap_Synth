#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "MIDI_CONVERSION.h"

// -----------------------------------------------------------------------------

// Receives MIDI input and reads analog inputs, generates sound accordingly.

// -----------------------------------------------------------------------------

// GUItool: begin automatically generated code
AudioSynthWaveformSine waveform1;      //xy=755.5714073181152,247.71431064605713
AudioOutputAnalog dac;           //xy=911,250
AudioConnection patchCord1(waveform1, dac);
// GUItool: end automatically generated code

// indicator stuff
#define LED 13                   // LED pin on Teensy/Arduino Uno
const int BLINK_TIME = 50;
int lastLED = 0;

// ADSR stuff
const int ATTACK = 0;
const int DECAY = 1;
const int SUSTAIN = 2;
const int RELEASE = 3;

// times in ms, to be read from analog in
float attackTime = 1000;
float decayTime = 100;
float sustainLevel = 0.75;
float releaseTime = 100;

// envelope 1
int envStage_1 = ATTACK;
float envLevel_1 = 0.0;
unsigned long envStartTime_1;

// timing stuff
unsigned long previousTime = 0;
unsigned long currentTime = 0;

// -----------------------------------------------------------------------------

void setup()
{
    pinMode(LED, OUTPUT);

    // MIDI Setup
    MIDI.begin(MIDI_CHANNEL_OMNI);           // Launch MIDI, default listening to channel 1.
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);

    // Audio Setup
    AudioMemory(10);
    
    // Done
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
}

void loop()
{
  previousTime = currentTime;
  currentTime = micros();
  
  // Monitor incoming MIDI
  MIDI.read();

  ADSR(currentTime - previousTime, envStage_1);

  // this is so that the ADSR doesn't trip up and multiply by 0.
  // better fix incoming?
  delay(1);
}

// callback for when a MIDI note on message is received
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  envStage_1 = ATTACK;
  envStartTime_1 = micros();

  waveform1.frequency(midi_to_freq[pitch]);

  digitalWrite(LED, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  envStage_1 = RELEASE;

  digitalWrite(LED, LOW);
}

void ADSR(unsigned long timeElapsed, int envelopeStage) {
  if (envelopeStage == ATTACK) {
    // increment attack, in milliseconds
    envLevel_1 += (1 / (attackTime)) * (timeElapsed / 1000);


  } else if (envelopeStage == DECAY) {
    // increment decay
    
  } else if (envelopeStage == SUSTAIN) {
    // set volume to sustain level

  } else if (envelopeStage == RELEASE) {
    envLevel_1 = 0;
  }
  waveform1.amplitude(envLevel_1);
}



