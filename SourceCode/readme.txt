If you want to change or compile the code, you have to adapt the USB-Type within Teensyduino.
You can do this by our own, replacing the files in this folder within Teensduino.
But please backup the original files first before replacing it.
This replacement is made for Teensyduino v1.8.19.
It will not work for Arduino v2.x

Because there was some problmens in the compiler I did not renamed the USB Types within the Menu.
I just added the Keyboard function to the ...MIDIx16... Type. (but reduced the MIDI Count to 1.)

Please use follow USB-Type for compiling the code: (After you replaced the files in Teensduino off course)
TEENSY LC,  USB Type = Serial + MIDIx16
TEENSY 4.0, USB Type = Serial + MIDIx16 + Audio
