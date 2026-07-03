#!/usr/bin/env python3
""" /*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gian Matteo Cossu
 *
 *   This file is part of TFBoost.
 *
 *   TFBoost is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   TFBoost is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with TFBoost.  If not, see <http://www.gnu.org/licenses/>.
 *
 *---------------------------------------------------------------------------*/
/*
 *
 *
 *  Created on: 01/07/2021
 *      Author:  Gian Matteo Cossu
 */ """




from tkinter import *
from tkinter import filedialog, constants
from tkinter import font
from PIL import ImageTk,Image
import os
import tkinter as tk
import subprocess
import webbrowser
import math
import random
import pandas as pd
import matplotlib.pyplot as plt
import tfb_ui
import sys

resW = tk.Tk();
tfb_ui.apply_font_scaling(resW)
resW.title('TFBoost GUI')
resW.geometry("800x750")
resW.resizable(0, 0)
resW.tk.call('wm', 'iconphoto', resW._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

InputDone = IntVar();
OutputDone = IntVar();

print ('Number of arguments:')
print(len(sys.argv))
print('arguments.')
print ('Argument List:', str(sys.argv))

print(sys.argv[1])

rows = int(sys.argv[3])

if (sys.argv[1]!=''):
    folder_selected1 = sys.argv[1]
    directory0 = os.path.split(folder_selected1)[0] + '/' + os.path.split(folder_selected1)[1]
    text1 = Text(resW, state='disabled', width=50, height=1)
    text1.place (x=300,y=112)
    text1.configure(state="normal")
    text1.insert('end', directory0)
    text1.configure(state="disabled")
    InputDone.set(1)

if (sys.argv[2]!=''):
    folder_selected2 = sys.argv[2]
    directory = os.path.split(folder_selected2)[0] + '/' + os.path.split(folder_selected2)[1]
    text1 = Text(resW, state='disabled', width=50, height=1)
    text1.place (x=300,y=160)
    text1.configure(state="normal")
    text1.insert('end', directory)
    text1.configure(state="disabled")
    OutputDone.set(1)

def openFileInput():
    global folder_selected1
    folder_selected1 = filedialog.askdirectory()
    if not folder_selected1:          # dialog cancelled
        return
    directory0 = os.path.split(folder_selected1)[0] + '/' + os.path.split(folder_selected1)[1]
    text1 = Text(resW, state='disabled', width=50, height=1)
    text1.place (x=300,y=112)
    text1.configure(state="normal")
    text1.insert('end', directory0)
    text1.configure(state="disabled")
    InputDone.set(1)


def openFileOutput():
    global folder_selected2
    folder_selected2 = filedialog.askdirectory()
    if not folder_selected2:          # dialog cancelled
        return
    directory = os.path.split(folder_selected2)[0] + '/' + os.path.split(folder_selected2)[1]
    text1 = Text(resW, state='disabled', width=50, height=1)
    text1.place (x=300,y=150)
    text1.configure(state="normal")
    text1.insert('end', directory)
    text1.configure(state="disabled")
    dir1 = directory + '/plots'
    print(dir1)
    if not os.path.exists(dir1):
        os.makedirs(dir1)
    dir2 = directory + '/data'
    print(dir2)
    if not os.path.exists(dir2):
        os.makedirs(dir2)

    OutputDone.set(1)

def run_results ():

    if not (InputDone.get()==1 and OutputDone.get()==1):
        return

    path1 = folder_selected1
    path2 = folder_selected2 + '/data'
    files2 = os.listdir(path2)
    index2 = random.randrange(0, len(files2))
    print(files2[index2])

    plt.clf()

    # The convoluted output files are named after the input files, so the same
    # filename indexes both the input current (path1) and the output (path2).
    data1 = pd.read_csv(path1+'/'+files2[index2],sep=r'\s+',header=None,skiprows=rows)
    data1 = pd.DataFrame(data1)
    data2 = pd.read_csv(path2+'/'+files2[index2],sep=r'\s+',header=None)
    data2 = pd.DataFrame(data2)

    x1 = data1[0]
    y1 = data1[1]

    plt.subplot(2, 1, 1)
    plt.plot(x1, y1, 'r-',label=files2[index2])
    plt.title('Results')
    plt.xlabel('time (s)')
    plt.ylabel('Current [A]')

    plt.legend()

    x2 = data2[0]
    y2 = data2[1]
    plt.subplot(2, 1, 2)
    plt.plot(x2, y2, 'g-',label=files2[index2])
    plt.xlabel('time (s)')
    plt.ylabel('Voltage [V]')

    plt.legend()
    plt.tight_layout(pad=1.0)
    plt.show()

def open_macro(macroname):
    """Open a ROOT histogram macro (<name>.C) from the output plots directory."""
    if OutputDone.get()==1:
        shellCommand = ['root', " " + folder_selected2 + "/plots/" + macroname + ".C"]
        print("Run Macro", shellCommand)
        return subprocess.Popen(shellCommand)



my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=105)

