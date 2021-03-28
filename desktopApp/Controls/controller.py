from XInput import *

class controller:
    def getControllerStickInput():
        connected = get_connected()
        if(connected[0]):
            controllerState = get_state(0)
            stickInput = get_thumb_values(controllerState)
            print("stick input", stickInput[0][1])
            return stickInput[0][1]
        print("no controller connected returning 0 from getController")
        return 0.0
