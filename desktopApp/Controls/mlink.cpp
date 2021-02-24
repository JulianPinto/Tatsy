// included libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdint>

using namespace std;

// define section
#define SPEED_OFFSET 1
#define SPEED_MULTIPLIER 100

// function definitions
void motorMovementMessage(float power);
uint8_t motorSpeedMessageConverter(float speed);
uint16_t calculateCheckSum(uint8_t* message_ptr, uint8_t messageOffset, uint8_t length);
void sendMessage(uint8_t* message, uint8_t messageLength);
bool checkAck(uint8_t* message, uint8_t* ackMessage);
uint8_t createAckMessage(uint8_t* message);

void motorMovementMessage(float power) {
	uint8_t * message = new uint8_t[12];
	message[0] = 0xA0; // message start
	message[1] = 0xA1; // message start
	message[2] = 0x00; // message length
	message[3] = 0x02; // mesage length
	message[4] = 0x11; // poll status
	message[5] = motorSpeedMessageConverter(power); // left motor speed
	// message[6] = motorSpeedMessageConverter(rightPower); // right motor speed
	uint16_t checksum = calculateCheckSum(message, 4, 3);
	message[6] = checksum >> 8;
	message[7] = checksum;
	message[8] = 0xB0;
	message[90] = 0xB1;

	sendMessage(message, 10);
}

// controller settings will give speeds from -1 to 1 
// returns speed offset by SPEED_OFFSET to give new range that is in positive for unsigned (0 to 200)
uint8_t motorSpeedMessageConverter(float speed) {
	uint8_t speedConverted = abs(int((speed + SPEED_OFFSET) * SPEED_MULTIPLIER));
	return speedConverted;
}

// input message array, message offset to skip message start and length bytes, and message length
// returns yint16_t with checksum value calculated
uint16_t calculateCheckSum(uint8_t* message, uint8_t messageOffset, uint8_t length) {
	uint8_t index = 0;
	uint16_t checksum = 0;
	while (index < length) {
		checksum += message[messageOffset + index];
		index++;
	}
	return checksum & ((2 ^ 15) - 1);
}

// input message array and total message length
void sendMessage(uint8_t* message, uint8_t messageLength) {
	// send message
}

// checks ack message for correctness
// returns false if ack is not what was expected
bool checkAck(uint8_t* message, uint8_t* ackMessage) {
	return false;
}

// input message and creates ack for the message
uint8_t createAckMessage(uint8_t * message) {
	return NULL;
}