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
int IN1 = 2;  //right motors
int IN2 = 4;
int ENA = 5;
int IN3 = 8;  //left motors
int IN4 = 9;
int ENB = 3;


void setup() {
  //connect and flush serial port
  Serial.begin(19200);
  Serial.flush();

  //initiallizing motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  recvWithStartEndMarkers();
}
