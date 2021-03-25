/* An Alternative Software Serial Library
   http://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
   Copyright (c) 2014 PJRC.COM, LLC, Paul Stoffregen, paul@pjrc.com

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

// Revisions are now tracked on GitHub
// https://github.com/PaulStoffregen/AltSoftSerial
//
// Version 1.2: Support Teensy 3.x
//
// Version 1.1: Improve performance in receiver code
//
// Version 1.0: Initial Release


#include "AltSoftSerial.h"
#include "config/AltSoftSerial_Boards.h"
#include "config/AltSoftSerial_Timers.h"

/****************************************/
/**          Initialization            **/
/****************************************/

// Added to AltSoftSerial
#define CTS_PIN 	  	    7
#define RTS_PIN 	  	    2
#define SOM1		  	    0xA0
#define SOM2		  	    0xA1
#define EOM1		  	    0xB0
#define EOM2		  	    0xB1
#define RESET_REQUEST 	    0x67
#define RESET_INDICATION    0x69
#define START			    0x0A
#define ACTIVATE_INDICATION 0x0D
#define STOP  			    0x0B
#define TRANSMIT_STATUS	    0x14
#define DATA_REQUEST	    0x13
#define POSITIVE_ACK	    0x01
#define NEGATIVE_ACK	    0x02


static enum {SOM_1,
             SOM_2,
             LEN_1,
             LEN_2,
             ID,
             CONTENT,
             CKSUM_1,
             CKSUM_2,
             EOM_1,
             EOM_2
            } myState = SOM_1;
volatile uint8_t msgId;
uint8_t hbCksum;
uint8_t lbCksum;
uint64_t content[64];
uint16_t length;
uint16_t index;
uint32_t cksum;
void (*_reset)(uint8_t flags,
               uint8_t hwRev,
               uint16_t hwFault,
               uint16_t fwMajorRev,
               uint16_t fwMinorRev,
               uint16_t fwFlags,
               uint8_t region);
void (*_start)(uint8_t outcome,
               uint32_t serial);
void (*_activate)();
void (*_stop)(uint8_t reason);
void (*_transmitStatus)(uint8_t handle,
                        uint8_t outcome,
                        int8_t rssi);
void (*_error)(uint8_t ackId,
               uint8_t reason);
void (*_acknowledgement)(uint8_t mesageId);
//
static uint16_t ticks_per_bit = 0;
bool AltSoftSerial::timing_error = false;

static uint8_t rx_state;
static uint8_t rx_byte;
static uint8_t rx_bit = 0;
static uint16_t rx_target;
static uint16_t rx_stop_ticks = 0;
static volatile uint8_t rx_buffer_head;
static volatile uint8_t rx_buffer_tail;
#define RX_BUFFER_SIZE 80
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];

static volatile uint8_t tx_state = 0;
static uint8_t tx_byte;
static uint8_t tx_bit;
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;
#define TX_BUFFER_SIZE 68
static volatile uint8_t tx_buffer[TX_BUFFER_SIZE];


#ifndef INPUT_PULLUP
#define INPUT_PULLUP INPUT
#endif

#define MAX_COUNTS_PER_BIT  6241  // 65536 / 10.5

