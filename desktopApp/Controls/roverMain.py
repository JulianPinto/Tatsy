from controller import controller
from uartCommunication import MlinkCommunication


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
        pass

    while True:
        print("writing controller")
        mlink.sendControllerSpeed(speed = controller.getControllerStickInput())
        print("reading message")
        mlink.readMessage()