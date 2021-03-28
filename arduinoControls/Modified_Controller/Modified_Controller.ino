/******************************************************MLink Setup*****************************/
#include "AltSoftSerial.h"

#define RESET       0x69
#define START       0x0A
#define ACTIVATE    0x0D
#define STOP        0x0B
#define DATA        0x14
#define ERROR       0x02
#define dir_ndx     5
#define speed_ndx   6

/****************************************/
/**   Using the MLinkControl library   **/
/**   in addition to blinking an LED   **/
/****************************************/

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

//Receive Messages: Added to capstone
void receiveMessage (uint64_t data[],
                     uint16_t dataSize);
//

// Used to check when there is a callback
bool hasReset = false;
bool hasStarted = false;
bool hasActivated = false;
bool hasSentData = false;

// LED
const uint8_t ledPin = 12;
uint8_t ledState = LOW;

// Timing Parameters
long intervalData = 1000; // Send voltage reading from a potentiometer every second
unsigned long currentMillis;
unsigned long previousMillisLed = 0;
unsigned long previousMillisData = 0;

AltSoftSerial mlink(resetIndication,
                    startIndication,
                    activateIndication,
                    stopIndication,
                    transmitStatusIndication,
                    errorIndication,
                    acknowledgeMessage,
                    //Added to capstone
                    receiveMessage);

void resetIndication (uint8_t flags,
                      uint8_t hwRev,
                      uint16_t hwFault,
                      uint16_t fwMajorRev,
                      uint16_t fwMinorRev,
                      uint16_t fwFlags,
                      uint8_t region)
{
  hasReset = true;
  Serial.println("Reset Indication'");
}

void startIndication (uint8_t outcome,
                      uint32_t serial)
{
  hasStarted = true;
  Serial.println("Start Indication'");
}

void activateIndication ()
{
  hasActivated = true;
  Serial.println("Activate Indication'");
}

void stopIndication (uint8_t reason)
{
  Serial.println("Stop Indication'");
}

void transmitStatusIndication (uint8_t handle,
                               uint8_t outcome,
                               int8_t rssi)
{
  hasSentData = true;
  Serial.println("Transmit Status Indication'");
}

void errorIndication (uint8_t ackId,
                      uint8_t reason)
{
  Serial.println("Error Indication'");
}

void acknowledgeMessage (uint8_t messageId)
{
  mlink.acknowledge (messageId);
  Serial.println("Acknowledgment'");
}


/********************************************Motor Setup*********************************************/
//Motor Connections
#define EnA 10
#define EnB 11
#define In1 3
#define In2 4
#define In3 5
#define In4 6

// The motor speed from the mlink from 0 to 255
int motor_speed;
int movDir = 0;
//data receive stuff

uint64_t receivedBytes;
boolean newData = false;

/*****************************************Void Setup*************************************/
void setup()
{
  Serial.begin(57600);
  mlink.begin (57600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  mlink.resetRequest(); // resetting MLink
  delay(500);
  Serial.print("Just reset");

  // All motor control pins are outputs
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);

  Serial.println("Done Setup");
}


/***************************************Main Loop*********************************************/
void loop() {
  currentMillis = millis();
  sendStart();
  receiveMessage;
  getSpeed();
}


/************************************************************Motor Functions*******************************************/
void goForward()   //run both motors in the same direction
{
  // turn on motor A
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  // set right motor speed
  analogWrite(EnA, motor_speed);
  // turn on motor B
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  // set left motor speed
  analogWrite(EnB, motor_speed);
  Serial.println("Going Forwards");
}

void goBack()   //run both motors in the same direction
{
  // turn on motor A
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  // set right motor speed
  analogWrite(EnA, motor_speed);
  // turn on motor B
  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  // set left motor speed
  analogWrite(EnB, motor_speed);
  Serial.println("Going Backwards");

}
void stay() {
  // now turn off motors
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
  Serial.println("Staying");
}


/************************************************************MLINK Functions*******************************************/
// sending start request to MLink
void sendStart()
{
  if (hasReset && !hasStarted)
  {
    // Please review the user manual for information
    // on startRequest parameters
    mlink.startRequest(0x00, 0x00, 0x00, 0xFF);
    hasStarted = true;
    Serial.print("Just started, and there are '");
    Serial.print(mlink.available());
    Serial.println("' bytes in the buffer");
  }
  else {
  }
}

void receiveMessage (uint64_t data[],
                     uint16_t dataSize)
{
  //  while (mlink.available() > 0 && newData == false) {
  //      rc = mlink.read();
  //      Serial.print(rc);
  //      if (rc != endMarker && receivedBytes[ndx - 1] != pen_endMarker) {
  //        receivedBytes[ndx] = rc;
  //        ndx++;
  //        if (ndx >= numBytes) {
  //          ndx = numBytes - 1;
  //        }
  //      }
  //      e0lse {
  //        ndx = 0;
  //        newData = true;
  //      }
  //    }



  Serial.println(dataSize);

  uint32_t motor_speed_low = data[2] % 0xFFFFFFFF;
  uint32_t  motor_speed_high = (data[2] >> 32) % 0xFFFFFFFF;
  motor_speed = motor_speed_low + (motor_speed_high * 32);
  Serial.println(motor_speed);

  uint32_t movDir_low = data[1] % 0xFFFFFFFF;
  uint32_t  movDir_high = (data[1] >> 32) % 0xFFFFFFFF;
  movDir = movDir_low + (movDir_high * 32);
  Serial.println(movDir);


}



void getSpeed() {
  switch (movDir) {
    case 0:
      motor_speed = 0;
      stay();
      break;
    case 1:
      goForward();
      break;
    case 2:
      goBack();
      break;
    case 3:
      digitalWrite(ledPin, HIGH);
      break;
    case 4:
      digitalWrite(ledPin, LOW);
      break;
  }
}