AltSoftSerial::AltSoftSerial (void (*reset)(uint8_t flags,
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
                              void (*acknowledgement)(uint8_t messageId))
{
  _reset = reset;
  _start = start;
  _activate = activate;
  _stop = stop;
  _transmitStatus = transmitStatus;
  _error = error;
  _acknowledgement = acknowledgement;
}

void AltSoftSerial::init(uint32_t cycles_per_bit)
{
  //Serial.printf("cycles_per_bit = %d\n", cycles_per_bit);
  if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
    CONFIG_TIMER_NOPRESCALE();
  } else {
    cycles_per_bit /= 8;
    //Serial.printf("cycles_per_bit/8 = %d\n", cycles_per_bit);
    if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
      CONFIG_TIMER_PRESCALE_8();
    } else {
#if defined(CONFIG_TIMER_PRESCALE_256)
      cycles_per_bit /= 32;
      //Serial.printf("cycles_per_bit/256 = %d\n", cycles_per_bit);
      if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
        CONFIG_TIMER_PRESCALE_256();
      } else {
        return; // baud rate too low for AltSoftSerial
      }
#elif defined(CONFIG_TIMER_PRESCALE_128)
      cycles_per_bit /= 16;
      //Serial.printf("cycles_per_bit/128 = %d\n", cycles_per_bit);
      if (cycles_per_bit < MAX_COUNTS_PER_BIT) {
        CONFIG_TIMER_PRESCALE_128();
      } else {
        return; // baud rate too low for AltSoftSerial
      }
#else
      return; // baud rate too low for AltSoftSerial
#endif
    }
  }
  // Added to AltSoftSerial
  pinMode(6, OUTPUT); // Debugging pin
  pinMode (CTS_PIN, INPUT);
  pinMode (RTS_PIN, OUTPUT);
  digitalWrite(RTS_PIN, LOW);
  //
  ticks_per_bit = cycles_per_bit;
  rx_stop_ticks = cycles_per_bit * 37 / 4;
  //pinMode(INPUT_CAPTURE_PIN, INPUT_PULLUP);
  digitalWrite(OUTPUT_COMPARE_A_PIN, HIGH);
  pinMode(OUTPUT_COMPARE_A_PIN, OUTPUT);
  rx_state = 0;
  rx_buffer_head = 0;
  rx_buffer_tail = 0;
  tx_state = 0;
  tx_buffer_head = 0;
  tx_buffer_tail = 0;
  ENABLE_INT_INPUT_CAPTURE();
}

void AltSoftSerial::end(void)
{
  DISABLE_INT_COMPARE_B();
  DISABLE_INT_INPUT_CAPTURE();
  flushInput();
  flushOutput();
  DISABLE_INT_COMPARE_A();
  // TODO: restore timer to original settings?
}


/****************************************/
/**           Transmission             **/
/****************************************/
void AltSoftSerial::writeByte(uint8_t b)
{
  uint8_t intr_state, head;

  head = tx_buffer_head + 1;
  if (head >= TX_BUFFER_SIZE) head = 0;
  while (tx_buffer_tail == head) ; // wait until space in buffer
  intr_state = SREG;
  cli();
  if (tx_state) {
    tx_buffer[head] = b;
    tx_buffer_head = head;
  } else {
    tx_state = 1;
    tx_byte = b;
    tx_bit = 0;
    ENABLE_INT_COMPARE_A();
    CONFIG_MATCH_CLEAR();
    SET_COMPARE_A(GET_TIMER_COUNT() + 16);
  }
  SREG = intr_state;
}

ISR(COMPARE_A_INTERRUPT)
{
  uint8_t state, byte, bit, head, tail;
  uint16_t target;

  state = tx_state;
  byte = tx_byte;
  target = GET_COMPARE_A();
  while (state < 10) {
    target += ticks_per_bit;
    if (state < 9)
      bit = byte & 1;
    else
      bit = 1; // stopbit
    byte >>= 1;
    state++;
    if (bit != tx_bit) {
      if (bit) {
        CONFIG_MATCH_SET();
      } else {
        CONFIG_MATCH_CLEAR();
      }
      SET_COMPARE_A(target);
      tx_bit = bit;
      tx_byte = byte;
      tx_state = state;
      return;
    }
  }
  head = tx_buffer_head;
  tail = tx_buffer_tail;
  if (head == tail) {
    if (state == 10) {
      // Wait for final stop bit to finish
      tx_state = 11;
      SET_COMPARE_A(target + ticks_per_bit);
    } else {
      tx_state = 0;
      CONFIG_MATCH_NORMAL();
      DISABLE_INT_COMPARE_A();
    }
  } else {
    if (++tail >= TX_BUFFER_SIZE) tail = 0;
    tx_buffer_tail = tail;
    tx_byte = tx_buffer[tail];
    tx_bit = 0;
    CONFIG_MATCH_CLEAR();
    if (state == 10)
      SET_COMPARE_A(target + ticks_per_bit);
    else
      SET_COMPARE_A(GET_TIMER_COUNT() + 16);
    tx_state = 1;
  }
}

void AltSoftSerial::flushOutput(void)
{
  while (tx_state) /* wait */ ;
}

