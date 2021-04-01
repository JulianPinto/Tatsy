import tkinter as tk
from tkinter.constants import CENTER, INSERT, NE, NW, SOLID
import tkinter.font as tkFont
from PIL import Image, ImageTk
import tkinter.messagebox
import os
import cv2
import numpy as np
import easygui
import time
from distutils.dir_util import copy_tree

# initializing window
class projectPage:
    def __init__(self, master, dirPath):
        # all class vars
        self.dirPath = dirPath
        self.thread = None
        self.videoPath = 0
        self.capture = None
        self.out = None
        self.videoTime = 0
        self.time = time.strftime("%c")
        self.viewRover = False
        self.recording = False
        self.fourcc = None

        self.master = master
        self.generalFont = None
        self.style = None
        self.notesArea = None
        self.infoText = None
        self.menuBar = None
        self.fileMenu = None
        self.videoMenu = None
        self.videoFrame = None
        self.videoDisplay = None
        self.timeDisplay = None
        self.pauseVideo = True

        # init window
        self.master.title("Tatsy Project")
        self.master.state('zoomed')
        self.master.update_idletasks()

        # font
        self.generalFont = tkFont.Font(family="Helventica", size="12")
        self.largerFont = tkFont.Font(family="Helventica", size="16")

        # create components
        self.notesArea = tk.Text(self.master, font=self.largerFont, padx=4, pady=4, yscrollcommand=set(), borderwidth=1, relief=SOLID)
        self.infoText = tk.Listbox(self.master, font=self.largerFont, borderwidth=1, relief=SOLID)
        self.infoText.bind("<Button-1>", self.copyListbox)

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
        self.videoMenu.add_command(label="View Video", command=self.showVideo)
        self.videoMenu.add_command(label="View Rover", command=self.openRoverVideo)

        self.videoMenu.add_separator()
        self.menuBar.add_cascade(label="Video", menu=self.videoMenu)

        self.master.config(menu=self.menuBar)

        #video components
        self.videoFrame = tk.Frame(self.master)
        self.videoDisplay = tk.Label(self.videoFrame, text="Select A Video To Watch", bg="white", borderwidth=1, relief=SOLID)
        self.videoDisplay.bind("<Button-1>", self.pausePlayVideo)
        self.timeDisplay = tk.Label(self.videoFrame, font=self.largerFont, text="Time: 0:0:0",
            padx=4, pady=4, justify=CENTER)
        # self.distanceDisplay = tk.Label(self.videoFrame, font=self.generalFont, text="Distance: 123cm", padx=4, pady=4, justify=CENTER)

        # display data
        self.infoText.insert(1, "Date: " + self.time)
        self.infoText.insert(2, "Project: " + self.dirPath[self.dirPath.rfind('\\') + 1:])
        self.infoText.insert(3, "Video: None")
        self.infoText.insert(4, "Duration: 0:0:0")
        self.infoText.insert(5, "Rover: Off")

        # packing to window
        self.notesArea.place(relheight=0.96, relwidth=0.3, relx=0.02, rely=0.02)
        self.infoText.place(relheight=0.2, relwidth=0.64, relx=0.34, rely=0.78)
        self.videoFrame.place(relheight=0.74, relwidth=0.64, relx=0.34, rely=0.02)

        #video frame packing
        self.videoDisplay.place(relheight=1, relwidth=1)
        self.timeDisplay.place(relx=0, rely=0, y=5)
        # self.distanceDisplay.place(relx=0, rely=0, y=40)

        # close handling
        self.master.wm_protocol("WM_DELETE_WINDOW", self.onClose)

        #setup Project Files
        self.readInNotesFile()
        self.update()

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
        cv2.destroyAllWindows()
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
        if not self.recording:
            self.recording = True
            self.fourcc = cv2.VideoWriter_fourcc(*'MJPG')
            path = self.dirPath + '\\video\\' + str(time.strftime("%c")).replace(" ", "-") + '.avi'
            self.out = cv2.VideoWriter('desktopapp\\projfile\\video\\out.avi', self.fourcc, 32.0, (1920, 1080))
        else:
            self.recording = False
            self.out.release()
            cv2.destroyAllWindows()
    
    def showVideo(self):
        if self.capture != None:
            self.capture.release()
        videoPath = easygui.fileopenbox(title = "Open video File", default = (self.dirPath + "\\video\\*.mp4"))
        if videoPath != None:
            self.capture = cv2.VideoCapture(videoPath)
            self.infoText.delete(2)
            self.infoText.insert(2, "Video: " + videoPath[videoPath.rfind('\\') + 1:])
            self.viewRover = False
    
    def update(self):
        # time dislplays
        self.time = time.strftime("%c")
        self.infoText.delete(0)
        self.infoText.insert(0, "Date: " + self.time)

        firstFrame = True
        try:
            self.videoDisplay.imgtk
            firstFrame = False
        except:
            pass
        if self.capture != None:
            if (not self.pauseVideo) or firstFrame:
                ret, frame = self.capture.read()
                if ret:
                    #recording
                    if (self.viewRover) and (self.recording):
                        self.out.write('frame', frame)
                    # video time
                    millis = self.capture.get(cv2.CAP_PROP_POS_MSEC)
                    seconds=(millis/1000)%60
                    seconds = int(seconds)
                    minutes=(millis/(1000*60))%60
                    minutes = int(minutes)
                    hours=(millis/(1000*60*60))%24
                    hours=int(hours)
                    self.videoTime = "Time: " + str(hours) + ':' + str(minutes) + ':' + str(seconds)
                    self.timeDisplay.configure(text=self.videoTime)
                    self.infoText.delete(3)
                    self.infoText.insert(3, "Duration " + self.videoTime)

                    # video frame
                    self.videoDisplay.update_idletasks()
                    dim = (int(self.videoDisplay.winfo_width()), int(self.videoDisplay.winfo_height()))
                    frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)
                    frame = cv2.flip(frame, 1)
                    cv2image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGBA)
                    img = Image.fromarray(cv2image)
                    imgtk = ImageTk.PhotoImage(image=img)
                    self.videoDisplay.imgtk = imgtk
                    self.videoDisplay.configure(image = imgtk)
        self.videoDisplay.after(15, self.update)
    
    def pausePlayVideo(self, event):
        if self.pauseVideo:
            self.pauseVideo = False
        else:
            self.pauseVideo = True

    def copyListbox(self, event):
        self.master.clipboard_clear()
        selected = self.infoText.get(tk.ANCHOR)
        self.master.clipboard_append(selected)
     
    def openRoverVideo(self):
        if self.capture != None:
            self.capture.release()
        self.capture = cv2.VideoCapture(0) # Change if wrong camera
        if self.capture.isOpened():
            self.infoText.delete(2)
            self.infoText.insert(2, "Video: Rover")
            self.viewRover = True
    
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