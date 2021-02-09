import XInput as XI

connected = XI.get_connected()
print(connected)

print(XI.get_thumb_values(XI.get_state(0))) 