/****************************************/
/**            Reception               **/
/****************************************/

// Added to AltSoftSerial so I can use the read
// function in my state machine function: readBytes().
int myRead()
{
  uint8_t head, tail, out;
  head = rx_buffer_head;
  tail = rx_buffer_tail;
  if (head == tail) return -1;
  if (++tail >= RX_BUFFER_SIZE) tail = 0;
  out = rx_buffer[tail];
  rx_buffer_tail = tail;
  return out;
}

// Added to AltSoftSerial so I can use
// the available function
uint8_t myAvailable()
{
  uint8_t head, tail;

  head = rx_buffer_head;
  tail = rx_buffer_tail;
  if (head >= tail) return head - tail;
  return RX_BUFFER_SIZE + head - tail;
}

void getByte()
{
  do
  {
    readByte(myRead());

  } while (myAvailable() > 0);
}

ISR(CAPTURE_INTERRUPT)
{
  uint8_t state, bit, head;
  uint16_t capture, target;
  uint16_t offset, offset_overflow;

  capture = GET_INPUT_CAPTURE();
  bit = rx_bit;
  if (bit) {
    CONFIG_CAPTURE_FALLING_EDGE();
    rx_bit = 0;
  } else {
    CONFIG_CAPTURE_RISING_EDGE();
    rx_bit = 0x80;
  }
  state = rx_state;
  if (state == 0) {
    if (!bit) {
      uint16_t end = capture + rx_stop_ticks;
      SET_COMPARE_B(end);
      ENABLE_INT_COMPARE_B();
      rx_target = capture + ticks_per_bit + ticks_per_bit / 2;
      rx_state = 1;
    }
  } else {
    target = rx_target;
    offset_overflow = 65535 - ticks_per_bit;
    while (1) {
      offset = capture - target;
      if (offset > offset_overflow) break;
      rx_byte = (rx_byte >> 1) | rx_bit;
      target += ticks_per_bit;
      state++;
      if (state >= 9) {
        DISABLE_INT_COMPARE_B();
        head = rx_buffer_head + 1;
        if (head >= RX_BUFFER_SIZE) head = 0;
        if (head != rx_buffer_tail) {
          rx_buffer[head] = rx_byte;
          rx_buffer_head = head;

          // Added to AltSoftSerial
          getByte();
        }
        CONFIG_CAPTURE_FALLING_EDGE();
        rx_bit = 0;
        rx_state = 0;
        return;
      }
    }
    rx_target = target;
    rx_state = state;
  }
  //if (GET_TIMER_COUNT() - capture > ticks_per_bit) AltSoftSerial::timing_error = true;
}

ISR(COMPARE_B_INTERRUPT)
{
  uint8_t head, state, bit;

  DISABLE_INT_COMPARE_B();
  CONFIG_CAPTURE_FALLING_EDGE();
  state = rx_state;
  bit = rx_bit ^ 0x80;
  while (state < 9) {
    rx_byte = (rx_byte >> 1) | bit;
    state++;
  }
  head = rx_buffer_head + 1;
  if (head >= RX_BUFFER_SIZE) head = 0;
  if (head != rx_buffer_tail) {
    rx_buffer[head] = rx_byte;
    rx_buffer_head = head;

    // Added to AltSoftSerial
    getByte();
  }
  rx_state = 0;
  CONFIG_CAPTURE_FALLING_EDGE();
  rx_bit = 0;
}

