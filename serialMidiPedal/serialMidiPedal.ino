/*
 * Midi pedal board, needs software installed for emulating midi
 * 
 * Author: Denis Leskovar
 * Email: dumr666@gmail.com
 * 
 */

// Includes
// Arduino official libs
#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>
// custom libs
#include "ioDlib.h"
#include "Debounce.h"

// Defines
// IO Defines
// LED defines
#define ledPin 13
#define doL1 3
#define doL2 4
#define doL3 5
#define doL4 6
// LED states
short doL1s = LOW;
short doL2s = LOW;
short doL3s = LOW;
short doL4s = LOW;

// Button Defines
#define diB1 7
#define diB2 8
#define diB3 9
unsigned long buttonDebounceTime = 20; // how long to wait for debounce

// OLED Variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MIDI_CHAN
long longTickCnt = 0;
// ioDLib init
ioDlib pin(ledPin);
// Debounce lib init
Debounce diBut1(diB1, buttonDebounceTime);
Debounce diBut2(diB2, buttonDebounceTime);
Debounce diBut3(diB3, buttonDebounceTime);
// Button array
uint8_t inStates[3] = {LOW, LOW, LOW};
uint8_t inOldStates[3] = {LOW, LOW, LOW};
uint8_t inChangedStates[3] = {LOW, LOW, LOW};
byte ccCommands[3] = {80, 81, 82};
// LED array
uint8_t outStates[3] = {LOW, LOW, LOW};
uint8_t outLEDs[3] = {doL1, doL2, doL3};

uint8_t randX = 0;
uint8_t randY = 0;
int randr = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

/**************************************************/
/*******   SETUP   ********************************/
/*******   LOOP    *******************************/
/**************************************************/
void setup()
{
  MIDI.begin(8);
  setupIOs();
  setupOled();
  //Serial.begin(115200);
  pin.setupTimer1(); // setup interrupt
}

/**************************************************/
/********  MAIN   *********************************/
/********  LOOP   *********************************/
/**************************************************/
void loop()
{
  readInputs();
  inputChanged();
  lightLED();
  if (timer1Tick == true)
  {
    applyChanges();
    
  display.clearDisplay();
    longTickCnt++;
    randX = random(64);
    randY = random(64);
    //display.clearDisplay();
    display.setCursor(randX, randY);
    display.drawPixel(randX, randY, WHITE);
    display.setTextColor(WHITE);
    display.setCursor(16, 70);
    display.setTextSize(8);
    display.print(longTickCnt);
    display.display();
    if (longTickCnt == 9)
    {
      longTickCnt = 0;
    }
    clearChanges();
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
  pinMode(diB1, INPUT);
  pinMode(diB2, INPUT);
  pinMode(diB3, INPUT);
}

// Setup OLED1306 with Adafruit_SSD1306
void setupOled()
{
   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  //delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  //display.setRotation(1);
  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

void readInputs()
{
  inStates[0] = diBut1.read();
  inStates[1] = diBut2.read();
  inStates[2] = diBut3.read();
  //digitalWrite(doL3, diBut3.count() % 2);  // TOGGLE
}

void inputChanged()
{
  for(uint8_t i = 0; i << (sizeof(inStates) / sizeof(inStates[0])); i++ )
  {
    if (inStates[i] != inOldStates[i]) {
      if (inStates[i] == HIGH) {
        inChangedStates[i] = HIGH;
      }
    }
  }
}

void clearChanges()
{
  for(uint8_t i = 0; i << (sizeof(inChangedStates) / sizeof(inChangedStates[0])); i++) 
  {
    inChangedStates[i] = LOW;
  }
}

void applyChanges()
{
  for(uint8_t i = 0; i << (sizeof(inChangedStates) / sizeof(inChangedStates[0])); i++ )
  {
    if (inChangedStates[i] == HIGH) {
      MIDI.sendControlChange(ccCommands[i],127,1);
      outStates[i] = HIGH;
    }
  }
}


void lightLED()
{
  for(uint8_t i = 0; i << (sizeof(inChangedStates) / sizeof(inChangedStates[0])); i++ )
  {
    digitalWrite(outLEDs[i], inChangedStates[i]);
  }
}