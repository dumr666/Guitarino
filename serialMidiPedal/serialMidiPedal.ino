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
#include <MIDIUSB.h>
#include <EasyButton.h>
#include <Encoder.h>
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
#define diB1 A1
#define diB2 A2
#define diB3 A3
#define diB4 A0
unsigned long buttonDebounceTime = 20; // how long to wait for debounce
EasyButton diBut1(diB1, buttonDebounceTime, false, false);
EasyButton diBut2(diB2, buttonDebounceTime, false, false);
EasyButton diBut3(diB3, buttonDebounceTime, false, false);
EasyButton diBut4(diB4, buttonDebounceTime, true, false);

// Encoder defines
//#define enc1 1
//#define enc2 0
Encoder enc1(1, 0);

unsigned long enc1PosOld = 0;

// OLED Variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Define proper RST_PIN if required.
#define RST_PIN -1
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
SSD1306AsciiWire oled;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define MIDI_CHAN 1
long longTickCnt = 0;
// ioDLib init
int timer1TickCounter = 0;
ioDlib pin(ledPin);
// Instance of the button.

// Button array
bool bReadChanges = false;
bool inStates[] = {false, false, false, false};
bool inOldStates[] = {false, false, false, false};
bool inChangedStates[] = {false, false, false, false};
uint8_t inArrayLength = (sizeof(inChangedStates) / sizeof(inChangedStates[0]));

//MIDI specific stuffW
uint8_t ccCommands[] = {111, 81, 3, 4};
int ccType[] = {1, 1, 2, 2};
uint8_t encCCComands[] = {85, 86};
int encCCType[] = {1, 1};
// LED array
uint8_t outStates[] = {false, false, false, false};
uint8_t outStateTick[] = {0, 0, 0, 0};
uint8_t outLEDs[] = {doL1, doL2, doL3, doL4};
unsigned long secMillisLedStart[] = {0, 0, 0, 0};
unsigned long secMillisLedCurr = 0;
unsigned long secInterval = 0;

unsigned long secMillisStart = 0;
int randr = 0;

//MIDI_CREATE_DEFAULT_INSTANCE();

/**************************************************/
/*******   SETUP   ********************************/
/*******   LOOP    *******************************/
/**************************************************/
void setup()
{
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
  readEncoder();
  lightLED();
  oneSecond();
  if (timer1Tick == true)
  {
    countStateTick();
    setChanges();
    //printMillis();
    //printInputs();
    resetChangedStates();
    timer1TickCounter++;
    if (timer1TickCounter == 10)
    {
      //printInputs();
      digitalWrite(13, !digitalRead(13));
      timer1Second();
      bReadChanges = false;
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

void sendMidiCommand(byte cmdNum, int cmdType)
{
  switch (cmdType)
  {
  case 1:
    controlChange(MIDI_CHAN, cmdNum, 127);
    printCommandNum(cmdNum, "CC");
    break;
  case 2:
    programChange(MIDI_CHAN, cmdNum);
    printCommandNum(cmdNum, "PC");
    break;
  }
}

void controlChange(byte channel, byte control, byte value)
{
  channel -= 1;
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void programChange(byte channel, byte program)
{
  // Array start at 0 :D
  channel -= 1;
  midiEventPacket_t msg = {0x0C, 0xC0 | channel, program, 0};
  MidiUSB.sendMIDI(msg);
  MidiUSB.flush();
}

void printCommandNum(uint8_t num, String type)
{
  oled.clear();
  oled.setFont(lcdnums14x24);
  oled.set2X();
  oled.home();
  oled.print(num);
  oled.setFont(ZevvPeep8x16);
  oled.setCursor(100, 0);
  oled.set2X();
  oled.print(type);
}

void writeOledBlank()
{
  oled.clear();
  oled.setFont(lcdnums14x24);
  oled.set2X();
  oled.home();
  oled.print("--");
  oled.setFont(ZevvPeep8x16);
  oled.setCursor(100, 0);
  oled.set2X();
  oled.print("--");
}

void printBlank()
{
  oled.clear();
  oled.setFont(lcdnums14x24);
  oled.set2X();
  oled.home();
  oled.print("---");
  oled.setFont(ZevvPeep8x16);
  oled.setCursor(100, 0);
  oled.set2X();
  oled.print("--");
}

void readInputs()
{
  diBut1.read();
  diBut2.read();
  diBut3.read();

  inStates[0] = diBut1.wasPressed();
  inStates[1] = diBut2.wasPressed();
  inStates[2] = diBut3.wasPressed();
  inStates[4] = diBut4.wasPressed();

  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    if ((inStates[i] == true) && (inChangedStates[i] == false))
    {
      bReadChanges = true;
      inChangedStates[i] = inStates[i];
    }
    if (inStates[i] == true)
    {
      secMillisLedStart[i] = millis();
      outStateTick[i] = 0;
      outStates[i] = true;
    }
  }
}

void readEncoder()
{
  unsigned long enc1Pos;
  enc1Pos = enc1.read();
  enc1Pos /= 4;
  uint8_t opNum = 5;
  if (!(enc1Pos == enc1PosOld))
  {
    Serial.print("Curr = ");
    Serial.print(enc1Pos);
    Serial.print("\tOld = ");
    Serial.print(enc1PosOld);
    Serial.println();
    if (enc1Pos > enc1PosOld)
    {
      opNum = 1;
    }
    else 
    {
      opNum = 0;
    }
    /*if ((enc1Pos > 1073741700) && (enc1PosOld == 0))
    {
      opNum = 0;
      enc1PosOld = enc1Pos;
    }
    else if (enc1Pos < enc1PosOld)
    {
      opNum = 0;
      sendMidiCommand(encCCComands[0], encCCType[0]);
    }
    else
    {
      opNum = 1;
      sendMidiCommand(encCCComands[1], encCCType[1]);
    }
  */
    
  }
  if (opNum < 5)
  {
    sendMidiCommand(encCCComands[opNum], encCCType[opNum]);
  }
  
    enc1PosOld = enc1Pos;
}

void countStateTick()
{
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    if (outStates[i] == true)
    {
      outStateTick[i] += 1;
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
      //MIDI.sendControlChange(ccCommands[i],127,1);
      //Serial.println(ccCommands[i]);
      //printCommandNum(ccCommands[i], "CC");
      sendMidiCommand(ccCommands[i], ccType[i]);
    }
  }
}

void lightLED()
{
  digitalWrite(doL1, outStates[0]);
  digitalWrite(doL2, outStates[1]);
  digitalWrite(doL3, outStates[2]);
}

// Finish stuff after one second
void oneSecond()
{
  secMillisLedCurr = millis();
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    if ((outStateTick[i] == 10) && outStates[i] == true)
    {
      writeOledBlank();
    }
    if (outStateTick[i] > 9)
    {
      outStates[i] = false;
    }
  }
}

void timer1Second()
{
  longTickCnt++;
  // Set counter for 10hz to 0
  timer1TickCounter = 0;
}

void printMillis()
{
  Serial.print("M1: ");
  Serial.print(secMillisLedStart[0]);

  Serial.print("M2: ");
  Serial.print(secMillisLedStart[1]);
  Serial.print("M3: ");
  Serial.print(secMillisLedStart[2]);
  Serial.print("M4: ");
  Serial.println(secMillisLedStart[3]);
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
