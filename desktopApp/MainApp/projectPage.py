import tkinter as tk
from tkinter.constants import CENTER, INSERT, NE, NW
import tkinter.font as tkFont
import tkinter.messagebox
import os
import easygui

# initializing window
class projectPage:
    def __init__(self, master, path):
        # init window
        self.master = master
        self.master.title("Tatsy Project")
        self.master.state('zoomed')
        self.master.update_idletasks()

        self.path = path

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

        self.menuBar.add_command(label="Record", command=self.recordVideo)
        self.menuBar.add_separator()

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

        self.notesArea.insert(INSERT, "\n\nNotes on current inspection\n\ncracking at 45cm to 50cm top side\n\npotential break at 60cm\n\nsome build up at 70cm\n\nunknown object stuck at 100cm\n\nmold at 110cm\n")

        # packing to window
        self.notesArea.place(relheight=0.96, relwidth=0.3, relx=0.02, rely=0.02)
        self.infoText.place(relheight=0.2, relwidth=0.64, relx=0.34, rely=0.78)
        self.videoFrame.place(relheight=0.74, relwidth=0.64, relx=0.34, rely=0.02)

        #video frame packing
        self.videoDisplay.place(relheight=1, relwidth=1)
        self.timeDisplay.place(relx=0, rely=0, y=5)
        self.distanceDisplay.place(relx=0, rely=0, y=40)

    def saveProject(self):
        print("save proj")

    def saveProjectAsNew(self):
        print("save as new")

    def recordVideo(self):
        print("record")

    def changeVideo(self):
        print("change vid input")
    
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

if __name__ == '__main__':
    root = tk.Tk()
    app = projectPage(root, 'C:\Users\work\Projects\Tatsy\desktopApp\projfile')
    root.mainloop()