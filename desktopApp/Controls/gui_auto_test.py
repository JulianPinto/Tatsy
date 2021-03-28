from controller import controller
from XInput import *
from pywinauto.application import Application

app=Application(backend='uia').connect(title='Base Data Request', timeout=200)

#app.BaseDataRequest.print_control_identifiers()
textInput = app.BaseDataRequest.child_window(auto_id="textToSend", control_type="Edit").wrapper_object()
textInput.type_keys("01FF")

sendInput = app.BaseDataRequest.child_window(title="Send", auto_id="buttonSend", control_type="Button").wrapper_object()
sendInput.click_input()

movement_enabler=True

connected = get_connected()

if __name__ == "__main__":
    def convertSpeed( speed):
        newSpeed = abs((speed + 1) * 100)
        hexSpeed = hex(int(newSpeed))
        hexSpeed = hexSpeed[2:]
        if(len(hexSpeed) == 1):
            hexSpeed = "0" + hexSpeed
        print("hexSpeed ", hexSpeed)
        return hexSpeed

    def getControllerStickInput():
        if(connected[0]):
            controllerState = get_state(0)
            stickInput = get_thumb_values(controllerState)
            print("stick input", stickInput[0][1])
            return stickInput[0][1]
        print("no controller connected returning 0 from getController")
        return 0.0

    def switchControl():
        events = get_events()
        if(connected[0]):
            for event in events:
                if event.type == EVENT_BUTTON_PRESSED:
                    if event.button == "DPAD_UP":
                        print("Contoller switch button pressed")
                        return True
        print("Contoller switch button not pressed")
        return False

    while True:
        if(movement_enabler):
            controllerSpeed=convertSpeed(speed = getControllerStickInput())

            textInput = app.BaseDataRequest.child_window(auto_id="textToSend", control_type="Edit").wrapper_object()
            
            textInput.set_text(controllerSpeed)

            sendInput = app.BaseDataRequest.child_window(title="Send", auto_id="buttonSend", control_type="Button").wrapper_object()
            sendInput.click_input()
            textInput.set_text("")
            if(switchControl):
                movement_enabler=False