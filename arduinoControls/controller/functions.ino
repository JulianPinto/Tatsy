/*----------------------------------------- MLink functions -------------------------*/
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


/*----------------------------------------- Data Receive -------------------------*/
//receiving joystick data
void recvWithStartEndMarkers() {
  static byte ndx = 0; //current index
  char startMarker = '<'; //initiallization marker
  char endMarker = '>'; //end marker
  char rc; //received character

  // if ther'es a serial connection and a dataset isn't currently being sent
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    //if data is being received
    if (recvInProgress == true) {
      //if the character received isn't the end marker
      if (rc != endMarker) {
        receivedChars[ndx] = rc; // add character to character array
        ndx++;
        //subtracts number of available characters in array
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      //terminates and resets string
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
        //converts string to joystick value
        //joystickVal = atoi(receivedChars);
        
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}



/*----------------------------------------- Motor Functions -------------------------*/
//move forwards
void forward() {
  analogWrite(ENA, 128);
  analogWrite(ENB, 128);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN3, HIGH);
}

//move backwards
void backward() {
  analogWrite(ENA, 128);
  analogWrite(ENB, 128);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);
}
