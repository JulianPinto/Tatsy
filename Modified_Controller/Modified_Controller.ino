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
                    acknowledgeMessage);

void resetIndication (uint8_t flags,
                      uint8_t hwRev,
                      uint16_t hwFault,
                      uint16_t fwMajorRev,
                      uint16_t fwMinorRev,
                      uint16_t fwFlags,
                      uint8_t region)
{
  hasReset = true;
  Serial.print("Reset Indication'");
}

void startIndication (uint8_t outcome,
                      uint32_t serial)
{
  hasStarted = true;
  Serial.print("Start Indication'");
}

void activateIndication ()
{
  hasActivated = true;
  Serial.print("Activate Indication'");
}

void stopIndication (uint8_t reason)
{
  Serial.print("Stop Indication'");
}

void transmitStatusIndication (uint8_t handle,
                               uint8_t outcome,
                               int8_t rssi)
{
  hasSentData = true;
  Serial.print("Transmit Status Indication'");
}

void errorIndication (uint8_t ackId,
                      uint8_t reason)
{
  Serial.print("Error Indication'");
}

void acknowledgeMessage (uint8_t messageId)
{
  mlink.acknowledge (messageId);
  Serial.print("Acknowledgment'");
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

//data receive stuff
const byte numBytes = 16;
char receivedBytes[numBytes];
boolean newData = false;

/*****************************************Void Setup*************************************/
void setup()
{
  Serial.begin(57600);
  mlink.begin (57600);
  pinMode(ledPin, OUTPUT);
  mlink.resetRequest(); // resetting MLink
  delay(500);
  Serial.print("Just reset and there are '");
  Serial.print(mlink.available());
  Serial.println("' bytes in the buffer");
  Serial.println("                ");


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
  recvData();
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

}
void stay() {
  // now turn off motors
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
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


//Receive data from MLINK
void recvData() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  uint8_t startMarker = 0xA0;
  uint8_t endMarker = 0xB1;
  uint8_t pen_endMarker = 0xB0;
  uint8_t rc;
  Serial.print("There are '");
  Serial.print(mlink.available());
  Serial.println("' bytes in the buffer");
  while (mlink.available() > 0 && newData == false) {
    rc = mlink.read();
    if (rc != endMarker && receivedBytes[ndx - 1] != pen_endMarker) {
      receivedBytes[ndx] = rc;
      ndx++;
      if (ndx >= numBytes) {
        ndx = numBytes - 1;
      }
    }
    else {
      ndx = 0;
      newData = true;
    }
  }


  //  if (currentMillis - previousMillisData >= intervalData) {
  //    if (hasActivated) {
  //      Serial.print("There are '");
  //      Serial.print(mlink.available());
  //      Serial.println("' bytes in the buffer");
  //      rc = mlink.read();
  //      while (recvInProgress == true) {
  //        Serial.println("Data is currently being received");
  //        if (rc != endMarker && receivedBytes[ndx - 1] != pen_endMarker) {
  //          receivedBytes[ndx] = rc;
  //          //          Serial.print("At ndx '");
  //          //          Serial.print(ndx);
  //          //          Serial.print("' the recieved byte is '");
  //          //          Serial.print(rc);
  //          //          Serial.println("'");
  //          ndx++;
  //          if (ndx >= numBytes) {
  //            ndx = numBytes - 1;
  //          }
  //        }
  //        else {
  //          receivedBytes[ndx] = rc; // terminate the string
  //          Serial.println("Data transmission is over");
  //          //          Serial.print("'");
  //          //          Serial.print(receivedBytes);
  //          //          Serial.println("' was trasnmitted");
  //          recvInProgress = false;
  //          ndx = 0;
  //          newData = true;
  //        }
  //        //Serial.println(rc);
  //      }
  //    }
  //    if (rc == startMarker) {
  //      Serial.println("Data transmission has started");
  //      recvInProgress = true;
  //      receivedBytes[ndx] = rc;
  //      //      Serial.print("At ndx '");
  //      //      Serial.print(ndx);
  //      //      Serial.print("' the recieved byte is '");
  //      //      Serial.print(rc);
  //      //      Serial.println("'");
  //      ndx++;
  //
  //      //Serial.println(rc);
  //    }
  //  }


  //    previousMillisData = currentMillis;
  //    if (hasActivated)
  //    {
  //      uint8_t controllerData;
  //      controllerData=mlink.read();
  //      Serial.println(controllerData);
  //    }
  //  }


}

void getSpeed() {
  if (newData == true) {
    switch (receivedBytes[dir_ndx]) {
      case 0:
        motor_speed = receivedBytes[speed_ndx];
        stay();
        break;
      case 1:
        motor_speed = receivedBytes[speed_ndx];
        goForward();
        break;
      case 2:
        motor_speed = receivedBytes[speed_ndx];
        goBack();
        break;
    }
    newData = false;
    previousMillisData = currentMillis;
  }
}
