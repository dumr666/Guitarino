/*
 * Midi pedal board, needs software installed for emulating midi
 * 
 * Author: Denis Leskovar
 * Email: dumr666@gmail.com
 * 
 * Known bugs:
 * - Dont know what happen when encoder position comes to -32768 and goes to +
 *        (dont want to test this, please do if you want)
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
#define doL1 16
#define doL2 14
#define doL3 15
#define doL4 10
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

int enc1PosOld = 0;
int enc1Pos = 0;
uint8_t encTimerTick = 0;
bool encOperation = false;

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
int oledBlankTick = 0;
ioDlib pin;

// Button array
bool operationProgress = false;
bool inStates[] = {false, false, false, false};
bool inOldStates[] = {false, false, false, false};
bool inChangedStates[] = {false, false, false, false};
uint8_t inArrayLength = (sizeof(inChangedStates) / sizeof(inChangedStates[0]));

// LED array
uint8_t outStates[] = {false, false, false, false};
uint8_t outStateTick[] = {0, 0, 0, 0};
uint8_t outLEDs[] = {doL1, doL2, doL3, doL4};

//MIDI specific stuffW
uint8_t ccCommands[] = {111, 81, 3, 4};
int ccType[] = {1, 1, 2, 2};
uint8_t encCCComands[] = {85, 86};
int encCCType[] = {1, 1};

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
  oled.print("DOOM_er");
  oled.setCursor(0, 4);
  oled.print("GUITARINO!");
  delay(2000);
  oled.clear();
  writeOledBlank();
  oledBlankTick = 11;
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
      inChangedStates[i] = inStates[i];
    }
    if (inStates[i] == true)
    {
      oledBlankTick = 0;
      outStateTick[i] = 0;
      outStates[i] = true;
    }
  }
  // Read encoder
  enc1PosOld = enc1Pos;
  readEncoder();
}

void readEncoder()
{
  enc1Pos = enc1.read();
  enc1Pos /= 4;
  uint8_t opNum = 5;
  if (!(enc1Pos == enc1PosOld))
  {
    if (enc1Pos > enc1PosOld)
    {
      opNum = 1;
    }
    else
    {
      opNum = 0;
    }
  }
  if (opNum < 5)
  {
    sendMidiCommand(encCCComands[opNum], encCCType[opNum]);
    oledBlankTick = 0;
    encTimerTick = 0;
    encOperation = true;
    outStates[3] = true;
    outStateTick[3] = 0;
    opNum = 5;
  }
}

void countStateTick()
{
  if (oledBlankTick < 11)
  {
    oledBlankTick += 1;
  }

  if (encOperation == true)
  {
    encTimerTick += 1;
  }
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
      sendMidiCommand(ccCommands[i], ccType[i]);
    }
  }
}

void lightLED()
{ 
  for (uint8_t i = 0; i < inArrayLength; i++)
  {
    digitalWrite(outLEDs[i], outStates[i]);
  }
}

// Finish stuff after one second
void oneSecond()
{
  if (oledBlankTick == 10)
  {
    writeOledBlank();
    oledBlankTick = 11;
  }

  if ((encTimerTick > 9) && (encOperation == true))
  {
    encOperation = false;
  }

  for (uint8_t i = 0; i < inArrayLength; i++)
  {
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
