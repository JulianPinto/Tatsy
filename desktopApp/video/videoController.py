import cv2
import numpy
import threading
from PIL import Image
from PIL import ImageTk
import tkinter as tk

def playVideo(frame, stopEvent, path):
    if not path > 0:
        print("no video path given")
        return
    try:
        while not stopEvent.is_set():
            videoReader = path.read()

            image = cv2.cvtColor(videoReader, cv2.COLOR_BGR2RGB)
            image = Image.fromarray(image)
            image = ImageTk.PhotoImage(image)

            frame.configure(image = image)
            frame.image = image
    
    except:
        print("error in playVideo")

cameraNum = 1
cap = cv2.VideoCapture(cameraNum)
if not cap.isOpen():
    print("no camera as input ", cameraNum)
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("no frame. Exiting")
        break
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindos()