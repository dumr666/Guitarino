/*
 * Midi pedal board, needs software installed for emulating midi
 * 
 * Author: Denis Leskovar
 * Email: dumr666@gmail.com
 * 
 */

// Includes
// Arduino official libs

#include <SPI.h>
#include <Wire.h>
#include <MIDI.h>
#include <EasyButton.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
// custom libs
#include "ioDlib.h"

// Defines
// IO Defines
// LED defines
#define ledPin 13
#define doL1 15
#define doL2 14
#define doL3 16
#define doL4 8
// LED states
short doL1s = LOW;
short doL2s = LOW;
short doL3s = LOW;
short doL4s = LOW;

// Button Defines
#define diB1 A3
#define diB2 A2
#define diB3 A1
#define diB4 A0
unsigned long buttonDebounceTime = 20; // how long to wait for debounce

// OLED Variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Define proper RST_PIN if required.
#define RST_PIN -1
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
SSD1306AsciiWire oled;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define MIDI_CHAN
long longTickCnt = 0;
// ioDLib init
int timer1TickCounter = 0;
ioDlib pin(ledPin);
// Instance of the button.
EasyButton diBut1(diB1, 20, false, false);
EasyButton diBut2(diB2, 20, false, false);
EasyButton diBut3(diB3, 20, false, false);
EasyButton diBut4(diB4, 20);

// Button array
bool bReadChanges = false;
bool inStates[] = {false, false, false, false};
bool inOldStates[] = {false, false, false, false};
bool inChangedStates[] = {false, false, false, false};
uint8_t inArrayLength = (sizeof(inChangedStates) / sizeof(inChangedStates[0]));
byte ccCommands[] = {111, 81, 82, 75};
// LED array
uint8_t outStates[] = {LOW, LOW, LOW, LOW};
uint8_t outLEDs[] = {doL1, doL2, doL3, doL4};
unsigned long secMillisLedStart = {0, 0, 0, 0};
unsigned long secMillisLedCurr = {0, 0, 0, 0};


unsigned long secMillisStart = 0;
int randr = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

/**************************************************/
/*******   SETUP   ********************************/
/*******   LOOP    *******************************/
/**************************************************/
void setup()
{
  //MIDI.begin();
  Serial.begin(115200);
  setupIOs();
  setupOled();
  pin.setupTimer1(); // setup interrupt
}

/**************************************************/
/********  MAIN   *********************************/
/********  LOOP   *********************************/
/**************************************************/
void loop()
{
  readInputs();
  lightLED();
  if (timer1Tick == true)
  {

    printInputs();
    timer1TickCounter++;
    if (timer1TickCounter == 10)
    {
      setChanges();
      //printInputs();
      digitalWrite(13, !digitalRead(13));
      timer1Second();
      bReadChanges = false;
      resetChangedStates();
    }
    //digitalWrite(doL4, !digitalRead(doL4));
    timer1Tick = false;
  }
  //Serial.println(pin.retard);
}

/*************************/
/********** END **********/
/*************************/

/***************************************/
/*******   FUNCTION DEFINITONS   *******/
// Check states with previous states

// Setup inputs outputs
void setupIOs()
{
  // setup four out leds
  pinMode(doL1, OUTPUT);
  pinMode(doL2, OUTPUT);
  pinMode(doL3, OUTPUT);
  pinMode(doL4, OUTPUT);

  // setup three in Buttons
  diBut1.begin();
  diBut2.begin();
  diBut3.begin();
}

// Setup OLED1306 with Adafruit_SSD1306
void setupOled()
{
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else  // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  oled.setFont(Stang5x7);
  oled.set2X();
  oled.clear();
  //oled.setLetterSpacing(0);
  oled.setCursor(0, 0);
  oled.print("SUPRISE");
  oled.setCursor(0, 4);
  oled.print("motherfucker!");
  delay(500);
  oled.clear();
  printBlank();
}

void printCommandNum(int num, String type)
{
  oled.clear();
  oled.setFont(lcdnums14x24);
  oled.set2X();
  oled.home();
  oled.print(num);
  oled.setFont(ZevvPeep8x16);
  oled.setCursor(100,0);
  oled.set2X();
  oled.print(type);
}

void printBlank()
{
  oled.clear();
  oled.setFont(lcdnums14x24);
  oled.set2X();
  oled.home();
  oled.print("---");
  oled.setFont(ZevvPeep8x16);
  oled.setCursor(100,0);
  oled.set2X();
  oled.print("--");
}

void readInputs()
{
  diBut1.read();
  diBut2.read();
  diBut3.read();
  inStates[0] = diBut1.isPressed();
  inStates[1] = diBut2.isPressed();
  inStates[2] = diBut3.isPressed();
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    if ((inStates[i] == true) && (inChangedStates[i] == false))
    {
      bReadChanges = true;
      inChangedStates[i] = inStates[i];
    }
  }
}

void resetChangedStates()
{
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    inChangedStates[i] = false;
  }
}

void setChanges()
{
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    if (inChangedStates[i] == true)
    {
      Serial.println(ccCommands[i]);
      printCommandNum(ccCommands[i], "CC");
    }
  }
}

void lightLED()
{

  digitalWrite(doL1, inChangedStates[0]);
  digitalWrite(doL2, inChangedStates[1]);
  digitalWrite(doL3, inChangedStates[2]);
}

void timer1Second()
{
  longTickCnt++;
  // Set counter for 10hz to 0
  timer1TickCounter = 0;
}

void printInputs()
{
  Serial.print("B1:");
  Serial.print(inChangedStates[0]);
  Serial.print("\t");
  Serial.print("B2:");
  Serial.print(inChangedStates[1]);
  Serial.print("\t");
  Serial.print("B3:");
  Serial.print(inChangedStates[2]);
  Serial.print("\t");
  Serial.print("B4:");
  Serial.print(inChangedStates[3]);
  Serial.println("\t");
}

// Finish stuff after one second
void oneSecond()
{
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    
  }
  if ((secMillisCurr - secMillisStart) >= 1000)
  {
    
  }
}
