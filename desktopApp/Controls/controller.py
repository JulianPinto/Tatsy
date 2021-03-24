from XInput import *

def getControllerStickInput():
    connected = get_connected()
    if(connected[0]):
        controllerState = get_state(0)
        stickInput = get_thumb_values(controllerState)
        print(stickInput[0][1])
        return stickInput[0][1]
    return 0.0