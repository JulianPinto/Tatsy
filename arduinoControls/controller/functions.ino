/*----------------------------------------- Data transfer functions -------------------------*/
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
        joystickVal = atoi(receivedChars);
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


//send data
void sendWithStartEndMarkers() {
  char sc; //sent character

  if (recvInProgress == true) {
    
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