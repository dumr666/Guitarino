/*
blink.cpp - Simple example in creating your own Arduino Library
Copyright (c) op from TMM. All right reserved.
 
*/

#include <Arduino.h>
#include "ioDlib.h"
// Define global vars
// Timer stuff
bool timer1Tick = false;
bool timer1Toggle = false;
// Millis stuff
int newMillis = 0;
int oldMillis = 0;
int diffMillis = 0;

// Buttons stuff

ioDlib::ioDlib()
{
    __asm__("nop\n\t"); 
}

/*******   INTERRUPT ROUTINES   *******/
// Setup timer for 10 ms inttrrupt
void ioDlib::setupTimer1()
{
    noInterrupts();
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1 = 0;  // initialize counter value to 0
        // 10 Hz (16000000/((6249+1)*256))
    OCR1A = 6249;
    // CTC
    TCCR1B |= (1 << WGM12);
    // Prescaler 256
    TCCR1B |= (1 << CS12);
    // Output Compare Match A Interrupt Enable
    TIMSK1 |= (1 << OCIE1A);
    interrupts();
}

// Main interrupt routine for Timer 1
ISR(TIMER1_COMPA_vect)
{
    ioDlib::OCR1A_ISR();
}

// Class interrupt routine for Timer 1
void ioDlib::OCR1A_ISR()
{
    timer1Tick = true;
    timer1Toggle = !timer1Toggle;
}

// Clear interrupt T1 tick
void ioDlib::clearT1Tick()
{
    timer1Tick = false;
}

// Clear interrupt T1 toggle
void ioDlib::clearT1Toggle()
{
    timer1Toggle = false;
}

// Calculate millis
void ioDlib::calculateMillis(bool serialPrint)
{

    oldMillis = newMillis;
    newMillis = millis();
    diffMillis = newMillis - oldMillis;

    if (serialPrint == true)
    {
        serialPrintMillis();
    }
}

// Print millis
void ioDlib::serialPrintMillis()
{
    Serial.print("Old:");
    Serial.print(oldMillis);
    Serial.print("\t|\t New: ");
    Serial.print(newMillis);
    Serial.print("\t|\tDiff: ");
    Serial.print(diffMillis);
    Serial.print("\t|\tToogle val: ");
    Serial.print(timer1Toggle);
    Serial.print("\n");
}