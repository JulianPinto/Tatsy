import serial
from serial.serialutil import SerialException, SerialTimeoutException

class MlinkCommunication:
    def __init__(self, port, baudRate = 57600, timeout = 1):
        try:
            ser = serial.Serial(port, rtscts=True)
            if ser.is_open:
                ser.close()
                
            self.ser = serial.Serial(port, baudRate, timeout = timeout, rtscts = True)
            self.ser.rts=False
            print("connection is open")
        except SerialException:
            print("Port", port, "not opened")



    def calculateCheckSum(self, message):
        offset = 8
        checksum = 0
        for x in range (offset, len(message), 2):
            checksum += int(message[x:x+2], 16)
        checksum = checksum & (2**15 - 1)
        checksumHex = hex(checksum)
        checksumHex = checksumHex[2:]
        while len(checksumHex) < 4:
            checksumHex = "0" + checksumHex
        return checksumHex

    def convertSpeed(self, speed):
        newSpeed = abs((speed + 1) * 100)
        hexSpeed = hex(int(newSpeed))
        hexSpeed = hexSpeed[2:]
        if(len(hexSpeed) == 1):
            hexSpeed = "0" + hexSpeed
        print("hexSpeed ", hexSpeed)
        return hexSpeed

    def initiateConnection(self, mlink):
        mlink.sendResetMessage()
        mlink.readMessage()
        mlink.sendStartMessage()
        mlink.readMessage()

    
    def sendMessage(self, message):
        try:
            written = self.ser.write(bytes.fromhex(message))
            print(written, "bytes written")
            if written != (len(message) / 2):
                raise TypeError("message not sent successfully")
        except TypeError:
            print("message not sent successfully")
        except SerialTimeoutException:
            print("timeout exception thrown")

    def convertMessageToString(self, message):
        messageString = str(message)
        messageString = messageString.replace("\\", "")
        messageString = messageString.replace("x", "")
        messageString = messageString.replace("\'", "")
        messageString = messageString.replace("b", "", 1)
        return messageString

    def sendStartMessage(self, hopID = "00", networkSize="80"):
        if(self.ser.is_open):
            hexString = "a0a100020a" + hopID + networkSize
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("start message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString)
                self.ser.rts=False

    def sendEndpointStartMessage(self, flags = "00", pollMatchOffset = "00", pollMatchMask = "00", hopID = "00"):
        if(self.ser.is_open):
            hexString = "a0a100040a" + flags + pollMatchOffset + pollMatchMask + hopID
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("endpoint start message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString.decode("hex"))
                self.ser.rts=False

    def sendControllerSpeed(self, speed):
        if(self.ser.is_open):
            hexString = "a0a10003150300" + self.convertSpeed(speed)
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("controller message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString)
                self.ser.rts=False

    def sendResetMessage(self):
        if(self.ser.is_open):
            hexString = "a0a1000067"
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("reset message: ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage("01")
                self.sendMessage(hexString)
                self.ser.rts=False

    def sendStartPollMessage(self, endpointAddress = "00", pollMessage = "ff", pollInterval = "00000001", pollPriority = "00"):
        if(self.ser.is_open):
            hexString = "a0a100070f" + endpointAddress + pollMessage + pollPriority + pollInterval
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("start poll request message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString)
                self.ser.rts=False

    def sendNack(self, message, reason):
        if(self.ser.is_open):
            messageID = message[8:10]
            hexString = "a0a1000202" + messageID + reason
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("Nack message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString)
                self.ser.rts=False

    def sendAck(self, message):
        if(self.ser.is_open):
            messageID = message[8:10]
            hexString = "a0a1000101" + messageID
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("ack message ", bytes.fromhex(hexString))

            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage(hexString)
                self.ser.rts=False

    def compareCheckSum(self, message):
        checksum = self.calculateCheckSum(message[:-8])
        if(checksum == message[-8:-4]):
            return True
        return False


    def readMessage(self):
        if(self.ser.is_open):
            message = self.ser.readline()
            if len(message)==0:
                return 0
            message = message.hex()
            print("message received", message)
            if(message[8:10] != '01'):
                if self.compareCheckSum(message):
                    self.sendAck(message)
                else:
                    self.sendNack(message, "02")
            self.ser.flush()
            return message
        return 0
