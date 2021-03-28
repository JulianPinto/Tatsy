from controller import controller
from uartCommunication import MlinkCommunication


if __name__ == "__main__":
    print("starting program")
    print("initializing")
    mlink = MlinkCommunication(port = "COM9") # idk change if wrong
    mlink.sendResetMessage()
    print("Just Reset")
    hasStarted=False
    

    print("entering loop")
    while True:
        if(not(hasStarted)):
            mlink.sendStartMessage()
            hasStarted=True
            print("Just Started")
        
        print("reading message")
        mlink.readMessage()
        print("writing controller")
        mlink.sendControllerSpeed(speed = controller.getControllerStickInput())
        

