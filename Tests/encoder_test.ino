/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder enc1(9, 8);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  pinMode(A1, INPUT_PULLUP);
  Serial.println("TwoKnobs Encoder Test:");
  pinMode(13, OUTPUT);
} 

unsigned long encPosOld  = 0;

void loop() {
  int sensorVal = digitalRead(A1);
  unsigned long encPos;
  encPos = enc1.read();
  encPos /= 4;
  if (encPos != encPosOld) {
    Serial.print("Left = ");
    Serial.print(encPos);
    Serial.println();
    encPosOld = encPos;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (sensorVal == HIGH) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
  }
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    enc1.write(0);
  }
}