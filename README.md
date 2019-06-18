# LetsGO
DIY, Advanced Remote CUE Controller for QLAB®, GO-Button® and Ableton© Live®

// Introduction:
This Project let you easy build USB Cue Controller, with integrated Audio and MIDI-Interface for using with QLAB®, GO-Button® and Ableton© Live®. No driver are needed to work. Just Plug n Play.
The Functionality goes up to USB and MIDI Cue Control, Wireless-Remote, USB-Audio and MIDI-Interface!
(Similar to Devies Like Audio-Ape or AirTurn, but with more functions integrated)

LetsGO is a professional developed and testet device, which is used all day in different by Theaters and Artist.

// What can the LetsGO do?
It can control the Software via 6 Push-buttons and 3 Mode Switch.
It has a Serial Midi Output, to control a Backup-Player or other Devices. It has a built in Stereo Audio Output with Line Level, running on 44.1kHz. It has a built in USB-MIDI Interface.
(See LetsGO in Action here: https://youtu.be/YLvgWyX-pi8)

So you get an All in One Solution especially when using GO-Button with iPad or iPhone.

The Push-Buttons are for GO, STOP ALL (Panic), Select previous Cue, Select next Cue, Volume Up and Down.
To have some flexibility, there are 3 Mode buttons that switch the USB Commands and the Hardware Midi Output Commands.

The Controller using the Teensy Platform from Paul Stoffregen by PJRC, because it is very good and has a lot of functions ready.
(Much more than Arduino)

I programmed it to act as USB Keyboard, Audio and even Midi-Interface.
The Main Focus was to have a fast, save, reliable and easy to use Controller.
My tests showed up, that the connection of a USB-Keyboard is the fastet way for executing the Cues.
(Midi and OSC have always more latency)
The positive Side-effect is, that no Driver for OSX or iOS is needed. :)

The Schematics is developed, that you can easily add a Wireless Remote.

If you use an iPad (GO-Button), you are limited with USB connections.
The built-in Audio Output isn’t that good and the connector is to bad for a professional use.
Thats why I added an Audio Interface using the Teensy Audio-Shield.
Yes there are sure better Audio IC``s or Boards, but it has an acceptable quality.
(Please refer to Limitations)


To see if the Device is running or doing something, two Outputs for LEDs are programmed.
One for Power Up (RUN) and one for executing a Cue (TRANSMIT)

So in the the End you will get a professional developed tested device, but you can build it by our selves and save a lot of money.
And you can even adjust it to your special needs. How Cool is that ;)


// Functions:
The Basic Version act as HID USB-Keyboard and is used just for commands. So use this if you just need a external Controller.
It is based on a Teensy-LC to keep it fast and simple. (And cheap :))

The Audio Version act additionally as USB Audio Device with a Stereo Line-Level Output and Earphone-Amplifier.
It is used with a Teensy 3.2 and a Teensy Audio Shield on Top.

The Audio-Midi Version additionally acts as USB Audio and MIDI Interface
It is used with a Teensy 3.2 and a Teensy Audio Board on Top.
Also you have to use an Optocoupler for Hardware MIDI Input.


// Limitations:
Audio-Interface Output Only.
Because of some divisions within the code, the Sample-Rate isn’t exactly 44.1 Khz
QLAB, Go-Button and Ableton can handle it as long you just playing back some audio, because they sync to the actually Sample-Rate.
But it is not possible to use the Inputs at the moment, because therefore the Sample-Rate have to be exactly 44.1 Khz.
(or can be controlled by the applications, which is not possible with Teensy at the moment.)

MIDI-Interface Notes, ControlChange, PitchBend, ProgrammChnge only.
It is possible to add some more functions like AfterTouch, PolyPressure etc.
For the most applications the implemented functions are enough.
Also I want to prevent some CPU Power.


// Part-List:
A Part-List for all used Parts you will find as PDF within the Project.
You can use some other Parts for Buttons and Wireless-Remote.
But the minimum requirements are the Teensy-LC or (3.2), The Teensy Audio-Board (if used) and the Pull-Down-Resistors. If you want to use the Hardware Midi Input, you have to use an Optocoupler.

// Schematics:
You can find the Schematics as PDF within the Project!
The Schematics can be used for all versions. Just keep the not used Function/Part out.
(Please refer to Usage of Pull-Down Resistors)


// Used Input-Pins on Teensy:
Please refer to the PDF „Pin-Function_Matrix
Make sure you dont confound between Pin-Nr. and Pin-Label!

// Usage of Pull-Down Resistors:
I decide to work with external Pull-Down Resistor. The Reason why is the electric circumstances.
If you use Pull-Up resistors, any very short connection to ground, would execute the function.
This can even happen if you don’t have an electric connection, but thru  electrostatic discharge.
(Wearing some synthetical clothes and touching the case where the teensy or Push-Button is built in.)
So please use for every Push-Button Input a 10k Ohm Resistor. I highly recommend the 3.6V Z-Diode as well to protect the Inputs.
If you don’t want to use a dedicated Input, so just connect this input to ground! (GND)
This prevents it from suddenly execute a function


