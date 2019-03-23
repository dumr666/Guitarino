# Guitarino
Guitarino is Arduino Pro Micro powered foot pedalboard with native MIDI USB support, it is work in progress and it should not be copied sold or whatever. Plesae also don't use the code in your school assigments, it is full of errors and baddly documented for now. But it works.

In current stage is implemented: 
  - Three buttons and three LEDs
  - I2C Oled screen
  - Magic
### Currently working on
 - Encoder is not working, it is connected to pin 0 and 1, but somehow interrupts from lib doesn't work
 - Documentation and explanation of code
 - Refactoring
 - Adding banks selector and preset switcher


### Technologies and libs

```sh
#include <SPI.h>
#include <Wire.h>
#include <MIDIUSB.h>
#include <EasyButton.h>
#include <Encoder.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
```