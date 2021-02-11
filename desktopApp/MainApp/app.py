# imports
import tkinter as tk
import tkinter.font as tkFont
from helperFunctions import *

# initializing window
root = tk.Tk()
root.title("Tatsy Project")
root.geometry("400x400")
root.resizable(0, 0)

# first window components
titleFont = tkFont.Font(family="Helvetica", size="36", weight="bold")
buttonFont = tkFont.Font(family="Helventica", size="20")

noProjectFrame = tk.Frame(root)

appTitleLabel = tk.Label(noProjectFrame, text="Tatsy", padx=30, pady=30, font=titleFont)
newProjectButton = tk.Button(noProjectFrame, text="New Project", font=buttonFont, width="15", pady="5", command=helpers.createNewProject)
openProjectButton = tk.Button(noProjectFrame, text="Open Project", font=buttonFont, width="15", pady="5", command=helpers.openProject)

#packing to frame
appTitleLabel.grid(pady="5", row="0")
newProjectButton.grid(pady="5", row="1")
openProjectButton.grid(pady="5", row="2")

noProjectFrame.pack(expand=True)


# main loop
root.mainloop()