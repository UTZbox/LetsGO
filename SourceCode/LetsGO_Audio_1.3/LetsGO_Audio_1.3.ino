/* LetsGO Cue controller. (Basic Version)
   Created by Mike Utz
  
   Contains 4 push buttons for sending Keystrokes and MIDI over USB plus MIDI messages,
   Mode Switches for setting different Keystroke and Midi functions
   Two Outputs shows if the Deice is started up (Ready) and one while Midi Commands will be sent (Transmit)

   The Cue Inputs are connected to input pins 2 - 5.
   The Keyboard Mode Switches are connected to Pin 6 -8
   The Midi Mode switch is connected to input pin 14 + 15
   The USB-MIDI ON/OFF switch is connected to input pin 20
   The USB Channel Switch is connected to imput pin 21
   The Outputs are connected to Pin 16 for RUN and 17 for Transmit LED.
   The Midi Interface is connected to the pins 0 and 1
   
   You must select Board: Teensy-3.2 and USB-Type: Audio+Keyboard+MIDI+Serial from the "Tools menu.

  
   History:
   Version 1.3  / Added USB -Audio Interface, with Teensy Audio-Board. (Based on LetsGO v1.2)
   Version 0.7   / Changed the Keyboard and Midi Mode Pins to match when it be used with the Audio-Board
   Version 0.6   / Added USBmidi and ON/OFF switch for it
   Version 0.5   / Added more Keyboard and Midi Modes, changed PinOut for more Modes, Cue Inputs HighLevel Active / Mode Inputs LowLevel Active
   Version 0.4.2 / Bug fixing not working timer SinceLastCue
   Version 0.4.1 / Changed In/Output pins for Mode switching and LEDs
   Version 0.4 /   Add different Modes for MIDI and Keyboard
   Version 0.3 /   Add a timer to prevent double actions, because of flickering inputs
   Version 0.2.3 / Changed MidiMode Switch to Input Pin 15
   Version 0.2.2 / Changed from Pullup to Pulldown Resistor on Inputs
   Version 0.2.1 / Changed Mode Button function (remove toggle)
   Version 0.2 /   Added 2 buttons for Volume Control (Media PLayer)
   Version 0.1 /   Modified Script starting from MidiGO V0.3
*/

