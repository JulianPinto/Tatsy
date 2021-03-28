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
        checksumString = str(checksum)
        while len(checksumString) < 4:
            checksumString = "0" + checksumString
        return checksumString

    def convertSpeed(self, speed):
        newSpeed = abs((speed + 1) * 100)
        hexSpeed = hex(int(newSpeed))
        hexSpeed = hexSpeed[2:]
        if(len(hexSpeed) == 1):
            hexSpeed = "0" + hexSpeed
        print("hexSpeed ", hexSpeed)
        return hexSpeed
    
    def sendMessage(self, message):
        try:
            written = self.ser.write(bytes.fromhex(message))
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

    def sendStartMessage(self, hopID = "00", networkSize="00"):
        if(self.ser.is_open):
            hexString = "a0a100020a" + hopID + networkSize
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("start message ", bytes.fromhex(hexString))
    
            self.sendMessage(hexString)

    def sendEndpointStartMessage(self, flags = "00", pollMatchOffset = "00", pollMatchMask = "00", hopID = "00"):
        if(self.ser.is_open):
            hexString = "a0a100040a" + flags + pollMatchOffset + pollMatchMask + hopID
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("endpoint start message ", bytes.fromhex(hexString))
            if(not(self.ser.rts)):
                self.ser.rts=True
                self.sendMessage( hexString)
                self.ser.rts=False

    def sendControllerSpeed(self, speed):
        if(self.ser.is_open):
            hexString = "a0a1000211" + self.convertSpeed(speed)
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "b0b1"
            print("controller message ", bytes.fromhex(hexString))

            self.sendMessage(hexString)

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

            self.sendMessage(hexString)

    def sendNack(self, message, reason):
        if(self.ser.is_open):
            messageID = hex(message[2])
            hexString = "a0a1000202" + messageID[2:] + reason
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("Nack message ", bytes.fromhex(hexString))

            self.sendMessage(hexString)

    def sendAck(self, message):
        if(self.ser.is_open):
            messageID = hex(message[2])
            hexString = "a0a1000101" + messageID[2:]
            checksum = self.calculateCheckSum(hexString)
            hexString = hexString + checksum + "b0b1"
            print("ack message ", bytes.fromhex(hexString))

            self.sendMessage(hexString)

    def compareCheckSum(self, message):
        if(self.ser.is_open):
            messageString = self.convertMessageToString(message)

            checksum = self.calculateCheckSum(messageString)
            if(checksum == messageString[-8:-4]):
                return True
            return False


    def readMessage(self):
        if(self.ser.is_open):
            message = self.ser.readline()
            print("message received", message)
            self.ser.flush()
            return message
        return 0