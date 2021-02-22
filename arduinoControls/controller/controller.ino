//Servo Library
#include <Servo.h>
//MLink Serial Library
#include <AltSoftSerial.h>

/*----------------------------------------- MLink Setup -------------------------*/
#define RESET     0x69
#define START     0x0A
#define ACTIVATE  0x0D
#define STOP      0x0B
#define DATA      0x14
#define ERROR     0x02

// Reset
void resetIndication (uint8_t flags,
                      uint8_t hwRev,
                      uint16_t hwFault,
                      uint16_t fwMajorRev,
                      uint16_t fwMinorRev,
                      uint16_t fwFlags,
                      uint8_t region);

// Start
void startIndication (uint8_t outcome,
                      uint32_t serial);
void activateIndication ();

// Stop
void stopIndication (uint8_t reason);

// Data
void transmitStatusIndication (uint8_t handle,
                               uint8_t outcome,
                               int8_t rssi);

// Error Checking
void errorIndication (uint8_t ackId,
                      uint8_t reason);

// Acknowledgment
void acknowledgeMessage(uint8_t messageId);

// Used to check when there is a callback
bool hasReset = false;
bool hasStarted = false;
bool hasActivated = false;
bool hasSentData = false;

AltSoftSerial mlink(resetIndication,
                    startIndication,
                    activateIndication,
                    stopIndication,
                    transmitStatusIndication,
                    errorIndication,
                    acknowledgeMessage);

/*----------------------------------------- Data Transfer Setup -------------------------*/
// Timing Parameters
long intervalLed = 2000;  // Blink LED every 2 seconds
long intervalData = 1000; // Send voltage reading from a potentiometer every second
unsigned long currentMillis;
unsigned long previousMillisLed = 0;
unsigned long previousMillisData = 0;

//data sending
#define ini_1 0xA0
#define ini_2 0xA1
#define poll_length 0x0001
#define poll_stat 0x11
#define end_1 0xB0
#define end_2 0xB1

boolean newData = false;
boolean recvInProgress = false; //whether or not data is being recieved

/*----------------------------------------- LED Setup -------------------------*/
const uint8_t ledPin = 13;
uint8_t ledState = LOW;

/*----------------------------------------- Servo Setup -------------------------*/
//pulse widths for esc in milliseconds
const int min_pw = 1000;
const int max_pw = 2000;
int ESC_val;
//Joystick Value
uint16_t joyData;
boolean dir;

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

  //connect and reeset MLink Endpoint
  mlink.begin (57600);
  mlink.resetRequest(); // resetting MLink

  //Led Pin
  pinMode(ledPin, OUTPUT);

  //initiallizing motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  currentMillis = millis();
  BlinkLed();
  sendStart();
  sendData();
  recv_position();
  if (joyData != 0) {
    if (dir) {
      forward(conv_speed(joyData));
    }
    else {
      backward(conv_speed(joyData));
    }
  }
  else {
    stay();
  }
}
