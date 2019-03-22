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

#include <SPI.h>
#include <Wire.h>
#include <MIDIUSB.h>
#include <EasyButton.h>
#include <Encoder.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

   [dill]: <https://github.com/joemccann/dillinger>
   [git-repo-url]: <https://github.com/joemccann/dillinger.git>
   [john gruber]: <http://daringfireball.net>
   [df1]: <http://daringfireball.net/projects/markdown/>
   [markdown-it]: <https://github.com/markdown-it/markdown-it>
   [Ace Editor]: <http://ace.ajax.org>
   [node.js]: <http://nodejs.org>
   [Twitter Bootstrap]: <http://twitter.github.com/bootstrap/>
   [jQuery]: <http://jquery.com>
   [@tjholowaychuk]: <http://twitter.com/tjholowaychuk>
   [express]: <http://expressjs.com>
   [AngularJS]: <http://angularjs.org>
   [Gulp]: <http://gulpjs.com>

   [PlDb]: <https://github.com/joemccann/dillinger/tree/master/plugins/dropbox/README.md>
   [PlGh]: <https://github.com/joemccann/dillinger/tree/master/plugins/github/README.md>
   [PlGd]: <https://github.com/joemccann/dillinger/tree/master/plugins/googledrive/README.md>
   [PlOd]: <https://github.com/joemccann/dillinger/tree/master/plugins/onedrive/README.md>
   [PlMe]: <https://github.com/joemccann/dillinger/tree/master/plugins/medium/README.md>
   [PlGa]: <https://github.com/RahulHP/dillinger/blob/master/plugins/googleanalytics/README.md>