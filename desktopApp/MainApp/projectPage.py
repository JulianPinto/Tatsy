import tkinter as tk
from tkinter.constants import CENTER, INSERT, NE, NW
import tkinter.font as tkFont
from helperFunctions import *

# initializing window
class projectPage:
    def __init__(self):
        # init window
        self.root = tk.Tk()
        self.root.title("Tatsy Project")
        self.root.state('zoomed')
        self.root.update_idletasks()

        self.generalFont = tkFont.Font(family="Helventica", size="12")

        # create components
        self.notesArea = tk.Text(self.root, font=self.generalFont, padx=4, pady=4, yscrollcommand=set())
        self.infoText = tk.Listbox(self.root, font=self.generalFont)

        #menu components
        self.menuBar = tk.Menu(self.root, font=self.generalFont, tearoff=0)

        # file menu
        self.fileMenu = tk.Menu(self.menuBar, tearoff=0, font=self.generalFont)
        self.fileMenu.add_command(label="New", command=helpers.createNewProject)
        self.fileMenu.add_command(label="Open", command=helpers.openProject)
        self.fileMenu.add_command(label="Save", command=helpers.saveProject)
        self.fileMenu.add_command(label="Save As", command=helpers.saveProjectAsNew)

        self.fileMenu.add_separator()
        self.menuBar.add_cascade(label="File", menu=self.fileMenu)

        self.menuBar.add_command(label="Record", command=helpers.recordVideo)
        self.menuBar.add_separator()

        self.root.config(menu=self.menuBar)

        #video components
        self.videoFrame = tk.Frame(self.root)
        self.videoDisplay = tk.Label(self.videoFrame, text="Video area", bg="white")
        self.timeDisplay = tk.Label(self.videoFrame, font=self.generalFont, text="Time: 07:45:33", 
            padx=4, pady=4, justify=CENTER)
        self.distanceDisplay = tk.Label(self.videoFrame, font=self.generalFont, text="Distance: 123cm",
            padx=4, pady=4, justify=CENTER)

        # display data
        self.infoText.insert(1, "Date: Jan8, 2021")
        self.infoText.insert(2, "Project: TestProj")
        self.infoText.insert(3, "Distance: 123cm")
        self.infoText.insert(4, "Duration: 12:32")

        self.notesArea.insert(INSERT, "Notes on current inspection\n cracking at 45cm to 50cm top side\n")

        # packing to window
        self.notesArea.place(relheight=0.96, relwidth=0.3, relx=0.02, rely=0.02)
        self.infoText.place(relheight=0.2, relwidth=0.64, relx=0.34, rely=0.78)
        self.videoFrame.place(relheight=0.74, relwidth=0.64, relx=0.34, rely=0.02)

        #video frame packing
        self.videoDisplay.place(relheight=1, relwidth=1)
        self.timeDisplay.place(relx=0, rely=0, y=5)
        self.distanceDisplay.place(relx=0, rely=0, y=40)

        # main loop
        self.root.mainloop()


# mainloop
if __name__ == '__main__':
    app = projectPage()