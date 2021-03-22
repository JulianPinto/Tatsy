/*----------------------------------------- MLink functions -------------------------*/
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

// sending start request to MLink
void sendStart()
{
  if (hasReset && !hasStarted)
  {
    // Please review the user manual for information
    // on startRequest parameters
    mlink.startRequest(0x00, 0x00, 0x00, 0xFF);
    hasStarted = true;
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

/*----------------------------------------- Data Receive -------------------------*/
//receiving joystick data
void recv_position() {
  static byte ndx = 0; //current index
  static uint8_t rc; //received character
  static uint8_t received_data[11];
  // if ther'es a serial connection and a dataset isn't currently being sent
  while (mlink.available() > 0 && newData == false) {
    rc = mlink.read();

    //if data is being received
    if (recvInProgress == true) {
      //if the character received isn't the end marker
      if (rc != end_2 && received_data[ndx] != end_1) {
        received_data[ndx] = rc; // add character to character array
        ndx++;
      }
      //terminates and resets string
      else {
        recvInProgress = false;
        ndx = 0;
        newData = true;
        //converts string to joystick value
        //joystickVal = atoi(receivedChars);
        joyData = received_data[5];
      }
    }

    else if (rc == ini_1) {
      //checks to see if the first part of the message start was sent
      received_data[ndx] = rc;
      ndx++;
      if (rc == ini_2) {
        //checks to see if the second part of the message start was sent
        recvInProgress = true;
        received_data[ndx] = rc;
        ndx++;
      }
    }
  }
}



/*----------------------------------------- Motor Functions -------------------------*/
//speed conversion
int conv_speed(int speed_val) {
  static int pwmOutput;
  speed_val = speed_val - 100;

  if (speed_val >= 0) {
    dir = true;
    pwmOutput = map(speed_val, 0, 100, 0 , 255);
    return pwmOutput;
  }
  else {
    dir = false;
    speed_val = speed_val * -1;
    pwmOutput = map(speed_val, 0, 100, 0 , 255);
    return pwmOutput;
  }
}


//move forwards
void forward(int vel) {
  analogWrite(ENA, vel);
  analogWrite(ENB, vel);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN3, HIGH);
}

//move backwards
void backward(int vel) {
  analogWrite(ENA, vel);
  analogWrite(ENB, vel);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);
}

//stop
void stay() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/*----------------------------------------- LED Functions -------------------------*/
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
