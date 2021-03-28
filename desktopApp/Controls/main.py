from controller import controller
from uartCommunication import MlinkCommunication


if __name__ == "__main__":
    print("starting program")
    print("initializing")
    mlink = MlinkCommunication(port = "COM1") # idk change if wrong
    print("entering loop")
    for x in range (0, 10):
        print("reading message")
        mlink.readMessage()
        print("writing controller")
        mlink.sendControllerSpeed(speed = controller.getControllerStickInput())
        print(x)