void readByte (uint8_t myByte)
{
  switch (myState)
  {
    case SOM_1:
      if (myByte == SOM1)
      {
        myState = SOM_2;
      }
      else
      {
        myState = SOM_2;
      }
      break;

    case SOM_2:
      if (myByte == SOM2)
      {
        myState = LEN_1;
      }
      else
      {
        myState = SOM_1;
      }
      break;

    case LEN_1:
      length = myByte << 8;
      myState = LEN_2;
      break;

    case LEN_2:
      length = length + myByte;
      myState = ID;
      break;

    case ID:
      msgId = myByte;
      cksum = myByte;
      index = 0;
      myState = CONTENT;
      break;

    case CONTENT:
      if (index < length)
      {
        content[index] = myByte;
        index++;
        cksum = cksum + myByte;
      }
      if (index == length)
      {
        myState = CKSUM_1;
      }
      break;

    case CKSUM_1:
      cksum = cksum & (0x7FFF);
      hbCksum = highByte (cksum);
      if (myByte == hbCksum)
      {
        myState = CKSUM_2;
      }
      else
      {
        myState = SOM_1;
      }
      break;

    case CKSUM_2:
      lbCksum = lowByte (cksum);
      if (myByte == lbCksum)
      {
        myState = EOM_1;
      }
      else
      {
        myState = SOM_1;
      }
      break;

    case EOM_1:

      if (myByte == EOM1)
      {
        myState = EOM_2;
      }
      else
      {
        myState = SOM_1;
      }
      break;

    case EOM_2:
      if (myByte == EOM2)
      {
        myState = SOM_1;
        processPackets();
      }
      else
      {
        myState = SOM_1;
      }
      break;
  }
}

void processPackets()
{
  if (msgId == RESET_INDICATION)
  {
    _acknowledgement(msgId);
    _reset(content[0], 						 // flags
           content[1], 						 // hwRev
           (content[2] << 8) | (content[3]), // hwFault
           (content[4] << 8) | (content[5]), // fwMajorRev
           (content[6] << 8) | (content[7]), // fwMinorRev
           (content[8] << 8) | (content[9]), // fwFlags
           content[10]);					 // region
  }
  if (msgId == START)
  {
    _acknowledgement(msgId);
    _start(content[0],								 // outcome
           (content[1] << 24) | (content[2] << 16) | // serial
           (content[3] << 8) | (content[4]));

  }
  if (msgId == ACTIVATE_INDICATION)
  {
    _acknowledgement(msgId);
    _activate();
  }
  if (msgId == STOP)
  {
    _acknowledgement(msgId);
    _stop(content[0]); // reason
  }
  if (msgId == TRANSMIT_STATUS)
  {
    _acknowledgement(msgId);
    _transmitStatus(content[0],  // handle
                    content[1],  // outcome
                    content[2]); // rssi
  }
  if (msgId == NEGATIVE_ACK)
  {
    _error(content[0],  // ackId
           content[1]); // reason
  }
  //receiving data from the base
  /*if(msgId == 0x13)
    {
  	uint8_t message;
  	message = content[1];
  	Serial.println(message);
    }*/
}


int AltSoftSerial::peek(void)
{
  uint8_t head, tail;

  head = rx_buffer_head;
  tail = rx_buffer_tail;
  if (head == tail) return -1;
  if (++tail >= RX_BUFFER_SIZE) tail = 0;
  return rx_buffer[tail];
}

int AltSoftSerial::available(void)
{
  uint8_t head, tail;

  head = rx_buffer_head;
  tail = rx_buffer_tail;
  if (head >= tail) return head - tail;
  return RX_BUFFER_SIZE + head - tail;
}

int AltSoftSerial::read(void)
{
  uint8_t head, tail, out;
  head = rx_buffer_head;
  tail = rx_buffer_tail;
  if (head == tail) return -1;
  if (++tail >= RX_BUFFER_SIZE) tail = 0;
  out = rx_buffer[tail];
  rx_buffer_tail = tail;
  return out;
}

int AltSoftSerial::availableForWrite(void)
{
  uint8_t head, tail;
  head = tx_buffer_head;
  tail = tx_buffer_tail;

  if (tail > head) return tail - head;
  return TX_BUFFER_SIZE + tail - head;
};

void AltSoftSerial::flushInput(void)
{
  rx_buffer_head = rx_buffer_tail;
}

void AltSoftSerial::resetRequest()
{
  uint8_t reset[] = {SOM1,
                     SOM2,
                     0x00,
                     0x00,
                     RESET_REQUEST,
                     0x00,
                     0x67,
                     EOM1,
                     EOM2
                    };
  while (digitalRead (CTS_PIN) == HIGH);
  if (digitalRead (CTS_PIN) == LOW)
  {
    digitalWrite(RTS_PIN, HIGH);
    write(0x01); //Used to ready the MLink to receive packets
    write(reset, sizeof(reset));
    digitalWrite(RTS_PIN, LOW);
  }
}

