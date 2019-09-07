/*
     C     C#    D     Eb    E     F     F#    G     G#    A     Bb    B
 0 16.35 17.32 18.35 19.45 20.60 21.83 23.12 24.50 25.96 27.50 29.14 30.87
 1 32.70 34.65 36.71 38.89 41.20 43.65 46.25 49.00 51.91 55.00 58.27 61.74
 2 65.41 69.30 73.42 77.78 82.41 87.31 92.50 98.00 103.8 110.0 116.5 123.5
 3 130.8 138.6 146.8 155.6 164.8 174.6 185.0 196.0 207.7 220.0 233.1 246.9
 4 261.6 277.2 293.7 311.1 329.6 349.2 370.0 392.0 415.3 440.0 466.2 493.9
 5 523.3 554.4 587.3 622.3 659.3 698.5 740.0 784.0 830.6 880.0 932.3 987.8
 6 1047  1109  1175  1245  1319  1397  1480  1568  1661  1760  1865  1976
 7 2093  2217  2349  2489  2637  2794  2960  3136  3322  3520  3729  3951
 8 4186  4435  4699  4978  5274  5588  5920  6272  6645  7040  7459  7902
 
 Guitar strings are E2=82.41Hz, A2=110Hz, D3=146.8Hz, G3=196Hz, B3=246.9Hz, E4=329.6Hz
 
 Bass strings are (5th string) B0=30.87Hz, (4th string) E1=41.20Hz, A1=55Hz, D2=73.42Hz, G2=98Hz
 
 This example tests the yin algorithm with actual notes from nylon string guitar recorded
 as wav format at 16B @ 44100 samples/sec. Since the decay of the notes will be longer than what
 the teensy can store in flash these notes are truncated to ~120,000B or about 1/2 of the whole
 signal.
 */
#include <SerialFlash.h>
#include <AudioTuner.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "coeff.h"
//---------------------------------------------------------------------------------------
#include "e2_note.h"
#include "a2_note.h"
#include "d3_note.h"
#include "g3_note.h"
#include "b3_note.h"
#include "e4_note.h"
//---------------------------------------------------------------------------------------
AudioTuner                tuner;
AudioOutputAnalog         dac;
AudioPlayMemory           wav_note;
AudioMixer4               mixer;
//---------------------------------------------------------------------------------------
AudioConnection patchCord0(wav_note, 0, mixer, 0);
AudioConnection patchCord1(mixer, 0, tuner, 0);
AudioConnection patchCord2(mixer, 0, dac, 0);
//---------------------------------------------------------------------------------------
IntervalTimer playNoteTimer;

void playNote(void) {
    if (!wav_note.isPlaying()) {
        wav_note.play(e2_note);
        //wav_note.play(a2_note);
        //wav_note.play(d3_note);
        //wav_note.play(g3_note);
        //wav_note.play(b3_note);
        //wav_note.play(e4_note);
        digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    }
}
//---------------------------------------------------------------------------------------
void setup() {
    AudioMemory(30);
    /*
     *  Initialize the yin algorithm's absolute
     *  threshold, this is good number.
     */
    tuner.begin(.15, fir_22059_HZ_coefficients, sizeof(fir_22059_HZ_coefficients), 2);
    pinMode(LED_BUILTIN, OUTPUT);
    playNoteTimer.begin(playNote, 1000);
}

void loop() {
    // read back fundamental frequency
    if (tuner.available()) {
        float note = tuner.read();
        float prob = tuner.probability();
        Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
    }
}