// HINT for Long wires connected:
If you want to use long distances between the „Push-Buttons“ and the Teensy, ( over 10m)
I highly recommend to use a Optocoupler as it is shown for the Midi Input.
Or use the schematic called (Long_Wire_Connection) which is also as .JPG within the Project.
If you are under 10 m the Inputs are protected by the Z-Diode and the Pulldown Resistor.
But make sure you always using the right polarity!

// Wireless Remote:
I made some very good experiences with the KAPPA-T868 Module from RF_Solutions.
It’s a digital Telemetry Radio Modul, which use the Frequency of 868 MHz.
This is much better than using any Bluetooth or other 2.4 GHz Standard.
From RF Solutions are some corresponding Pocket Remote Transmitter called FOBBER or other Modules available.

For users in the U.S.A or Canada! You have to use a Module with the Frequency of 433 MHz. (868 MHz. is not allowed)

// Connection to (iPad)
Whit the Camera Connection Kit you can connect it to the iPad.
No additional App or something is needed!
The Teensy will be powered from the iPad.
But I highly recommend to use the Apple USB3 Connector.
With this, you can power up the iPad thru the original Charger.

Maybe some Third-Party connectors may also work.
But I always use the original Apple one to run it save.


// Compiled Files:
You don’t have to do some coding ore compiling the source code for programming the Teensy.
Just download and install the Teensy Loader from PJRC.com.
https://www.pjrc.com/teensy/loader.html
With this you can program the Teensy directly, using the .hex file which are in the Project.
There are actually, 3 Versions:
LetsGO_basic (used with a Teensy LC)
LetsGO_Audio (used with a Teensy 3.2 and the Teensy Audio-Shield)
LetsGO_Aduio-Midi (used with a Teensy 3.2 and the Teensy Audio-Shield)


// Source Code:
Because I don’t want to hide it and try to make some money, the source code is within the project as well.
It took many hours to build it. Even if it looks very simple, there are a lot of work of testing and optimizing inside.
We use the „LetsGo“ all day in a professional setting and it has to be reliable, fast, save and easy to use.
So you are welcome to change the code to your needs. But please read the Important HINT for compiling the Source Code first.

// Used Libraries
Following Libraries are used by the Source Code:
Bounce2.h
MIDI.h
Audio.h
Wire.h
SPI.h
SD.h

// Important HINT for compiling Source Code:
Within the Teensyduino there is no combination of Audio + Keyboard or Audio + Midi + keyboard for compiling settings.
If you want to edit the Source Code and compile it new, you have to adapt the Teensyduino installation.
Therefore you have to replace two files named boards.txt and usb-desc.h
Before you do that make sure you make a copy of the files to a save place.
If something goes wrong, you have to complete reinstall Arduino and Teensyduino.

Target folder for boards.txt:
/Applications/Arduino.app/Contents/Java/hardware/teensy/avr

Target folder for usb-desc.h:
/Applications/Arduino.app/Contents/Java/hardware/teensy/avr/cores/teensy3

// Teensyduino Compiler Settings:
For the Basic Version without Audio and MIDI (Teensy LC) use following settings:
Board „Teensy LC“
USB Type: Keyboard
CPU Speed „48 Mhz“
Optimize „Faster“
Keyboard Layout „US_English“

For the whole functionality (Teensy 3.2) use following settings:
Board „Teensy 3.2 / 3.1“
USB Type: USB Audio + Keyboard MIDI + Serial
CPU Speed „72 Mhz“
Optimize „Faster“
Keyboard Layout „US_English“

(You can change the settings for CPU Speed and Optimize, but i recommend to use this)

// To dos and further improvements:
- Change the Audio Clock to 48 kHz
- Build a Basic Version with USB-MIDI Interface
- Add 3th Keyboard Mode for Ableton Live with KeyStroke 9 and 0 instead Media Volume

// Support
If you need some Support just send me an E-Mail to mail - at - utzs.ch (My Name is Mike)
Or do you wish to have a preconfigured and tested Board?
So just send me an E-Mail too. :-)

// Licensing:
The Project using Hardware, Software and Libraries which are released under the GNU and CC License.
Teensy and Teensyduino are registered Trade Marks by Paul Stoffregen. www.pjrc.com
QLAB and Go-Button are registered Trademarks by Figure 53. www.figure53.com
Ableton Live is a registered Trade Mark of Ableton www.ableton.com
Ipad, Mac, iOS and OSX are registered Trade Marks of Aplle Inc. www.apple.com
For details about using and licensing, please refer to the dedicated website or authors.
The LetsGO Project ist licensed under GNU V3. Copyright by Mike Utz. www.utzs.ch
