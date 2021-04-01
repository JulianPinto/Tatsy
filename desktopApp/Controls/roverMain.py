from controller import controller
from uartCommunication import MlinkCommunication
import time


if __name__ == "__main__":
    print("starting program")
    print("initializing")
    mlink = MlinkCommunication(port = "COM9") # idk change if wrong
    mlink.sendResetMessage()
    mlink.readMessage()
    mlink.sendStartMessage()
    mlink.readMessage()
    

    print("entering loop")
    while(mlink.readMessage() != 0):
        time.sleep(0.2)

    while True:
        print("writing controller")
        mlink.sendControllerSpeed(speed = controller.getControllerStickInput())
        time.sleep(0.5)
        print("reading message")
        mlink.readMessage()
        time.sleep(0.5)