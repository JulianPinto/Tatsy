import tkinter as tk
from tkinter.constants import CENTER, INSERT, NE, NW
import tkinter.font as tkFont
import tkinter.messagebox
import os
import cv2
import easygui
import threading
from distutils.dir_util import copy_tree

# initializing window
class projectPage:
    def __init__(self, master, dirPath):
        # all class vars
        self.dirPath = dirPath
        self.thread = None
        self.videoPath = 0
        self.videoThread = None
        self.stopEvent = threading.Event()
        self.capture = None
        self.out = None

        self.master = master
        self.generalFont = None
        self.notesArea = None
        self.infoText = None
        self.menuBar = None
        self.fileMenu = None
        self.videoMenu = None
        self.videoFrame = None
        self.videoDisplay = None
        self.timeDisplay = None

        # init window
        self.master.title("Tatsy Project")
        self.master.state('zoomed')
        self.master.update_idletasks()

        self.generalFont = tkFont.Font(family="Helventica", size="12")

        # create components
        self.notesArea = tk.Text(self.master, font=self.generalFont, padx=4, pady=4, yscrollcommand=set())
        self.infoText = tk.Listbox(self.master, font=self.generalFont)

        #menu components
        self.menuBar = tk.Menu(self.master, font=self.generalFont, tearoff=0)

        # file menu
        self.fileMenu = tk.Menu(self.menuBar, tearoff=0, font=self.generalFont)
        self.fileMenu.add_command(label="New", command=self.newProject)
        self.fileMenu.add_command(label="Open", command=self.openProject)
        self.fileMenu.add_command(label="Save", command=self.saveProject)
        self.fileMenu.add_command(label="Save As", command=self.saveProjectAsNew)

        self.fileMenu.add_separator()
        self.menuBar.add_cascade(label="File", menu=self.fileMenu)

        # video menu
        self.videoMenu = tk.Menu(self.menuBar, tearoff=0, font=self.generalFont)
        self.videoMenu.add_command(label="Record", command=self.recordVideo)
        self.videoMenu.add_command(label="View Video", command=self.openVideo)
        self.videoMenu.add_command(label="View Rover", command=self.openRoverVideo)

        self.videoMenu.add_separator()
        self.menuBar.add_cascade(label="Video", menu=self.videoMenu)

        self.master.config(menu=self.menuBar)

        #video components
        self.videoFrame = tk.Frame(self.master)
        self.videoDisplay = tk.Label(self.videoFrame, text="Video area", bg="white")
        self.timeDisplay = tk.Label(self.videoFrame, font=self.generalFont, text="Time: 07:45:33", 
            padx=4, pady=4, justify=CENTER)
        self.distanceDisplay = tk.Label(self.videoFrame, font=self.generalFont, text="Distance: 123cm",
            padx=4, pady=4, justify=CENTER)

        # display data
        self.infoText.insert(1, "Date: Jan 8, 2021")
        self.infoText.insert(2, "Project: TestProj")
        self.infoText.insert(3, "Distance: 123cm")
        self.infoText.insert(4, "Duration: 12:32")
        self.infoText.insert(5, "Controls: F-23")

        # packing to window
        self.notesArea.place(relheight=0.96, relwidth=0.3, relx=0.02, rely=0.02)
        self.infoText.place(relheight=0.2, relwidth=0.64, relx=0.34, rely=0.78)
        self.videoFrame.place(relheight=0.74, relwidth=0.64, relx=0.34, rely=0.02)

        #video frame packing
        self.videoDisplay.place(relheight=1, relwidth=1)
        self.timeDisplay.place(relx=0, rely=0, y=5)
        self.distanceDisplay.place(relx=0, rely=0, y=40)

        # close handling
        self.master.wm_protocol("WM_DELETE_WINDOW", self.onClose)

        #setup Project Files
        self.readInNotesFile()

    def onClose(self):
        print("closing")
        if self.capture != None:
            self.capture.release()

        if self.out != None:
            self.out.release()
        try:
            self.stopEvent.set()
            if self.videoThread != None:
                while self.videoThread.is_alive():
                    pass
        except:
            pass
        self.master.destroy()

    def readInNotesFile(self):
        textFile = open(self.dirPath + '\\notes.txt', 'r')
        textInput = textFile.read()
        textFile.close()
        
        self.notesArea.insert(INSERT, textInput)

    def saveProject(self):
        notesInput = self.notesArea.get("1.0", tk.END)
        textFile = open(self.dirPath + '\\notes.txt', 'w')
        textFile.write(notesInput)
        textFile.close()

    def saveProjectAsNew(self):
        self.saveProject()

        open(self.dirPath + '\\notes.txt', 'r').close()
        newDirPath = easygui.diropenbox(title = "New Project Directory")
        if newDirPath != None:
            copy_tree(self.dirPath, newDirPath)
        
        self.nextWindow(newDirPath)

    def recordVideo(self):
        if self.capture == None:
            return
        if not self.capture.read()[0]:
            return

        fourcc = cv2.VideoWriter(*'DIVX')
        self.out = cv2.VideoWriter(self.dirPath + '\\video\\output.mp4', fourcc, 32.0, (1920, 1080), True)
        while self.capture.isOpened():
            ret, frame = self.capture.read()
            if not ret:
                break
            
            frame = cv2.flip(frame, 0)
            self.out.write(frame)
            
            cv2.imshow('frame', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        
        self.capture.release()
        self.out.release()
        cv2.destroyAllWindows()
    
    def openVideo(self):
        if self.capture != None:
            self.capture.release()
        videoPath = easygui.fileopenbox(title = "Open video File", default = (self.dirPath + "\\video\\*.mp4"))
        if videoPath != None:
            self.capture = cv2.VideoCapture(videoPath)
            while self.capture.isOpened():           
                ret, frame = self.capture.read()

                if not ret:
                    key =  cv2.waitKey()
                    if (key == 32) or (key == 107): # space or k
                        self.capture.set(cv2.CAP_PROP_POS_FRAMES, 0)
                    elif key == 106: # j key goes back 5 sec
                        frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                        frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                        newFrameNum = frameNum - frameRate * 5
                        if newFrameNum < 0:
                            newFrameNum = 0
                        self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                        cv2.waitKey(1)
                    elif key & 0xFF == ord('q'):
                        break
                else:
                    cv2.imshow('frame', frame)
                    key =  cv2.waitKey(1)
                    if (key == 32) or (key == 107):
                        cv2.waitKey()
                    elif key == 106: # j key goes back 5 sec
                        frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                        frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                        newFrameNum = frameNum - frameRate * 5
                        if newFrameNum < 0:
                            newFrameNum = 0
                        self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                    elif key == 108: # l key go forward 5 sec
                        frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                        frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                        newFrameNum = frameNum + frameRate * 5
                        if newFrameNum > self.capture.get(cv2.CAP_PROP_FRAME_COUNT):
                            newFrameNum = self.capture.get(cv2.CAP_PROP_FRAME_COUNT)                       
                        self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                    elif key & 0xFF == ord('q'):
                        break
            
            self.capture.release()
            cv2.destroyAllWindows()
     
    def openRoverVideo(self):
        if self.capture != None:
            self.capture.release()
        self.capture = cv2.VideoCapture(0) # Change if wrong camera
        while self.capture.isOpened():
            ret, frame = self.capture.read()
            if not ret:
                key =  cv2.waitKey()
                if (key == 32) or (key == 107): # space or k
                    self.capture.set(cv2.CAP_PROP_POS_FRAMES, 0)
                elif key == 106: # j key goes back 5 sec
                    frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                    frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                    newFrameNum = frameNum - frameRate * 5
                    if newFrameNum < 0:
                        newFrameNum = 0
                    self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                    cv2.waitKey(1)
                elif key & 0xFF == ord('q'):
                    break
            else:
                cv2.imshow('frame', frame)
                key =  cv2.waitKey(1)
                if (key == 32) or (key == 107):
                    cv2.waitKey()
                elif key == 106: # j key goes back 5 sec
                    frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                    frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                    newFrameNum = frameNum - frameRate * 5
                    if newFrameNum < 0:
                        newFrameNum = 0
                    self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                elif key == 108: # l key go forward 5 sec
                    frameNum = self.capture.get(cv2.CAP_PROP_POS_FRAMES)
                    frameRate = self.capture.get(cv2.CAP_PROP_FPS)
                    newFrameNum = frameNum + frameRate * 5
                    if newFrameNum > self.capture.get(cv2.CAP_PROP_FRAME_COUNT):
                        newFrameNum = self.capture.get(cv2.CAP_PROP_FRAME_COUNT)                       
                    self.capture.set(cv2.CAP_PROP_POS_FRAMES, newFrameNum)
                elif key & 0xFF == ord('q'):
                    break

        self.capture.release()
        cv2.destroyAllWindows()
    
    def newProject(self):
        dirPath = easygui.diropenbox(title = "New Project Directory")
        if dirPath != None:
            if not os.path.exists(dirPath + '\\notes.txt'):
                open(dirPath + '\\notes.txt', 'w').close()
            if not os.path.exists(dirPath + '\\video'):
                os.mkdir(dirPath + '\\video')

            self.nextWindow(dirPath)

    def openProject(self):
        dirPath = easygui.diropenbox(title = "Open Project Directory")
        if dirPath != None:
            if not os.path.exists(dirPath + '\\notes.txt'):
                tkinter.messagebox.showinfo("Invalid Directory", "This directory does not have the right files for a project directory.")
                return
            if not os.path.exists(dirPath + '\\video'):
                tkinter.messagebox.showinfo("Invalid Directory", "This directory does not have the right files for a project directory.")
                return

            self.nextWindow(dirPath)

    def nextWindow(self, path):
        self.master.destroy()
        self.master = tk.Tk()
        self.app = projectPage(self.master, path)
        self.master.mainloop()

if __name__ == '__main__':
    root = tk.Tk()
    app = projectPage(root, r'C:\Users\work\Projects\Tatsy\desktopApp\projfile')
    root.mainloop()