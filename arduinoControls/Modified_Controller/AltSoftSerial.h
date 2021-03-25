/* An Alternative Software Serial Library
 * http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
 * Copyright (c) 2014 PJRC.COM, LLC, Paul Stoffregen, paul@pjrc.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * Project: Arduino Library meant to control the MLink endpoint
 * 
 * Creation Date: July 1, 2020
 */

#ifndef AltSoftSerial_h
#define AltSoftSerial_h

#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#if defined(__arm__) && defined(CORE_TEENSY)
#define ALTSS_BASE_FREQ F_BUS
#else
#define ALTSS_BASE_FREQ F_CPU
#endif

class AltSoftSerial : public Stream
{
public:
	/**
	 * @brief sends a reset reset request
	 * 		  Added to AltSoftSerial
	 */
	void resetRequest();

	/**
	 * @brief sends a start request
	 * 		  called by the application
	 * 		  added to AltSoftSerial
	 * @param startFlags sets the flags for the start request
	 * 		  0: Mode
	 * 		  1: hopId
	 * @param pollMatchOffset set the byte offset
	 * 		  Range: 0 to 255
	 * @param pollMatchMask set the bit mask
	 * 		  Range: 0 to 255
	 * @param hopId ignored if mode flag is set
	 * 		  23: North America
	 * 		  11: Australia and New Zealand
	 */ 
	void startRequest (uint8_t startFlags,
					   uint8_t pollMatchOffset,
					   uint8_t pollMatchMask,
					   uint8_t hopId);

	/**
	 * @brief sends a stop request
	 * 		  called by the application
	 * 		  added to AltSoftSerial
	 * @param stopFlags sets the flags for the stop request
	 * 		  0: retain all outstanding dataReqs
	 * 			 retain any in-progress dataInd
	 * 	 	  1: remove all outstanding dataReqs
	 * 			 remove any in-progress dataInd
	 */ 
	void stopRequest (uint8_t stopFlags);
	/**
	 * @brief sends a data request
	 * 		  called by the application
	 * 		  added to AltSoftSerial
	 * @param analogData analog data collected by the user
	 * @param dataFlags set the flags for the data request
	 * 		  Range: 0 to 3
	 * 		  0: Demand Poll
	 * 		  1: Replace Data
	 * @param tmo set the timeout in seconds
	 */ 
	void dataRequest (uint16_t analogData,
					  uint8_t dataFlags,
					  uint32_t tmo);

	/**
	 * @brief calculates the check sum
	 * 		  called by the library
	 * 		  added to AltSoftSerial
	 * @param packet[] a packet is passed by 
	 * 		  reference to this function
	 * @param packetLength the length of the packet
	 */ 			
	void cksum (uint8_t packet[], 
			    uint8_t packetLength);

	/**
	 * @brief sends a positive acknowledgment packet
	 * 		  called by the application
	 * 		  added to AltSoftSerial
	 * @param ackId checks the message ID of the
	 * 		  packet to be acknowledged
	 */ 
	void acknowledge(uint8_t ackId);

	/**
	 * @brief application can access the library
	 * 		  modified the parameters from AltSoftSerial
	 * @param (*reset)(...) points to the resetIndication 
	 * 		  function in the application
	 * 		  called when a reset packet is detected
	 * 		  passes parameters to the application
	 * @param (*start)(...) points to the startIndication
	 * 		  function in the application	
	 * 		  called when a start packet is detected
	 * 		  passes parameters to the application
	 * @param (*activate)() points to the activateIndication
	 * 		  function in the application
	 * 		  called when an activate packet is detected
	 * @param (*stop)(...) points to the stopIndication
	 * 		  function in the application
	 * 		  called when a stop packet is detected
	 * 		  passes parameters to the application
	 * @param (*transmitStatus)(...) points to the transmitStatusIndication 
	 * 		  function in the application
	 * 		  called when a transmit status packet is detected
	 * 		  passes parameters to the application
	 * @param (*error)(...) points to the errorIndication 
	 * 		  function in the application
	 * 		  called when a negative acknowledgment packet
	 * 		  is detected
	 * 		  passes parameters to the application
	 * @param (*acknowledgment)(...) points to the acknowledgementMessage
	 * 		  function in the application
	 * 		  called when packet has been successfully detected
	 * 		  passes parameters to the application
	 */ 
	AltSoftSerial(void (*reset)(uint8_t flags,
								uint8_t hwRev,
								uint16_t hwFault,
								uint16_t fwMajorRev,
								uint16_t fwMinorRev,
								uint16_t fwFlags,
								uint8_t region),
				  void (*start)(uint8_t outcome,
				  				uint32_t serial),
				  void (*activate)(),
				  void (*stop)(uint8_t reason),
				  void (*transmitStatus)(uint8_t handle,
				  						 uint8_t outcome,
										 int8_t rssi),
				  void (*error)(uint8_t ackId,
				  				uint8_t reason),
				  void (*acknowledgement)(uint8_t messageId));
	/**
	 * @brief deconstructor
	 */ 			  
	~AltSoftSerial() { end(); }