#include <Bounce2.h>
#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//Configuring the Audio settings and routing
AudioInputUSB            usb1;           //xy=200,69  (must set Tools > USB Type to Audio)
AudioOutputI2S           i2s1;           //xy=365,94
AudioConnection          patchCord1(usb1, 0, i2s1, 0);
AudioConnection          patchCord2(usb1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184

//The number of push buttons (exclusive the Toggle Switch for MIDI Mode)
const int NUM_OF_BUTTONS = 4;
// First Pin Number 
const int FIRSTPIN = 2;
// Last Pin Number
const int LASTPIN = 6; // Last used Pin Nr. +1 !

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Create Bounce objects for each button and switch.

//button debounce time
const int DEBOUNCE_TIME = 5;

// Timer for delay a next Cue, if one has sent.
elapsedMillis sinceLastCue;

Bounce buttons[NUM_OF_BUTTONS] = {
  Bounce (),
  Bounce (),
  Bounce (),
  Bounce (),
};

//Variable that stores the current MIDI or keyboard mode of the device (what type of messages the push buttons send).
int midiMode = 0;
int keyMode = 0;
// Variable to Store the State of USB MIDI Send ON/OFF
int usbMidiState = 0;
//Variable to Store the chosen MIDI Channel
int MIDI_CHAN = 1;

//Arrays and Variable which store the exact MIDI messages each push button will send.
const int MIDI_NOTE_NUMS[NUM_OF_BUTTONS] = {60, 61, 62, 63};
const int MIDI_NOTE_VELS[NUM_OF_BUTTONS] = {110, 110, 110, 110};

const int MIDI_MSC_LENGHT = 7;
const int HW_MIDI_MSC_LENGHT = 6;
byte MIDI_MSC_COMMAND[] = {0xf0, 0x7f, 0x7f, 0x02, 0x7f, 0x01, 0xf7};
byte MIDI_MSC_VALUE[] = {0x01, 0x0A, 0x11, 0x12};

const int MIDI_PRG_NUM[NUM_OF_BUTTONS] = {0, 1, 2, 3};

//Arrays that stores the Keystrokes
const int KEY_STROKE_0[NUM_OF_BUTTONS] = {KEY_SPACE, KEY_ESC, KEY_DOWN, KEY_UP};               // Qlab or GObutton
const int KEY_STROKE_1[NUM_OF_BUTTONS] = {KEY_ENTER, KEY_P, KEY_DOWN, KEY_UP};                 // Ableton Live
const int KEY_STROKE_2[NUM_OF_BUTTONS] = {KEY_DOWN, KEY_UP, KEY_B, KEY_F5};                    // PowerPoint on Windows
const int KEY_STROKE_3[NUM_OF_BUTTONS] = {KEY_DOWN, KEY_UP, KEY_B, KEY_ENTER};                 // PowerPoint on Mac
const int KEY_STROKE_4[NUM_OF_BUTTONS] = {KEY_DOWN, KEY_UP, KEY_B, KEY_P};                     // Keynote on Mac
const int KEY_STROKE_5[NUM_OF_BUTTONS] = {KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_DOWN, KEY_UP};       // PDF Reader
const int KEY_STROKE_6[NUM_OF_BUTTONS] = {KEY_1, KEY_2, KEY_3, KEY_4};                         // HOTKEYS for QLAB, Ableton Live or DAWs

//==============================================================================
//==============================================================================
//The setup function. Called once when the Teensy is turned on or restarted

void setup() {
  // Configuring the Audio Function and setup the SGTL500 Chip
  AudioMemory(12);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  sgtl5000_1.lineOutLevel(29);
  // Configure the pins for input which has external pulldown resistors!

  for (int i = FIRSTPIN; i < LASTPIN; i++) {
    pinMode (i, INPUT);
  }

  //Configure Mode Inputs
    pinMode (6, INPUT_PULLUP);
    pinMode (7, INPUT_PULLUP);
    pinMode (8, INPUT_PULLUP);
    pinMode (14, INPUT_PULLUP);
    pinMode (15, INPUT_PULLUP);
    pinMode (20, INPUT_PULLUP);
    pinMode (21, INPUT_PULLUP);

  // Configure Output pins for Ready and Transmit LED
  pinMode (16, OUTPUT);
  pinMode (17, OUTPUT);

  // Setup and Assign the Pins to the Bounce Library

  for (int i = 0; i < NUM_OF_BUTTONS; i++) {
    buttons[i].attach(i + FIRSTPIN);
    buttons[i].interval(DEBOUNCE_TIME);
  }
  // Start the Hardware MIDI Output
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  // Set Ready Output (LED)
  digitalWrite(16, HIGH);
}


//==============================================================================


void loop()
{
  // read the PC's volume setting and set it to the Audio Processor
  float vol = usb1.volume();
  sgtl5000_1.volume(vol);
  
  //==============================================================================
  // Update all the buttons/switch. 
  for (int i = 0; i < NUM_OF_BUTTONS; i++) {
    buttons[i].update();
  }

  //==============================================================================
  // Check the status of each push button

  for (int i = 0; i < NUM_OF_BUTTONS; i++) {

    //========================================
    // Check each button for "rising" edge and if the timer is reached its value.
    
    if (buttons[i].risingEdge() and sinceLastCue >=50 ) {
      //Send Keystroke on related Mode.
      if (keyMode == 1) {
        Keyboard.press (KEY_STROKE_0[i]);
      }

      if (keyMode == 2) {
        Keyboard.press (KEY_STROKE_1[i]);
      }

      if (keyMode == 3) {
        Keyboard.press (KEY_STROKE_2[i]);
      }
      
      if (keyMode == 4) {
        if (buttons[3].risingEdge()) {
          Keyboard.press(MODIFIERKEY_GUI);
          delay(5);
          Keyboard.press(MODIFIERKEY_SHIFT);
          delay(5);
        }
        Keyboard.press (KEY_STROKE_3[i]);
      }
      
      if (keyMode == 5) {
        if (buttons[3].risingEdge()) {
          Keyboard.press(MODIFIERKEY_ALT);
          delay(5);
          Keyboard.press(MODIFIERKEY_GUI);
          delay(5);
        }        
        Keyboard.press (KEY_STROKE_4[i]);
      }
      
      if (keyMode == 6) {
        Keyboard.press (KEY_STROKE_5[i]);
      }
      
      if (keyMode == 7) {
        Keyboard.press (KEY_STROKE_6[i]);
      }
      
      // Send Midi Message on related Mode.
      if (midiMode == 1) {
        MIDI.sendNoteOn (MIDI_NOTE_NUMS[i], MIDI_NOTE_VELS[i], MIDI_CHAN);
        if (usbMidiState == 1){
          usbMIDI.sendNoteOn (MIDI_NOTE_NUMS[i], MIDI_NOTE_VELS[i], MIDI_CHAN);
        }
      }
      
      if (midiMode == 2) {
        MIDI_MSC_COMMAND[5] = MIDI_MSC_VALUE[i];
        MIDI.sendSysEx (HW_MIDI_MSC_LENGHT, MIDI_MSC_COMMAND, true);
        if (usbMidiState == 1){
          usbMIDI.sendSysEx (MIDI_MSC_LENGHT, MIDI_MSC_COMMAND, true);
        }
      }

      if (midiMode == 3) {
        MIDI.sendProgramChange (MIDI_PRG_NUM[i], MIDI_CHAN);
        if (usbMidiState == 1){
          usbMIDI.sendProgramChange (MIDI_PRG_NUM[i], MIDI_CHAN);
        }
      }
      
      // Turn the Transmit Output (LED) on
      digitalWrite(17, HIGH);
      
      // Reset Timer
      sinceLastCue = 0;
    }

    //========================================
    // Check each button for "falling" edge
    
    else if (buttons[i].fallingEdge() and sinceLastCue >=25 ) {
      //Send Keystroke or Midi Message on related Mode.
     
      if (midiMode == 1) { 
        MIDI.sendNoteOff (MIDI_NOTE_NUMS[i], 0, MIDI_CHAN);
        if (usbMidiState == 1){
          usbMIDI.sendNoteOff (MIDI_NOTE_NUMS[i], 0, MIDI_CHAN);
        }
      }
      
      if (keyMode == 1) {
        Keyboard.release (KEY_STROKE_0[i]);
      }

      if (keyMode == 2) {
        Keyboard.release (KEY_STROKE_1[i]);
      }

      if (keyMode == 3) {
        Keyboard.release (KEY_STROKE_2[i]);
      }
      
      if (keyMode == 4) {
        Keyboard.release (KEY_STROKE_3[i]);
        if (buttons[3].fallingEdge()) {
          Keyboard.release(MODIFIERKEY_SHIFT);
          Keyboard.release(MODIFIERKEY_GUI);
        }
      }
      
      if (keyMode == 5) {
        Keyboard.release (KEY_STROKE_4[i]);
        if (buttons[3].fallingEdge()) {
          Keyboard.release(MODIFIERKEY_GUI);
          Keyboard.release(MODIFIERKEY_ALT);
        }        
      }

      if (keyMode == 6) {
        Keyboard.release (KEY_STROKE_5[i]);
      }

      if (keyMode == 7) {
        Keyboard.release (KEY_STROKE_6[i]);
      }
      
      // Turn the Transmit Output (LED) off
      digitalWrite(17, LOW);
      
      // Reset Timer
      sinceLastCue = 0;
    }

  }

  //==============================================================================
  // Check the status of the Mode Switches and set the desired Device Mode.

  // Mode Switch for Keboard (BCD Logic)
  if (digitalRead(6) == LOW && digitalRead(7) == LOW && digitalRead(8) == LOW) {
    keyMode = 7;
  }
  else if (digitalRead(7) == LOW && digitalRead(8) == LOW) {
    keyMode = 6;
  }  
  else if (digitalRead(6) == LOW && digitalRead(8) == LOW) {
    keyMode = 5;
  }
  else if (digitalRead(8) == LOW) {
    keyMode = 4;
  }
  else if (digitalRead(6) == LOW && digitalRead(7) == LOW) {
    keyMode = 3;
  }      
  else if (digitalRead(7) == LOW) {
    keyMode = 2;
  }
  else if (digitalRead(6) == LOW) {
    keyMode =1;
  }
  else {
    keyMode = 0; // No Keyboard Action
  }

  // Mode Switch for MIDI (BCD Logic)
  if (digitalRead(14) == LOW && digitalRead(15) == LOW) {
    midiMode = 3;
  }
  else if (digitalRead(15) == LOW) {
    midiMode = 2;
  }
  else if (digitalRead(14) == LOW) {
    midiMode = 1;
  }
  else {
    midiMode = 0; // No Midi Action
  }
  
  // Mode Switch for USB-MIDI ON/OFF
  if (digitalRead(20) == LOW) {
    usbMidiState = 1;
  }
  else {
    usbMidiState = 0;
  }

  // Channel Switch for MIDI Channel
  if (digitalRead(21) == LOW) {
    MIDI_CHAN = 2;
  }
  else {
    MIDI_CHAN = 1;
  }
  //==============================================================================
  // MIDI Controllers should discard incoming MIDI messages to prevent buffer overflow
  while (MIDI.read()) {
    // read & ignore incoming messages
  }
  while (usbMIDI.read()) {
    // read & ignore incoming messages
  }
  //==============================================================================

}
