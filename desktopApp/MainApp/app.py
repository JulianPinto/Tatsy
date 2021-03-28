# imports
import os
import tkinter as tk
import tkinter.font as tkFont
import tkinter.messagebox
import easygui

from projectPage import projectPage

# initializing window
class newPage:
    def __init__(self, master):
        self.master = master

        self.master.title("Tatsy Project")
        self.master.geometry("400x400")
        self.master.resizable(0, 0)

        # first window components
        self.titleFont = tkFont.Font(family="Helvetica", size="36", weight="bold")
        self.buttonFont = tkFont.Font(family="Helventica", size="20")

        self.noProjectFrame = tk.Frame(self.master)

        self.appTitleLabel = tk.Label(self.noProjectFrame, text="Tatsy", padx=30, pady=30, font=self.titleFont)
        self.newProjectButton = tk.Button(self.noProjectFrame, text="New Project", font=self.buttonFont, width="15", pady="5", command=self.newProject)
        self.openProjectButton = tk.Button(self.noProjectFrame, text="Open Project", font=self.buttonFont, width="15", pady="5", command=self.openProject)

        #packing to frame
        self.appTitleLabel.grid(pady="5", row="0")
        self.newProjectButton.grid(pady="5", row="1")
        self.openProjectButton.grid(pady="5", row="2")

        self.noProjectFrame.pack(expand=True)

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
            print(dirPath)
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

# main loop
if __name__ == '__main__':
    root = tk.Tk()
    app = newPage(root)

    root.mainloop()