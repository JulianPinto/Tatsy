#include <Servo.h>
//pulse widths for esc in milliseconds
const int min_pw = 1000;
const int max_pw = 2000;
int ESC_val;

//joystick range;
const int min_joy = 0;
const int max_joy = 1023;
int joystickVal;

//data recieving
const byte numChars = 32;
char receivedChars[numChars];//chars received
char sentChars[numChars];//chars sent

//data sending
boolean newData = false;
boolean recvInProgress = false; //whether or not data is being recieved

//motor setup
#define ENA 10
#define ENB 5
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6

void setup() {
  //connect and flush serial port
  Serial.begin(57600);
  Serial.flush();

  //initiallizing motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  recvWithStartEndMarkers();
}