my_label7 = Label(resW, text='TFBOOST RESULTS GUI',font = ("Arial",9))
my_label7.place(x=20,y=85)

my_button = Button(resW, text="Choose directory for the input currents:",command=openFileInput,font = ("Arial",9))
my_button.place(x=20,y=110)

my_button2 = Button(resW, text="Choose directory for the output files and plots:",command=openFileOutput,font = ("Arial",9))
my_button2.place(x=20,y=150)

res_btn = Button(resW, text="\nPRESS TO VISUALIZE RANDOM WAVEFORM\n",command=run_results,font = ("Arial",9))
res_btn.place(x=270,y=200)


my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=260)

my_label7 = Label(resW, text='HISTOGRAMS WITHOUT NOISE',font = ("Arial",9))
my_label7.place(x=20,y=270)

# Histogram viewer buttons, built from a table to avoid repeating one function
# and one Button(...) line per histogram.
# Each entry: (button label, ROOT macro base name, x, y).
hist_font = ("Arial", 9)

hist_nonoise = [
    ("\n TOA with CFD \n",    "histTOA_CFD",   60, 300),
    ("\n TOA with LE \n",     "histTOA_LE",   175, 300),
    ("\n TOA with ARC \n",    "histTOA_RM",   280, 300),
    ("\n dVdt with CFD \n",   "histdvdt_CFD", 395, 300),
    ("\n dVdt with LE  \n",   "histdvdt_LE",  510, 300),
    ("\n dVdt with ARC \n",   "histdvdt_RM",  618, 300),
    ("\n  Time at peak  \n",  "histTpeak",     60, 360),
    ("\n Volt. at peak \n",   "histVpeak",    175, 360),
    ("\n V on Th (CFD) \n",   "histVonThCFD", 280, 360),
    ("\n   V on Th (LE)  \n", "histVonThLE",  395, 360),
    ("\n V on Th (RM) \n",    "histVonThRM",  510, 360),
]

for text, macro, x, y in hist_nonoise:
    Button(resW, text=text, command=lambda m=macro: open_macro(m), font=hist_font).place(x=x, y=y)

my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=460)

my_label7 = Label(resW, text='HISTOGRAMS WITH NOISE',font = ("Arial",9))
my_label7.place(x=20,y=470)

hist_noise = [
    ("\n TOA with CFD \n",    "histTOA_CFD_noise",   60, 500),
    ("\n TOA with LE \n",     "histTOA_LE_noise",   175, 500),
    ("\n TOA with ARC \n",    "histTOA_RM_noise",   280, 500),
    ("\n dVdt with CFD \n",   "histdvdt_CFD_noise", 395, 500),
    ("\n dVdt with LE  \n",   "histdvdt_LE_noise",  510, 500),
    ("\n dVdt with ARC \n",   "histdvdt_RM_noise",  618, 500),
    ("\n  Time at peak  \n",  "histTpeak_noise",     60, 560),
    ("\n Volt. at peak \n",   "histVpeak_noise",    175, 560),
    ("\n V on Th (CFD) \n",   "histVonThCFD_noise", 280, 560),
    ("\n   V on Th (LE)  \n", "histVonThLE_noise",  395, 560),
    ("\n V on Th (RM) \n",    "histVonThRM_noise",  510, 560),
    (" Jitter with CFD ",     "JitterCFD",          618, 560),
    ("   Jitter with LE  ",   "JitterLE",           618, 590),
    (" Jitter with ARC ",     "JitterRM",           618, 620),
]

for text, macro, x, y in hist_noise:
    Button(resW, text=text, command=lambda m=macro: open_macro(m), font=hist_font).place(x=x, y=y)




resW.mainloop()
