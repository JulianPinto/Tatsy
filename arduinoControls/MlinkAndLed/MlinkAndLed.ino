#include <AltSoftSerial.h>

#define RESET     0x69
#define START     0x0A
#define ACTIVATE  0x0D
#define STOP      0x0B
#define DATA      0x14
#define ERROR     0x02

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
long intervalLed = 2000;  // Blink LED every 2 seconds
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
}

void startIndication (uint8_t outcome,
                      uint32_t serial)
{
  hasStarted = true;
}

void activateIndication ()
{
  hasActivated = true;
}

void stopIndication (uint8_t reason)
{

}

void transmitStatusIndication (uint8_t handle,
                               uint8_t outcome,
                               int8_t rssi)
{
  hasSentData = true;
}

void errorIndication (uint8_t ackId,
                      uint8_t reason)
{

}

void acknowledgeMessage (uint8_t messageId)
{
  mlink.acknowledge (messageId);
}

void setup() 
{
  Serial.begin(57600);
  mlink.begin (57600);
  pinMode(ledPin, OUTPUT);
  mlink.resetRequest(); // resetting MLink
  Serial.println("                ");
  Serial.println("Done Setup");
}

void loop() 
{
  currentMillis = millis();
  BlinkLed();
  sendStart();
  sendData();
}

// Blinking the LED
void BlinkLed()
{
  if (currentMillis - previousMillisLed >= intervalLed)
  {
    previousMillisLed = currentMillis;
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
     digitalWrite (ledPin, ledState);
  }
}

// sending start request to MLink
void sendStart()
{
  if (hasReset && !hasStarted)
  {
    // Please review the user manual for information 
    // on startRequest parameters
    mlink.startRequest(0x00, 0x00, 0x00, 0xFF);
    hasStarted = true;
    Serial.println("reset and started");
  }
  else{
    Serial.println("Not reset");
  }
}

// sending voltage values from a potentiometer
void sendData()
{
  if (currentMillis - previousMillisData >= intervalData)
  {
    previousMillisData = currentMillis;
    if (hasActivated)
    {
      uint16_t analogData = analogRead(A1);
      mlink.dataRequest(analogData, 0x02, 0);
    }
  }
}
