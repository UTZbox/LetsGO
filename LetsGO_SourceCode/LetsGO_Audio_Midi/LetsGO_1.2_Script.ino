/* LetsGO Cue controller and Stereo USB Audio and MIDI Interface.
   Created by Mike Utz
  
   Contains 6 push buttons for sending Key Strokes and MIDI messages,
   and Mode Switches for setting whether the Cue Inputs
   send Keytrokes for Q-Lab/GO Button incl. Midi Notes or for Ableton Live incl.  MSC messages.
   Two Outputs shows if the Deice is started up (Ready/loop) and one while Midi Commands will be sent (Transmit)

   The Modi Mode switch is connected to input pin 17
   The Keyboard Mode Switches are connected to Pin 20 and 21
   The Cue Inputs are connected to input pins 2 - 7.
   The Outputs are connected to Pin 14 for RUN and 15 for Transmit LED.
   The Midi Interface is connected to the pins 0 and 1
   
   You must select Board: Teensy 3.2 and USB-Type Audio+Keyboard+MIDI from the "Tools" menu for this code to compile.

  
   History:
   Version 1.2 /   Added USB-MIDI to/from Serial-MIDI Interface function + BugFix SinceLastCue Timer
   Version 1.1 /   changed Pins for mode Switch, because of double using of the SGTL5000 chip
   Version 1.0 /   finaly added the USB Audio funktion using the Teensy Audio Board (SGTL5000)
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

//Configuring the Audio settings and routing
AudioInputUSB            usb1;           //xy=165,74
AudioInputI2S            i2s1;           //xy=166,124
AudioOutputUSB           usb2;           //xy=624,126
AudioOutputI2S           i2s2;           //xy=625,74
AudioConnection          patchCord1(usb1, 0, i2s2, 0);
AudioConnection          patchCord2(usb1, 1, i2s2, 1);
AudioConnection          patchCord3(i2s1, 0, usb2, 0);
AudioConnection          patchCord4(i2s1, 1, usb2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=174,184

//The number of push buttons (exclusive the Toggle Switch for MIDI Mode)
const int NUM_OF_BUTTONS = 6;
// First Pin Number 
const int FIRSTPIN = 2;
// Last Pin Number
const int LASTPIN = 8;

// the MIDI channel number to send messages
const int MIDI_CHAN = 16;

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Create Bounce objects for each button and switch. The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
// 5 = 5 ms debounce time which is appropriate for good quality mechanical push buttons.
// If a button is too "sensitive" to rapid touch, you can increase this time.

//button debounce time
const int DEBOUNCE_TIME = 5;

// Timer for delay a next Cue, if one has sent.
elapsedMillis sinceLastCue;

Bounce buttons[NUM_OF_BUTTONS] =
{
  Bounce (),
  Bounce (),
  Bounce (),
  Bounce (),
  Bounce (),
  Bounce (),
};



//Variable that stores the current MIDI or keyboard mode of the device (what type of messages the push buttons send).
// Midi Mode 0 = is for Midi Notes and = 1 is for Midi Schow Control MSC
// Key Mode 0 = is for QLab, = 1 is for GoButton, = 2 is for Ableton Live
int midiMode = 0;
int keyMode = 0;

//Arrays and Variable which store the exact note and MSC messages each push button will send.
const int MIDI_NOTE_NUMS[NUM_OF_BUTTONS] = {40, 41, 42, 43, 44, 45};
const int MIDI_NOTE_VELS[NUM_OF_BUTTONS] = {110, 110, 110, 110, 110, 110};

const int HW_MIDI_MSC_LENGHT = 6;
byte MIDI_MSC_COMMAND[] = {0xf0, 0x7f, 0x7f, 0x02, 0x7f, 0x01, 0xf7};
byte MIDI_MSC_VALUE[] = {0x01, 0x0A, 0x12, 0x11, 0x00, 0x00};

//Arrays that stores the Keystrokes for Q-Lab and GO Button
const int KEY_STROKE_0[NUM_OF_BUTTONS] = {KEY_SPACE, KEY_ESC, KEY_UP, KEY_DOWN, KEY_MEDIA_VOLUME_INC, KEY_MEDIA_VOLUME_DEC};
const int KEY_STROKE_1[NUM_OF_BUTTONS] = {KEY_SPACE, KEY_ESC, KEY_UP, KEY_DOWN, KEY_9, KEY_0};
const int KEY_STROKE_2[NUM_OF_BUTTONS] = {KEY_ENTER, KEY_P, KEY_UP, KEY_DOWN, KEY_MEDIA_VOLUME_INC, KEY_MEDIA_VOLUME_DEC};


//==============================================================================
// Define Functions for receiving and sending out MIDI
// Reading Serial MIDI and map it to USB-MIDI

void serNoteOn(byte channel, byte note, byte velocity) {
  usbMIDI.sendNoteOn(note, velocity, channel);
}
void serNoteOff(byte channel, byte note, byte velocity) {
  usbMIDI.sendNoteOff(note, velocity, channel);
}
void serControlChange(byte channel, byte number, byte value) {
  usbMIDI.sendControlChange(number, value, channel);
}
void serProgramChange(byte channel, byte program) {
  usbMIDI.sendProgramChange(program, channel);
}
void serPitchChange(byte channel, int pitch) {
  usbMIDI.sendPitchBend(pitch, channel);
}

// Reading USB-MIDI and map it to Serial MIDI

void usbNoteOn(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOn(note, velocity, channel);
}
void usbNoteOff(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOff(note, velocity, channel);
}
void usbControlChange(byte channel, byte control, byte value) {
  MIDI.sendControlChange(control, value, channel);
}
void usbProgramChange(byte channel, byte program) {
  MIDI.sendProgramChange(program, channel);
}
void usbPitchChange(byte channel, int pitch) {
  MIDI.sendPitchBend(pitch, channel);
}

//==============================================================================
//The setup function. Called once when the Teensy is turned on or restarted

void setup()
{
  // Configuring the Audio Function and setup the SGTL500 Chip
  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  sgtl5000_1.lineInLevel(0);
  sgtl5000_1.lineOutLevel(13);

  // Configure usbMIDI and MIDI Message handling
  usbMIDI.setHandleNoteOff(usbNoteOff);
  usbMIDI.setHandleNoteOn(usbNoteOn);
  usbMIDI.setHandleControlChange(usbControlChange);
  usbMIDI.setHandleProgramChange(usbProgramChange);
  usbMIDI.setHandlePitchChange(usbPitchChange);

  MIDI.setHandleNoteOff(serNoteOff);
  MIDI.setHandleNoteOn(serNoteOn);
  MIDI.setHandleControlChange(serControlChange);
  MIDI.setHandleProgramChange(serProgramChange);
  MIDI.setHandlePitchBend(serPitchChange);
  
  
  // Configure the pins for input.
  // has external pulldown resistors!

  for (int i = FIRSTPIN; i < LASTPIN; i++)
  {
    pinMode (i, INPUT);
  }

  //Configure Mode Inputs
  pinMode(17, INPUT);
  pinMode(20, INPUT);
  pinMode(21, INPUT);

  // Configure Output pins for Ready and Transmit LED
  pinMode (14, OUTPUT);
  pinMode (15, OUTPUT);

  // Setup and Assign the Pins to the Bounce Library

  for (int i = 0; i < NUM_OF_BUTTONS; i++)
  {
    buttons[i].attach(i + FIRSTPIN);
    buttons[i].interval(DEBOUNCE_TIME);
  }
  // Start the Hardware MIDI Output
  MIDI.begin(MIDI_CHANNEL_OMNI);
}


//==============================================================================


void loop()
{
  //==============================================================================
  // Update all the buttons/switch. 
  for (int i = 0; i < NUM_OF_BUTTONS; i++)
  {
    buttons[i].update();
  }

  //==============================================================================
  // Check the status of each push button

  for (int i = 0; i < NUM_OF_BUTTONS; i++)
  {

    //========================================
    // Check each button for "rising" edge and if the timer is reached its value.
    
    if (buttons[i].risingEdge() and sinceLastCue >=45 )
    {
      //Send Keystroke or Midi Message on related Mode.
      if (keyMode == 0)
      {
        Keyboard.press (KEY_STROKE_0[i]);
      }

      if (keyMode == 1)
      {
        Keyboard.press (KEY_STROKE_1[i]);
      }

      if (keyMode == 2)
      {
        Keyboard.press (KEY_STROKE_2[i]);
      }

       if (midiMode == 0)
      {
        MIDI.sendNoteOn (MIDI_NOTE_NUMS[i], MIDI_NOTE_VELS[i], MIDI_CHAN);
        }
      
      if (midiMode == 1)
      { MIDI_MSC_COMMAND[5] = MIDI_MSC_VALUE[i];
        MIDI.sendSysEx (HW_MIDI_MSC_LENGHT, MIDI_MSC_COMMAND, true);
      }
      
      // Turn the Transmit Output (LED) on
      digitalWrite(15, HIGH);
      // Reset Timer
      sinceLastCue = 0;    
    }

    //========================================
    // Check each button for "falling" edge
    
    else if (buttons[i].fallingEdge() and sinceLastCue >=5 )
    {
      //Send Keystroke or Midi Message on related Mode.
     
      if (midiMode == 0)
      { 
        MIDI.sendNoteOff (MIDI_NOTE_NUMS[i], 0, MIDI_CHAN);
      }
      
      if (keyMode == 0)
      {
        Keyboard.release (KEY_STROKE_0[i]);
      }

      if (keyMode == 1)
      {
        Keyboard.release (KEY_STROKE_1[i]);
      }

      if (keyMode == 2)
      {
        Keyboard.release (KEY_STROKE_2[i]);
      }

      // Turn the Transmit Output (LED) off
      digitalWrite(15, LOW);
      // Reset Timer
      sinceLastCue = 0;
    }

  }

  //==============================================================================
  // Check the status of the Mode Switches and set the desired Device Mode.
  
  if (digitalRead(17) == HIGH)
  {
    midiMode =1;
  }
  else
  {
    midiMode = 0;
  }


  if (digitalRead(20) == HIGH)
  {
    keyMode =1;
  }
  else if (digitalRead(21) == HIGH)
  {
    keyMode = 2;
  }
  else
  {
    keyMode = 0;
  }


  //==============================================================================
  // Set Ready Output (LED)
  if (digitalRead(14) == LOW)
  {
    digitalWrite(14, HIGH);
  }

  //==============================================================================
  // Read Incomming MIDI Messages
  {
    usbMIDI.read();
    MIDI.read();
  }

}
