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
byte receivedBytes[numBytes];
char command;
boolean newData = false;

// LED
const uint8_t ledPin = 12;



/*****************************************Void Setup*************************************/
void setup()
{
  Serial.begin(57600);
  Serial.flush();
  delay(500);
  Serial.println("Connection Innitialized\n");
  pinMode(ledPin, OUTPUT);

  // All motor control pins are outputs
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);

  Serial.println("Done Setup\n");
}


/***************************************Main Loop*********************************************/
void loop() {
  if (Serial.available())//checks if there is a serial connection
  {
    command = (Serial.read()); //reads serial port

    //moves the rover forward
    switch (command) {
      case 'F':

        Serial.write("Forwards\n"); //sends whats happening back to the python program
        goForward();
        break;
      //moves the rover back
      case 'B':
        Serial.write("Backwards\n"); //sends whats happening back to the python program
        goBack();
        break;

      //Stops the rover from moving
      case 'S':
        Serial.write("Stop\n"); //sends whats happening back to the python program
        stay();
        break;
      //Turns on the Lights
      case 'H':
        Serial.write("Lights On\n"); //sends whats happening back to the python program
        digitalWrite (ledPin, HIGH);
        break;
      //Turns off the Lights
      case 'L':
        Serial.write("Lights Off\n");
        digitalWrite (ledPin, LOW);
        break;
      //Ends the serial connection
      case 'E':
        Serial.write("Ending Connction\n");
        Serial.end();
        break;
    }
  }
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
