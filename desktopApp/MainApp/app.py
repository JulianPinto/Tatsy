# imports
import tkinter as tk

win = tk.Tk()

win.title("Tatsy Project")

# win.resizable(False, False)

frame = tk.Frame(win)
frame.pack()


tk.Label(frame, text="Label").pack()
tk.Button(win, text="Button").pack()

label2 = tk.Label(win, text="label2")
label2.pack()



# main loop
win.mainloop()