	/**
	 * @brief sets the data rate (baud)
	 * @param baud speed in bits per second
	 * 		  ALTSS_BASE_FREQ = 16000000
	 */ 
	static void begin(uint32_t baud) { init((ALTSS_BASE_FREQ + baud / 2) / baud); }

	/**
	 * @brief disables the serial communcation port
	 */ 
	static void end();

	/**
	 * @brief returns the next byte of incoming serial
	 * 		  data without removing it from the buffer
	 * @return the first byte of incoming serial data
	 * 		   if no data available returns -1
	 */ 
	int peek();

	/**
	 * @brief reads one byte from the receive buffer
	 * 		  in the serial communication port
	 * @return data, if no data available returns -1
	 */ 
	int read();

	/**
	 * @brief gets the number of bytes available for
	 * 		  reading from the serial port
	 * @return the number of bytes available in the buffer
	 * 		   returns 0 when no data available
	 */ 
	int available();

	/**
	 * @brief gets the number of bytes that can be
	 * 		  written before blocking happens
	 * @return number of bytes available
	 */ 
	int availableForWrite();
#if ARDUINO >= 100
	size_t write(uint8_t byte) { writeByte(byte); return 1; }
	void flush() { flushOutput(); }
#else

	/**
	 * @brief writes a character string or data to 
	 * 	 	  the serial communications port
	 * @param byte data to be written to serial port
	 * 		  str: a string to send as a series of bytes
	 * 		  buf: an array(pointer) to send data
	 * 		  len: length of the output(written) data
	 */ 
	void write(uint8_t byte) { writeByte(byte); }

	/**
	 * @brief waits until the send buffer of the 
	 * 		  serial communicaions port is empty
	 */ 
	void flush() { flushInput(); }
#endif
	using Print::write;

	/**
	 * @brief waits until the send buffer of the
	 * 		  serial communications port is empty
	 */ 
	static void flushInput();

	/**
	 * @brief waits for the data on the 
	 * 		  output buffer to be sent
	 */ 
	static void flushOutput();

	/**
	 * @brief for drop-in compatibility with 
	 * 		  NewSoftSerial, rxPin & txPin ignored
	 */ 
	AltSoftSerial(uint8_t rxPin, uint8_t txPin, bool inverse = false) { }

	/**
	 * @brief enables the selected software serial
	 * 		  port to listen at a time.
	 * @return false
	 */ 
	bool listen() { return false; }

	/**
	 * @brief tests if the requested software
	 * 		  serial port is actively listening
	 * @return true 
	 */
	bool isListening() { return true; }

	/**
	 * @brief part of the original AltSoftSerial
	 * 		  not used
	 */ 
	bool overflow() { bool r = timing_error; timing_error = false; return r; }

	/**
	 * @brief part of the original AltSoftSerial
	 */
	static int library_version() { return 1; }

	/**
	 * @brief part of the original AltSoftSerial
	 * 		  not used
	 */ 
	static void enable_timer0(bool enable) { }

	/**
	 * @brief part of the original AltSoftSerial
	 */ 
	static bool timing_error;
private:
	/**
	 * @brief this is the same function as begin
	 */ 
	static void init(uint32_t cycles_per_bit);

	/**
	 * @brief this is the same function as write
	 */ 
	static void writeByte(uint8_t byte);
};

/**
 * @brief gets the number of bytes
 * 		  available on the serial port
 * @return the number of bytes available
 * 		   otherwise returns 0
 */ 
uint8_t myAvailable();

/**
 * @brief gets byte once it is available
 */ 
void getByte();

/**
 * @brief state machine that checks the incoming 
 * 		  packets
 * @param myByte gets a byte when it is available
 */ 
void readByte(uint8_t myByte);

/**
 * @brief based on the packet received will 
 * 		  make a callback to the application
 */ 
void processPackets();

/**
 * @brief reads one byte of data from the receive 
 * 		  buffer in the serial communications port
 * @return data
 */ 
int myRead();

/**
 * @brief determines the packet received
 * 		  stores data in an array
 * @param contentId selects the packet contentId
 * @return the content from the selected packet
 */ 
#endif
