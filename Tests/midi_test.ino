#include <MIDI.h>
#include <Bounce2.h>

// Push button start and end pins (only works in sequence)
int button1 = 7;  
int button2 = 8;  
int button3 = 9;

int bounceDelay = 5;

// Push buttons
Bounce Button1 = Bounce();  
Bounce Button2 = Bounce();  
Bounce Button3 = Bounce();

// Button state
int button1State;  
int lastButton1State;  
int button2State;  
int lastButton2State;  
int button3State;  
int lastButton3State;

// LED pins
int led1 = 3;  
int led2 = 4;  
int led3 = 5;

MIDI_CREATE_DEFAULT_INSTANCE(); // Create an instance of MIDI

void setup() {  
  MIDI.begin(8);
  //Serial.begin(115200);

  pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(button3,INPUT);

  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);

  Button1.attach(button1);
  Button1.interval(bounceDelay);
  Button2.attach(button2);
  Button2.interval(bounceDelay);
  Button3.attach(button3);
  Button3.interval(bounceDelay);
}
void loop() {  
  Button1.update();
  Button2.update();
  Button3.update();

  button1State = Button1.read();
  button2State = Button2.read();
  button3State = Button3.read();

  delay(5);

  if (button1State != lastButton1State) {
    if (button1State == LOW) { 
      digitalWrite(led1, HIGH); 
      MIDI.sendControlChange(16,127,1);
      delay(100);
      digitalWrite(led1, LOW);
    } 
    lastButton1State = button1State;
  }

  if (button2State != lastButton2State) {
    if (button2State == LOW) {  
      digitalWrite(led2, HIGH);
      MIDI.sendControlChange(82,127,1);  // Send a Note
      delay(100);
      digitalWrite(led2, LOW);
    } 
    lastButton2State = button2State;
  }

  if (button3State != lastButton3State) {
    if (button3State == LOW) {  
      digitalWrite(led3, HIGH);
      MIDI.sendProgramChange(6, 1);  // Send a Note
      delay(100);
      digitalWrite(led3, LOW);
    } 
    lastButton3State = button3State;
  }
}