void AltSoftSerial::startRequest(uint8_t startFlags,
                                 uint8_t pollMatchOffset,
                                 uint8_t pollMatchMask,
                                 uint8_t hopId)
{
  uint8_t temp = 0;
  uint8_t start[] =  {SOM1,
                      SOM2,
                      0x00,
                      0x04,
                      START,
                      startFlags,
                      pollMatchOffset,
                      pollMatchMask,
                      hopId,
                      temp,
                      temp,
                      EOM1,
                      EOM2
                     };

  cksum (start, sizeof(start) - 1);

  while (digitalRead(CTS_PIN) == HIGH);
  if (digitalRead (CTS_PIN) == LOW)
  {
    digitalWrite(RTS_PIN, HIGH);
    write(start, sizeof(start));
    digitalWrite(RTS_PIN, LOW);
  }
}

void AltSoftSerial::stopRequest (uint8_t stopFlags)
{
  uint8_t temp = 0;
  uint8_t stop[] = {SOM1,
                    SOM2,
                    0x00,
                    0x01,
                    STOP,
                    stopFlags,
                    temp,
                    temp,
                    EOM1,
                    EOM2
                   };

  cksum (stop, sizeof(stop) - 1);

  while (digitalRead(CTS_PIN) == HIGH);
  if (digitalRead (CTS_PIN) == LOW)
  {
    digitalWrite(RTS_PIN, HIGH);
    write(stop, sizeof(stop));
    digitalWrite(RTS_PIN, LOW);
  }
}

void AltSoftSerial::dataRequest(uint16_t analogData,
                                uint8_t dataFlags,
                                uint32_t tmo)
{
  uint8_t tmoBytes[4];
  uint8_t temp = 0;
  tmoBytes[3] = tmo & 0xFF;
  tmoBytes[2] = (tmo >> 8) & 0xFF;
  tmoBytes[1] = (tmo >> 16) & 0xFF;
  tmoBytes[0] = (tmo >> 24) & 0xFF;

  uint8_t data[] = {SOM1,
                    SOM2,
                    0x00,
                    0x07,
                    DATA_REQUEST,
                    dataFlags,
                    tmoBytes[0],
                    tmoBytes[1],
                    tmoBytes[2],
                    tmoBytes[3],
                    highByte(analogData),
                    lowByte(analogData),
                    temp,
                    temp,
                    EOM1,
                    EOM2
                   };

  cksum (data, sizeof(data) - 1);

  while (digitalRead (CTS_PIN) == HIGH);
  if (digitalRead (CTS_PIN) == LOW)
  {
    digitalWrite (RTS_PIN, HIGH);
    write (data, sizeof(data));
    digitalWrite (RTS_PIN, LOW);

  }
}

void AltSoftSerial::acknowledge(uint8_t ackId)
{
  uint8_t temp = 0;
  uint8_t ack[] = {SOM1,
                   SOM2,
                   0x00,
                   0x01,
                   POSITIVE_ACK,
                   ackId,
                   temp,
                   temp,
                   EOM1,
                   EOM2
                  };

  cksum (ack, sizeof(ack) - 1);

  digitalWrite(RTS_PIN, HIGH);
  write(ack, sizeof(ack));
  digitalWrite(RTS_PIN, LOW);
}

void AltSoftSerial::cksum(uint8_t packet[], uint8_t packetLength)
{
  uint16_t sum = 0;
  for (int i = 4; i <= packetLength - 4; i++)
  {
    sum += packet[i] & (0x7FFF);
  }
  packet[packetLength - 3] = highByte(sum);
  packet[packetLength - 2] = lowByte(sum);
}

#ifdef ALTSS_USE_FTM0
void ftm0_isr(void)
{
  uint32_t flags = FTM0_STATUS;
  FTM0_STATUS = 0;
  if (flags & (1 << 0) && (FTM0_C0SC & 0x40)) altss_compare_b_interrupt();
  if (flags & (1 << 5)) altss_capture_interrupt();
  if (flags & (1 << 6) && (FTM0_C6SC & 0x40)) altss_compare_a_interrupt();
}
#endif
