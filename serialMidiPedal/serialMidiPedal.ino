
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

// custom libs
#include "ioDlib.h"
#include "Debounce.h"

// Defines
#define ledPin 13
#define doL1 3
#define doL2 4
#define doL3 5
#define doL4 6

#define diB1 7
#define diB2 8
#define diB3 9

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define global vars
// LED states
short doL1s = LOW;
short doL2s = LOW;
short doL3s = LOW;
short doL4s = LOW;
// debounce for first button
unsigned long longTickCnt = 0;     // debounce start time for B1
unsigned long timeDebouncing = 20; // how long to wait for debounce

// ioDLib init
ioDlib pin(ledPin);
// Debounce lib init
Debounce diBut1(diB1, 30);
Debounce diBut2(diB2, 30);
Debounce diBut3(diB3, 30);

/*************************/
/********* MAIN **********/
/*************************/

/*******   SETUP   *******/
uint8_t randX = 0;
uint8_t randY = 0;
int randr = 0;
void setup()
{
  randomSeed(analogRead(0));
  setupIOs();
  setupOled();
  Serial.begin(115200);
  pin.setupTimer1(); // setup interrupt
}

/*******   LOOP   *******/
void loop()
{

  display.clearDisplay();
  digitalWrite(doL1, diBut1.read());
  digitalWrite(doL2, diBut2.read());
  digitalWrite(doL3, diBut3.count() % 2);

  //testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps

  if (timer1Tick == true)
  {
    longTickCnt++;
    // Drawing

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
  display.setRotation(1);

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}