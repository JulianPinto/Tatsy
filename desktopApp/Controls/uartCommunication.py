from typing import Type
import serial
from serial.serialutil import SerialException, SerialTimeoutException

class MlinkCommunication:
    def __init__(self, port, baudRate = 57600, timeout = 1):
        try:
            self.ser = serial.Serial(port, baudRate, timeout = timeout, rtscts = True)
        except SerialException:
            print("Port ", port, " not opened")

    def calculateCheckSum(message):
        offset = 8
        checksum = 0
        for x in range (offset, message.length, 2):
            checksum += int(message[x:x+2], 16)
        checksum = checksum & (2**15 - 1)
        checksumString = str(checksum)
        while len(checksumString) < 4:
            checksumString = "0" + checksumString
        return checksumString

    def convertSpeed(speed):
        newSpeed = abs((speed + 1) * 100)
        hexSpeed = hex(int(newSpeed))
        hexSpeed = hexSpeed[2:]
        if(len(hexSpeed) == 1):
            hexSpeed = "0" + hexSpeed
        print("hexSpeed " + hexSpeed)
        return hexSpeed

    def sendStartMessage(self, hopID = "00", networkSize="00"):
        if(self.ser.is_open):
            hexString = "A0A100020A" + hopID + networkSize
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "B0B1"
            print("start message " + bytes.fromhex(hexString))

            try:
                written = self.ser.write(bytes.fromhex(hexString))
                if written != 10:
                    raise TypeError("message not sent successfully")
            except TypeError:
                print("message Not sent successfully")
            except SerialTimeoutException:
                print("timeout exception thrown")

    def sendEndpointStartMessage(self, flags = "00", pollMatchOffset = "00", pollMatchMask = "00", hopID = "17"):
        if(self.ser.is_open):


    def sendControllerSpeed(self, speed):
        if(self.ser.is_open):
            hexString = "A0A1000211" + str(convertSpeed(speed))
            checkSum = self.calculateCheckSum(hexString)
            hexString = hexString + checkSum + "B0B1"
            print("controller message " + bytes.fromhex(hexString))

            try:
                written = self.ser.write(bytes.fromhex(hexString))
                if written != 10:
                   raise TypeError("message not sent successfully")
            except TypeError:
                print("message Not sent successfully")
            except SerialTimeoutException:
                print("timeout exception thrown")


ml = MlinkCommunication("COM1")
ml.sendStartMessage()