/*
  Input output pins functions

  Author: Denis Leskovar
  email: dumr666@gmail.com
  licence: just don't sell it

*/

#ifndef _ioDlib_h
#define _ioDlib_h

#include <Arduino.h>
// helpers define
#define ON true
#define OFF false
#define IN INPUT
#define OUT OUTPUT

// variables
extern bool timer1Tick;
extern bool timer1Toggle;
extern int newMillis;
extern int oldMillis;
extern int diffMillis;

class ioDlib
{
public:
  ioDlib(int pin);                                         //Constructor. attach pin to blink
  void blink(bool value);                                  //enable blinking with 1s duration
  void blink(bool value, int blinkLength);                 //enable blinking with blink duration
  void blink(bool value, int blinkLength, int blinkLoops); //enable blinking with blink duration and number of loops
  // Timers and millis
  void setupTimer1();                     // setup Timer 1
  static void OCR1A_ISR();                // class interrupt routine
  void clearT1Tick();                     // clear timer1Tick var to false
  void clearT1Toggle();                   // clear toggle boolean
  void serialPrintMillis();               // print millis
  void calculateMillis(bool serialPrint); // calculate Millis

private:
  uint8_t pinNumber;
};

#endif