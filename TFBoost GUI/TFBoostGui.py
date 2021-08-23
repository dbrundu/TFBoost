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
 *  Created on: 25/07/2021
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
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import shutil
import re

root = tk.Tk();
root.title('TFBoost GUI')
root.geometry("800x750")
root.resizable(0, 0)
root.tk.call('wm', 'iconphoto', root._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

def quit_me():
    root.quit()
    root.destroy()
    exit()

root.protocol("WM_DELETE_WINDOW", quit_me)


global stat1,stat2,stat3,stat4,stat5,stat6
stat1 =IntVar()
stat2 =IntVar()
stat3 =IntVar()
stat4 =IntVar()
stat5 =IntVar()
stat6 =IntVar()

global CIN, CF, CL, Rp, Rf, Rc, gm, Gain, tau2
CIN = DoubleVar()
CF = DoubleVar()
CL = DoubleVar()
Rp = DoubleVar()
Rf = DoubleVar()
Rc = DoubleVar()
gm = DoubleVar()
Gain = DoubleVar()
tau2 = DoubleVar()

global Rn, RP, Cd, Ndz
RP = DoubleVar()
Rn = DoubleVar()
Cd = DoubleVar()
Ndz = DoubleVar()

global inputfile_selected
inputfile_selected = IntVar()
inputfile_selected.set(0)

global Nsamples, dT, LE_thr, CFD_thr, RM_delay, Bound_fit
Nsamples = DoubleVar()
dT = DoubleVar()
LE_thr = DoubleVar()
CFD_thr = DoubleVar()
RM_delay = DoubleVar()
Bound_fit = DoubleVar()

global Radice
Radice = StringVar()

def openFileInput():
    global folder_selected1
    folder_selected1 = filedialog.askdirectory()
    global directory0
    directory0 = os.path.split(folder_selected1)[0] + '/' + os.path.split(folder_selected1)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=250,y=112)
    text1.configure(state="normal")
    text1.insert('end', directory0)
    text1.configure(state="disabled")

    inputfile_selected.set(1)
    warn0 = Label(root, text='INPUT FOLDER SELECTED!               ',font = ("Modern",9), fg='green')
    warn0.place(x=20,y=645)

    fileslist = os.listdir(directory0)
    index_files = random.randrange(0, len(fileslist))
    global my_ext, line2skip
    my_ext = str(os.path.splitext(fileslist[index_files])[1])
    line2skip = StringVar()
    check_current()
    
    #check_btn = Button(root, text="Check",command=check_current,font = ("Arial",9),bg='SkyBlue3')
    #check_btn.place(x=680,y=112)
    

def openFileOutput():
    global folder_selected2
    folder_selected2 = filedialog.askdirectory()
    global directory
    directory = os.path.split(folder_selected2)[0] + '/' + os.path.split(folder_selected2)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=250,y=150)
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
    

def usesamecurve():
    global filechosen
    filechosen = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",8))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=195,y=233,width=200)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(filechosen.name)[1])
    text1.configure(state="disabled")

def FromFileopen():
    global FromFilePath
    FromFilePath = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",8))
    directoryFile = os.path.split(FromFilePath.name)[0] + '/' +os.path.split(FromFilePath.name)[1]
    text1 = Text(frameFF, state='disabled', width=50, height=1)
    text1.place (x=230,y=82,width=400)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(FromFilePath.name)[0] + '/' +os.path.split(FromFilePath.name)[1])
    text1.configure(state="disabled")
    
    check_TransferFunction()
    #check_btn = Button(frameFF, text="Check",command=check_TransferFunction,font = ("Arial",9),bg='SkyBlue3')
    #check_btn.place(x=230,y=80)

def Diamondopen():
    global DiamondPath
    DiamondPath = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",8)

def noisefromfiles():
    global folder_selected3
    folder_selected3 = filedialog.askdirectory()
    #filelabel = Label(root, text=filechosen,font = ("Arial",8))
    directory2 = os.path.split(folder_selected3)[0] + '/' + os.path.split(folder_selected3)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=160,y=470)
    text1.configure(state="normal")
    text1.insert('end', directory2)
    text1.configure(state="disabled")

def singlestg():


    stat1.set(1)
    stat2.set(0)
    stat3.set(0)
    stat4.set(0)
    stat5.set(0)
    stat6.set(0)

    if (logo3.winfo_exists() ):
        framedouble.place_forget()

    if (logo6.winfo_exists()):
        framecsa.place_forget()

    if (btnFromFile.winfo_exists()):
        frameFF.place_forget()

    if (btnRM4b.winfo_exists()):
        frameWA.place_forget()   

    if (btnRM3d.winfo_exists()):
        frame3D.place_forget()

    framesingle.place(x=20,y=170)

    # png circuit
    logo2 =Label(framesingle,image=my_logo2)
    logo2.place(x=20,y=0)
    

    TF_label2 = Label(framesingle, text='Single Stage TIA with Bipolar transistor',font = ("Modern",9))
    TF_label2.place(x=380,y=20,width=400)
    TF_label3 = Label(framesingle, text='only critical damping case is considered',font = ("Modern",9))
    TF_label3.place(x=380,y=40,width=400)

    #png equation
    logo4 =Label(framesingle,image=my_logo4)
    logo4.place(x=500,y=70)

    link1 = Label(framesingle, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=150,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))

    rpi1 =Label(framesingle,text='r   =',font = ("Modern",10))
    rpi2 =Label(framesingle,text=' \u03C0',font = ("Modern",6))
    rpi1.place(x=135,y=200)
    rpi2.place(x=140,y=210)
    global rpi_entry 
    rpi_entry = Entry(framesingle,font = ("Modern",10))
    rpi_entry.place(x=170,y=200,width=60)
    rpi_entry.insert(END,'220')
    rpi3 =Label(framesingle,text='\u03A9',font = ("Modern",10))
    rpi3.place(x=230,y=200)

    # R_F show
    rF1 =Label(framesingle,text='R    =',font = ("Modern",10))
    rF2 =Label(framesingle,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=230)
    rF2.place(x=140,y=240)
    global rF_entry
    rF_entry = Entry(framesingle,font = ("Modern",10))
    rF_entry.place(x=170,y=230,width=60)
    rF_entry.insert(END,'4300')
    rF3 =Label(framesingle,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=230)

    # R_C show
    rC1 =Label(framesingle,text='R    =',font = ("Modern",10))
    rC2 =Label(framesingle,text=' C',font = ("Modern",6))
    rC1.place(x=127,y=260)
    rC2.place(x=140,y=270)
    global rC_entry
    rC_entry = Entry(framesingle,font = ("Modern",10))
    rC_entry.place(x=170,y=260,width=60)
    rC_entry.insert(END,'50')
    rC3 =Label(framesingle,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=260)

    # C_in show
    Cin1 =Label(framesingle,text='C    =',font = ("Modern",10))
    Cin2 =Label(framesingle,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=290)
    Cin2.place(x=140,y=300)
    global Cin_entry
    Cin_entry = Entry(framesingle,font = ("Modern",10))
    Cin_entry.place(x=170,y=290,width=60)
    Cin_entry.insert(END,'2e-12')
    Cin3 =Label(framesingle,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=290)

    # C_f show
    Cf1 =Label(framesingle,text='C    =',font = ("Modern",10))
    Cf2 =Label(framesingle,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=320)
    Cf2.place(x=140,y=330)
    global Cf_entry
    Cf_entry = Entry(framesingle,font = ("Modern",10))
    Cf_entry.place(x=170,y=320,width=60)
    Cf_entry.insert(END,'50e-15')
    Cf3 =Label(framesingle,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=320)

    # C_L show
    CL1 =Label(framesingle,text='C    =',font = ("Modern",10))
    CL2 =Label(framesingle,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=350)
    CL2.place(x=140,y=360)
    global CL_entry
    CL_entry = Entry(framesingle,font = ("Modern",10))
    CL_entry.place(x=170,y=350,width=60)
    CL_entry.insert(END,'4e-12')
    CL3 =Label(framesingle,text='F',font = ("Modern",10))
    CL3.place(x=230,y=350)

    # gm show
    gm1 =Label(framesingle,text='g    =',font = ("Modern",10))
    gm2 =Label(framesingle,text='m',font = ("Modern",6))
    gm1.place(x=127,y=380)
    gm2.place(x=140,y=390)
    global gm_entry
    gm_entry = Entry(framesingle,font = ("Modern",10))
    gm_entry.place(x=170,y=380,width=60)
    gm_entry.insert(END,'0.5')
    gm3 =Label(framesingle,text='S',font = ("Modern",10))
    gm3.place(x=230,y=380)

    # R_m calculation
    rm1 =Label(framesingle,text='R     =',font = ("Modern",10))
    rm2 =Label(framesingle,text='m0',font = ("Modern",6))
    rm1.place(x=123,y=470)
    rm2.place(x=137,y=480)
    global rm_entry
    rm_entry = Entry(framesingle,font = ("Modern",10))
    rm_entry.place(x=170,y=470,width=75)
    rm3 =Label(framesingle,text='\u03A9',font = ("Modern",10))
    rm3.place(x=250,y=470)

    # tau calculation
    tau1 =Label(framesingle,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=123,y=500)
    global tau_entry
    tau_entry = Entry(framesingle,font = ("Modern",10))
    tau_entry.place(x=170,y=500,width=75)
    tau3 =Label(framesingle,text='s',font = ("Modern",10))
    tau3.place(x=250,y=500)

    global btnRM1
    btnRM1 = Button(framesingle, text="Get TIA Values\n and \n SET Transfer Function",command=rm,font = ("Arial",9))
    btnRM1.place(x=500,y=440)

    # NSAMPLES show
    nsamples =Label(framesingle,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(framesingle,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(framesingle,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(framesingle,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(framesingle,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)

    # Bound show
    bound1 =Label(framesingle,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=260)
    global bound_entry
    bound_entry = Entry(framesingle,font = ("Modern",10))
    bound_entry.place(x=550,y=260,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(framesingle,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=290)
    global LE_entry
    LE_entry = Entry(framesingle,font = ("Modern",10))
    LE_entry.place(x=550,y=290,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(framesingle,text='V',font = ("Modern",10))
    LE3.place(x=630,y=290)

    # CFD show
    CFD1 =Label(framesingle,text='CFD threshold fraction  =',font = ("Modern",10))
    CFD1.place(x=372,y=320)
    global CFD_entry
    CFD_entry = Entry(framesingle,font = ("Modern",10))
    CFD_entry.place(x=550,y=320,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(framesingle,text='ARC method delay  =',font = ("Modern",10))
    ARC1.place(x=397,y=350)
    global ARC_entry
    ARC_entry = Entry(framesingle,font = ("Modern",10))
    ARC_entry.place(x=550,y=350,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(framedouble,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=350)


def csastg():

    stat1.set(0)
    stat2.set(0)
    stat3.set(1)
    stat4.set(0)
    stat5.set(0)
    stat6.set(0)
        
    if (logo2.winfo_exists()):
        framesingle.place_forget()

    if (logo3.winfo_exists()):
        framedouble.place_forget()
  
    if (btnFromFile.winfo_exists()):
        frameFF.place_forget()

    if (btnRM4b.winfo_exists()):
        frameWA.place_forget()

    if (btnRM3d.winfo_exists()):
        frame3D.place_forget()

    framecsa.place(x=20,y=170)


    # png circuit
    logo6 =Label(framecsa,image=my_logo6)
    logo6.place(x=20,y=0)

    TF_label2 = Label(framecsa, text='Single Stage MOS Charge Sensitive Amplifier',font = ("Modern",9))
    TF_label2.place(x=380,y=20,width=400)
    TF_label3 = Label(framecsa, text='all damping behavior are possible with values provided',font = ("Modern",9))
    TF_label3.place(x=380,y=40,width=400)

    #png equation
    logo4 =Label(framecsa,image=my_logoCSA)
    logo4.place(x=460,y=75)

    link1 = Label(framecsa, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=150,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))


    # R_F show
    rF1 =Label(framecsa,text='R    =',font = ("Modern",10))
    rF2 =Label(framecsa,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=230)
    rF2.place(x=140,y=240)
    global rF_entry
    rF_entry = Entry(framecsa,font = ("Modern",10))
    rF_entry.place(x=170,y=230,width=60)
    rF_entry.insert(END,'3e6')
    rF3 =Label(framecsa,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=230)

    # R_C show
    rC1 =Label(framecsa,text='R    =',font = ("Modern",10))
    rC2 =Label(framecsa,text=' C',font = ("Modern",6))
    rC1.place(x=127,y=260)
    rC2.place(x=140,y=270)
    global rC_entry
    rC_entry = Entry(framecsa,font = ("Modern",10))
    rC_entry.place(x=170,y=260,width=60)
    rC_entry.insert(END,'570e3')
    rC3 =Label(framecsa,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=260)

    # C_in show
    Cin1 =Label(framecsa,text='C    =',font = ("Modern",10))
    Cin2 =Label(framecsa,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=290)
    Cin2.place(x=140,y=300)
    global Cin_entry
    Cin_entry = Entry(framecsa,font = ("Modern",10))
    Cin_entry.place(x=170,y=290,width=60)
    Cin_entry.insert(END,'100e-15')
    Cin3 =Label(framecsa,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=290)

    # C_f show
    Cf1 =Label(framecsa,text='C    =',font = ("Modern",10))
    Cf2 =Label(framecsa,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=320)
    Cf2.place(x=140,y=330)
    global Cf_entry
    Cf_entry = Entry(framecsa,font = ("Modern",10))
    Cf_entry.place(x=170,y=320,width=60)
    Cf_entry.insert(END,'5e-15')
    Cf3 =Label(framecsa,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=320)

    # C_L show
    CL1 =Label(framecsa,text='C    =',font = ("Modern",10))
    CL2 =Label(framecsa,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=350)
    CL2.place(x=140,y=360)
    global CL_entry
    CL_entry = Entry(framecsa,font = ("Modern",10))
    CL_entry.place(x=170,y=350,width=60)
    CL_entry.insert(END,'21e-15')
    CL3 =Label(framecsa,text='F',font = ("Modern",10))
    CL3.place(x=230,y=350)

    # gm show
    gm1 =Label(framecsa,text='g    =',font = ("Modern",10))
    gm2 =Label(framecsa,text='m',font = ("Modern",6))
    gm1.place(x=127,y=380)
    gm2.place(x=140,y=390)
    global gm_entry
    gm_entry = Entry(framecsa,font = ("Modern",10))
    gm_entry.place(x=170,y=380,width=60)
    gm_entry.insert(END,'55e-6')
    gm3 =Label(framecsa,text='S',font = ("Modern",10))
    gm3.place(x=230,y=380)

    # R_m calculation
    rm1 =Label(framecsa,text='R     =',font = ("Modern",10))
    rm2 =Label(framecsa,text='m0',font = ("Modern",6))
    rm1.place(x=23,y=440)
    rm2.place(x=37,y=450)
    global rm_entry
    rm_entry = Entry(framecsa,font = ("Modern",10))
    rm_entry.place(x=70,y=440,width=75)
    rm3 =Label(framecsa,text='\u03A9',font = ("Modern",10))
    rm3.place(x=150,y=440)

    rm_entry = Entry(framecsa,font = ("Modern",10))
    rm_entry.place(x=70,y=440,width=75)
    rm3 =Label(framecsa,text='\u03A9',font = ("Modern",10))
    rm3.place(x=150,y=440)

    # tau calculation
    tau1 =Label(framecsa,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=23,y=470)
    global tau_entry
    tau_entry = Entry(framecsa,font = ("Modern",10))
    tau_entry.place(x=70,y=470,width=75)
    tau3 =Label(framecsa,text='s',font = ("Modern",10))
    tau3.place(x=150,y=470)

    # zeta calculation
    zeta1 =Label(framecsa,text='\u03B6    =',font = ("Modern",10))
    zeta1.place(x=23,y=500)
    global zeta_entry
    zeta_entry = Entry(framecsa,font = ("Modern",10))
    zeta_entry.place(x=70,y=500,width=75)
  
    global btnRM
    btnRM = Button(framecsa, text="Get CSA Values\n and \n SET Transfer Function",command=rmcsa,font = ("Arial",9))
    btnRM.place(x=500,y=440)

    # NSAMPLES show
    nsamples =Label(framecsa,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(framecsa,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'262144')

    # dT show
    dt1 =Label(framecsa,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(framecsa,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(framecsa,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)

    # Bound show
    bound1 =Label(framecsa,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=260)
    global bound_entry
    bound_entry = Entry(framecsa,font = ("Modern",10))
    bound_entry.place(x=550,y=260,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(framecsa,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=290)
    global LE_entry
    LE_entry = Entry(framecsa,font = ("Modern",10))
    LE_entry.place(x=550,y=290,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(framecsa,text='V',font = ("Modern",10))
    LE3.place(x=630,y=290)

    # CFD show
    CFD1 =Label(framecsa,text='CFD threshold fraction  =',font = ("Modern",10))
    CFD1.place(x=372,y=320)
    global CFD_entry
    CFD_entry = Entry(framecsa,font = ("Modern",10))
    CFD_entry.place(x=550,y=320,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(framecsa,text='ARC method delay  =',font = ("Modern",10))
    ARC1.place(x=397,y=350)
    global ARC_entry
    ARC_entry = Entry(framecsa,font = ("Modern",10))
    ARC_entry.place(x=550,y=350,width=70)
    ARC_entry.insert(END,'5e-9')
    ARC1 =Label(framecsa,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=350)
    

def FromFile():
    
    stat1.set(0)
    stat2.set(0)
    stat3.set(0)
    stat4.set(1)
    stat5.set(0)
    stat6.set(0)

    if (logo2.winfo_exists()):
        framesingle.place_forget()

    if (logo3.winfo_exists()):
        framedouble.place_forget()

    if (logo6.winfo_exists()):
        framecsa.place_forget()

    if (btnRM4b.winfo_exists()):
        frameWA.place_forget()

    if (btnRM3d.winfo_exists()):
        frame3D.place_forget()
  
    
    frameFF.place(x=0,y=170)


    lab2 = Label(frameFF, text='Perform convolution with TFBoost using a transfer function from an existing file.',font = ("Modern",9))
    lab2.place(x=20,y=5)
    lab2b = Label(frameFF, text='The transfer function can be calculated using the "Make Deconvolution" option.',font = ("Modern",9))
    lab2b.place(x=20,y=25)
    lab2c = Label(frameFF, text='If the timestep is different than the one of the input files, use the "Make Resampling" option.',font = ("Modern",9))
    lab2c.place(x=20,y=45)


    btnFromFile.place(x=20,y=80)
    btnRM3.place(x=50,y=350)

    btn_dec.place(x=50,y=250)

    btn_samp.place(x=50,y=300)

    # NSAMPLES show
    nsamples =Label(frameFF,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(frameFF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(frameFF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(frameFF,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(frameFF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)

    # Bound show
    bound1 =Label(frameFF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=260)
    global bound_entry
    bound_entry = Entry(frameFF,font = ("Modern",10))
    bound_entry.place(x=550,y=260,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(frameFF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=290)
    global LE_entry
    LE_entry = Entry(frameFF,font = ("Modern",10))
    LE_entry.place(x=550,y=290,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(frameFF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=290)

    # CFD show
    CFD1 =Label(frameFF,text='CFD threshold fraction  =',font = ("Modern",10))
    CFD1.place(x=372,y=320)
    global CFD_entry
    CFD_entry = Entry(frameFF,font = ("Modern",10))
    CFD_entry.place(x=550,y=320,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(frameFF,text='ARC method delay  =',font = ("Modern",10))
    ARC1.place(x=397,y=350)
    global ARC_entry
    ARC_entry = Entry(frameFF,font = ("Modern",10))
    ARC_entry.place(x=550,y=350,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(frameFF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=350)
    

def WaveAnalysis():

    stat1.set(0)
    stat2.set(0)
    stat3.set(0)
    stat4.set(0)
    stat5.set(1)
    stat6.set(0)


    if (logo2.winfo_exists()):
        framesingle.place_forget()

    if (logo3.winfo_exists()):
        framedouble.place_forget()

    if (logo6.winfo_exists()):
        framecsa.place_forget()

    if (btnRM3.winfo_exists()):
        frameFF.place_forget()

    if (btnRM3d.winfo_exists()):
        frame3D.place_forget()
  
    
    frameWA.place(x=20,y=170)

    lab2 = Label(frameWA, text='Perform a waveform analysis with TFBoost. No convolution is done, only the measurements.',font = ("Modern",9))
    lab2.place(x=20,y=5)
    lab2c = Label(frameWA, text='make sure the time step is the same of the input files',font = ("Modern",9))
    lab2c.place(x=20,y=25)

    btnRM4b.place(x=50,y=300)

    # NSAMPLES show
    nsamples =Label(frameWA,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(frameWA,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(frameWA,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(frameWA,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(frameWA,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)

        # Bound show
    bound1 =Label(frameWA,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=260)
    global bound_entry
    bound_entry = Entry(frameWA,font = ("Modern",10))
    bound_entry.place(x=550,y=260,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(frameWA,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=290)
    global LE_entry
    LE_entry = Entry(frameWA,font = ("Modern",10))
    LE_entry.place(x=550,y=290,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(frameWA,text='V',font = ("Modern",10))
    LE3.place(x=630,y=290)

    # CFD show
    CFD1 =Label(frameWA,text='CFD threshold fraction  =',font = ("Modern",10))
    CFD1.place(x=372,y=320)
    global CFD_entry
    CFD_entry = Entry(frameWA,font = ("Modern",10))
    CFD_entry.place(x=550,y=320,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(frameWA,text='ARC method delay  =',font = ("Modern",10))
    ARC1.place(x=397,y=350)
    global ARC_entry
    ARC_entry = Entry(frameWA,font = ("Modern",10))
    ARC_entry.place(x=550,y=350,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(frameWA,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=350)
  
    


def diamond():

    stat1.set(0)
    stat2.set(0)
    stat3.set(0)
    stat4.set(0)
    stat5.set(0)
    stat6.set(1)


    if (logo2.winfo_exists()):
        framesingle.place_forget()

    if (logo3.winfo_exists()):
        framedouble.place_forget()

    if (logo6.winfo_exists()):
        framecsa.place_forget()

    if (btnRM3.winfo_exists()):
        frameFF.place_forget()

    if (btnRM4b.winfo_exists()):
        frameWA.place_forget()
  
    
    frame3D.place(x=20,y=170)


    lab2 = Label(frame3D, text='Perform the convolution of a set of currents of an ideal 3D Diamond Detector ',font = ("Modern",9))
    lab2.place(x=20,y=5)
    lab2c = Label(frame3D, text='with a transfer function that consider highly resistive columns electrodes.',font = ("Modern",9))
    lab2c.place(x=20,y=25)
    link1 = Label(frame3D, text='click here for a description of the model.', fg="blue", cursor="hand2",font = ("Modern",9))
    link1.place(x=20,y=45)
    link1.bind("<Button-1>", lambda e: callback("https://raw.githubusercontent.com/gianmatteocossu/TFBoost/master/3dDiamondModel.pdf"))

    btnRM3d.place(x=440,y=300)
    

    # R_n show
    Rn1 =Label(frame3D,text='R    =',font = ("Modern",10))
    Rn2 =Label(frame3D,text=' n',font = ("Modern",6))
    Rn1.place(x=27,y=110)
    Rn2.place(x=40,y=120)
    global Rn_entry
    Rn_entry = Entry(frame3D,font = ("Modern",10))
    Rn_entry.place(x=70,y=110,width=60)
    Rn_entry.insert(END,'100e3')
    Rn3 =Label(frame3D,text='\u03A9',font = ("Modern",10))
    Rn3.place(x=130,y=110)


    # R_p show
    Rp1 =Label(frame3D,text='R    =',font = ("Modern",10))
    Rp2 =Label(frame3D,text=' p',font = ("Modern",6))
    Rp1.place(x=27,y=140)
    Rp2.place(x=40,y=150)
    global Rp_entry
    Rp_entry = Entry(frame3D,font = ("Modern",10))
    Rp_entry.place(x=70,y=140,width=60)
    Rp_entry.insert(END,'100e3')
    Rp3 =Label(frame3D,text='\u03A9',font = ("Modern",10))
    Rp3.place(x=130,y=140)


    # Cd show
    Cd1 =Label(frame3D,text='C    =',font = ("Modern",10))
    Cd2 =Label(frame3D,text=' D',font = ("Modern",6))
    Cd1.place(x=27,y=170)
    Cd2.place(x=40,y=180)
    global Cd_entry
    Cd_entry = Entry(frame3D,font = ("Modern",10))
    Cd_entry.place(x=70,y=170,width=60)
    Cd_entry.insert(END,'30e-15')
    Cd3 =Label(frame3D,text='F',font = ("Modern",10))
    Cd3.place(x=130,y=170)

    # Ndz show
    ndz1 =Label(frame3D,text=' Ndz =',font = ("Modern",10))
    ndz1.place(x=27,y=200)
    global ndz_entry
    ndz_entry = Entry(frame3D,font = ("Modern",10))
    ndz_entry.place(x=70,y=200,width=60)
    ndz_entry.insert(END,'20')
    
    # Zin
    zin1 =Label(frame3D,text='Zin:',font = ("Modern",10))
    zin1.place(x=27,y=250)
    global varzin
    varzin =IntVar()
    varzin.set(0)
    zinb =Checkbutton(frame3D,variable=varzin,font = ("Modern",10))
    zinb.place(x=150,y=250)

    # Zin button input file
    btnZin = Button(frame3D, text="Zin pulse response file",command=Diamondopen,font = ("Arial",9))
    btnZin.place(x=27,y=275)

    # Arbitrary tracks
    arb1 =Label(frame3D,text='Arbitrary Tracks:',font = ("Modern",10))
    arb1.place(x=27,y=310)
    global varArb
    varArb =IntVar()
    varArb.set(1)
    arbb =Checkbutton(frame3D,variable=varArb,font = ("Modern",10))
    arbb.place(x=150,y=310)

    # Number of Tracks
    tra1 =Label(frame3D,text='Number of Tracks:',font = ("Modern",10))
    tra1.place(x=27,y=330)
    global trab
    trab =Entry(frame3D,font = ("Modern",10))
    trab.place(x=158,y=330,width=40)
    trab.insert(END,'1000')

    # NSAMPLES show
    nsamples =Label(frame3D,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(frame3D,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(frame3D,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(frame3D,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(frame3D,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)


#      rpi,Rf,Rc,gm
def rm() :
    a1=float(rpi_entry.get())
    a2=float(rF_entry.get())
    a3=float(rC_entry.get())
    a4=float(gm_entry.get())
    y=((a1*a2*a3*a4-a1*a3)/(a2+a3+a1*(1+a4*a3)))

    c1=float(Cin_entry.get())
    c2=float(Cf_entry.get())
    c3=float(CL_entry.get())

    z = c1*c2+c1*c3 +c2*c3

    tau = math.sqrt((a1*a2*a3*z)/(a2+a3+a1*(1+a4*a3)))
    
    if(stat2.get()==1):
        G = float(G_entry.get())
        gmr_entry.delete(0,END)
        gmr_entry.insert(END,int(y)*G)

    rm_entry.delete(0,END)
    rm_entry.insert(END,int(y))
    tau_entry.delete(0,END)
    tau_entry.insert(END,format(tau, "3.2e"))


    # SAVE ALL VARIABLES
    CIN.set(Cin_entry.get())
    CF.set(Cf_entry.get())
    CL.set(CL_entry.get())

    Rp.set(rpi_entry.get())
    Rf.set(rF_entry.get())
    Rc.set(rC_entry.get())

    gm.set(gm_entry.get())
    Gain.set(G_entry.get())
    tau2.set(tauo_entry.get())

    Nsamples.set(nsamples_entry.get())
    Bound_fit.set(bound_entry.get())
    dT.set(dt_entry.get())

    LE_thr.set(LE_entry.get())
    CFD_thr.set(CFD_entry.get())
    RM_delay.set(ARC_entry.get())



#      rpi,Rf,Rc,gm
def rmcsa() :
    
    a2=float(rF_entry.get())
    a3=float(rC_entry.get())
    a4=float(gm_entry.get())

    rp=a2*a3/(a2+a3)

    g=a4*rp-rp/a2
    
    y=a2*(g/(1+g))
    
    c1=float(Cin_entry.get())
    c2=float(Cf_entry.get())
    c3=float(CL_entry.get())

    z = c1*c2+c1*c3 +c2*c3

    tau = math.sqrt((rp*a2*z)/(1+g))


    zeta = 0.5 * (a2*(c1+c2*(1+g))+rp*c3)/(math.sqrt((rp*a2*z)*(1+g)))

    tau = tau/zeta

    my_formatter = "{0:.2f}"
    zeta_entry.delete(0,END)
    zeta_entry.insert(END,my_formatter.format(zeta))

    # label for dumping condition
    damp1 =Label(framecsa,text='Condition:',font = ("Modern",10))
    damp1.place(x=200,y=440)

    if (zeta > 1.2):
        cond='OVERDAMPED                       \n                          '
        damp2 =Label(framecsa,text=cond,font = ("Modern",11), fg='red')
        damp2.place(x=270,y=440)

    if (zeta < 0.8):
        cond='UNDERDAMPED                      \n                          '
        damp2 =Label(framecsa,text=cond,font = ("Modern",11), fg='red')
        damp2.place(x=270,y=440)

    if (zeta > 0.8 and zeta < 1.2):
        cond='CRITICALLY DAMPED\n(0.8<\u03B6<1.2)'
        damp2 =Label(framecsa,text=cond,font = ("Modern",11), fg='red')
        damp2.place(x=270,y=440)
    
    if(stat2==1):
        G = float(G_entry.get())
        gmr_entry.delete(0,END)
        gmr_entry.insert(END,int(y)*G)

    rm_entry.delete(0,END)
    rm_entry.insert(END,format(int(y),"3.2e"))
    tau_entry.delete(0,END)
    tau_entry.insert(END,format(tau, "3.2e"))

        # SAVE ALL VARIABLES
    CIN.set(Cin_entry.get())
    CF.set(Cf_entry.get())
    CL.set(CL_entry.get())

    Rp.set(rpi_entry.get())
    Rf.set(rF_entry.get())
    Rc.set(rC_entry.get())

    gm.set(gm_entry.get())
    Gain.set(G_entry.get())
    tau2.set(tauo_entry.get())

    Nsamples.set(nsamples_entry.get())
    Bound_fit.set(bound_entry.get())
    dT.set(dt_entry.get())

    LE_thr.set(LE_entry.get())
    CFD_thr.set(CFD_entry.get())
    RM_delay.set(ARC_entry.get())


#      rpi,Rf,Rc,gm
def write3D() :

    global Radice

    # SAVE ALL VARIABLES
    Rn.set(Rn_entry.get())
    RP.set(Rp_entry.get())
    Cd.set(Cd_entry.get())

    Nsamples.set(nsamples_entry.get())
    dT.set(dt_entry.get())
    Ndz.set(ndz_entry.get())

    text_file = open("../etc/3Ddiamond.cfg", 'w+')

    text_file.write('\nConvolution        =true;\n')
    text_file.write('\nInputDirectory     =' + '"' + folder_selected1 + '";\n')
    text_file.write('\nOutputDirectory    ="/home/osboxes/Desktop/TFBoost/build/3Ddiamond";\n')

    text_file.write('\ndT =' + str(dT.get())  +';\n') 
    text_file.write('Nsamples             = ' + str(int(Nsamples.get())) + ';\n')
    
    text_file.write('Rn                   =' + str(Rn.get()) + ';\n')
    text_file.write('Rp                   =' + str(RP.get()) + ';\n')
    text_file.write('Cd                   =' + str(Cd.get()) + ';\n')
    text_file.write('Ndz                  =' + str(int(Ndz.get())) + ';\n')
    
    if varzin.get() == 0:
        text_file.write('\nZin            = false;\n') 
        text_file.write('ZinFile         = "/home/osboxes/Desktop/TFBoost/build/Zin_files/zin_kansas.txt";\n')
    if varzin.get() == 1:
        text_file.write('\nZin            = true;\n') 
        text_file.write('ZinFile          = "' + os.path.split(DiamondPath.name)[0] + '/' +os.path.split(DiamondPath.name)[1] + '";\n') 

    if varArb.get() == 0:
        text_file.write('\nArbTracks      = false;\n') 
    if varArb.get() == 1:
        text_file.write('\nArbTracks      = true;\n')  

    text_file.write('NTracks              =' + str(int(trab.get())) + ';\n')
    text_file.write('Radice               ="' + Radice + '";\n')

    text_file.write('\nSumDirectory       = "/home/osboxes/Desktop/TFBoost/build/3Ddiamond/H3D_Currents/data_arbitrary";  \n')
    text_file.write('\ntoken              =" ";  \n')
    text_file.write('columnT              =0;  \n')
    text_file.write('columnI              =1;  \n')
    text_file.write('columnIe             =2;  \n')
    text_file.write('columnIh             =3;  \n')
    text_file.write('\nNlinestoskip       = 0;  \n')

    text_file.close()

    subprocess.run(["./../build/3Ddiamond_tbb", "ls -l"])


def doublestg():

    
    stat1.set(0)
    stat2.set(1)
    stat3.set(0)
    stat4.set(0)
    stat5.set(0) 
    stat6.set(0)


    if (logo4.winfo_exists()):
        framesingle.place_forget()

    if (logo6.winfo_exists()):
        framecsa.place_forget()

    if (btnFromFile.winfo_exists()):
        frameFF.place_forget()

    if (btnRM4b.winfo_exists()):
        frameWA.place_forget()

    if (btnRM3d.winfo_exists()):
        frame3D.place_forget()
  
    framedouble.place(x=20,y=170)



    # r_pi show
    global rpi1,rpi2,rpi3, rF1,rF2,rF3
    global rC1,rC2,rC3, Cin1,Cin2,Cin3
    global Cf1,Cf2,Cf3, CL1,CL2,CL3
    global gm1,gm2,gm3, G1,G2
    global tauo1,tauo2,tauo3, rm1,rm2,rm3
    global tau1,tau2,tau3, grm1,grm2,grm3
    global gG1,gG2

    # png circuit
    logo2 =Label(framedouble,image=my_logo2)
    logo2.place(x=20,y=0)

    TF_label2 = Label(framedouble, text='Double Stage TIA defined as a single stage TIA',font = ("Modern",9))
    TF_label2.place(x=380,y=20,width=400)
    TF_label3 = Label(framedouble, text='followed by a single pole voltage gain transfer function',font = ("Modern",9))
    TF_label3.place(x=380,y=40,width=400)

    #png equation
    logo3 =Label(framedouble,image=my_logo3)
    logo3.place(x=470,y=70)

    link1 = Label(framedouble, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=150,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))


    rpi1 =Label(framedouble,text='r   =',font = ("Modern",10))
    rpi2 =Label(framedouble,text=' \u03C0',font = ("Modern",6))
    rpi1.place(x=135,y=200)
    rpi2.place(x=140,y=210)
    global rpi_entry 
    rpi_entry = Entry(framedouble,font = ("Modern",10))
    rpi_entry.place(x=170,y=200,width=60)
    rpi_entry.insert(END,'220')
    rpi3 =Label(framedouble,text='\u03A9',font = ("Modern",10))
    rpi3.place(x=230,y=200)

    # R_F show
    rF1 =Label(framedouble,text='R    =',font = ("Modern",10))
    rF2 =Label(framedouble,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=230)
    rF2.place(x=140,y=240)
    global rF_entry
    rF_entry = Entry(framedouble,font = ("Modern",10))
    rF_entry.place(x=170,y=230,width=60)
    rF_entry.insert(END,'4300')
    rF3 =Label(framedouble,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=230)

    # R_C show
    rC1 =Label(framedouble,text='R    =',font = ("Modern",10))
    rC2 =Label(framedouble,text=' C',font = ("Modern",6))
    rC1.place(x=127,y=260)
    rC2.place(x=140,y=270)
    global rC_entry
    rC_entry = Entry(framedouble,font = ("Modern",10))
    rC_entry.place(x=170,y=260,width=60)
    rC_entry.insert(END,'50')
    rC3 =Label(framedouble,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=260)

    # C_in show
    Cin1 =Label(framedouble,text='C    =',font = ("Modern",10))
    Cin2 =Label(framedouble,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=290)
    Cin2.place(x=140,y=300)
    global Cin_entry
    Cin_entry = Entry(framedouble,font = ("Modern",10))
    Cin_entry.place(x=170,y=290,width=60)
    Cin_entry.insert(END,'2e-12')
    Cin3 =Label(framedouble,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=290)

    # C_f show
    Cf1 =Label(framedouble,text='C    =',font = ("Modern",10))
    Cf2 =Label(framedouble,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=320)
    Cf2.place(x=140,y=330)
    global Cf_entry
    Cf_entry = Entry(framedouble,font = ("Modern",10))
    Cf_entry.place(x=170,y=320,width=60)
    Cf_entry.insert(END,'50e-15')
    Cf3 =Label(framedouble,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=320)

    # C_L show
    CL1 =Label(framedouble,text='C    =',font = ("Modern",10))
    CL2 =Label(framedouble,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=350)
    CL2.place(x=140,y=360)
    global CL_entry
    CL_entry = Entry(framedouble,font = ("Modern",10))
    CL_entry.place(x=170,y=350,width=60)
    CL_entry.insert(END,'4e-12')
    CL3 =Label(framedouble,text='F',font = ("Modern",10))
    CL3.place(x=230,y=350)

    # gm show
    gm1 =Label(framedouble,text='g    =',font = ("Modern",10))
    gm2 =Label(framedouble,text='m',font = ("Modern",6))
    gm1.place(x=127,y=380)
    gm2.place(x=140,y=390)
    global gm_entry
    gm_entry = Entry(framedouble,font = ("Modern",10))
    gm_entry.place(x=170,y=380,width=60)
    gm_entry.insert(END,'0.5')
    gm3 =Label(framedouble,text='S',font = ("Modern",10))
    gm3.place(x=230,y=380)

    # G0 show
    global G1
    global G2
    G1 =Label(framedouble,text='G    =',font = ("Modern",10))
    G2 =Label(framedouble,text='0',font = ("Modern",6))
    G1.place(x=127,y=410)
    G2.place(x=140,y=420)
    global G_entry
    G_entry = Entry(framedouble,font = ("Modern",10))
    G_entry.place(x=170,y=410,width=60)
    G_entry.insert(END,'10')
    #G3 =Label(framedouble,text='S',font = ("Modern",10))
    #G3.place(x=230,y=590)

    # tau2 show
    global tauo_entry
    tauo1 =Label(framedouble,text='\u03C4    =',font = ("Modern",10))
    tauo2 =Label(framedouble,text='*',font = ("Modern",7))
    tauo1.place(x=130,y=440)
    tauo2.place(x=140,y=438)
    tauo_entry = Entry(framedouble,font = ("Modern",10))
    tauo_entry.place(x=170,y=440,width=75)
    tauo_entry.insert(END,'150e-12')
    tauo3 =Label(framedouble,text='s',font = ("Modern",10))
    tauo3.place(x=250,y=440)

    # R_m calculation
    rm1 =Label(framedouble,text='R     =',font = ("Modern",10))
    rm2 =Label(framedouble,text='m0',font = ("Modern",6))
    rm1.place(x=123,y=470)
    rm2.place(x=137,y=480)
    global rm_entry
    rm_entry = Entry(framedouble,font = ("Modern",10))
    rm_entry.place(x=170,y=470,width=75)
    rm3 =Label(framedouble,text='\u03A9',font = ("Modern",10))
    rm3.place(x=250,y=470)

    # tau calculation
    tau1 =Label(framedouble,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=123,y=500)
    global tau_entry
    tau_entry = Entry(framedouble,font = ("Modern",10))
    tau_entry.place(x=170,y=500,width=75)
    tau3 =Label(framedouble,text='s',font = ("Modern",10))
    tau3.place(x=250,y=500)

    # G0Rm0 calculation

    grm1 =Label(framedouble,text='R     =',font = ("Modern",10))
    grm2 =Label(framedouble,text='m0',font = ("Modern",6))
    grm1.place(x=123,y=530)
    grm2.place(x=137,y=540)
    gG1 =Label(framedouble,text='G',font = ("Modern",10))
    gG2 =Label(framedouble,text='0',font = ("Modern",6))
    gG1.place(x=100,y=530)
    gG2.place(x=115,y=540)
    global gmr_entry
    gmr_entry = Entry(framedouble,font = ("Modern",10))
    gmr_entry.place(x=170,y=530,width=75)
    grm3 =Label(framedouble,text='\u03A9',font = ("Modern",10))
    grm3.place(x=250,y=530)
    
    global btnRM
    btnRM = Button(framedouble, text="Get TIA Values\n and \n SET Transfer Function",command=rm,font = ("Arial",9))
    btnRM.place(x=500,y=440)

    # NSAMPLES show
    nsamples =Label(framedouble,text='N° Samples  =',font = ("Modern",10))
    nsamples.place(x=440,y=200)
    global nsamples_entry 
    nsamples_entry = Entry(framedouble,font = ("Modern",10))
    nsamples_entry.place(x=550,y=200,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(framedouble,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=230)
    global dt_entry
    dt_entry = Entry(framedouble,font = ("Modern",10))
    dt_entry.place(x=550,y=230,width=70)
    dt_entry.insert(0,format(step,"3.2e"))
    dt3 =Label(framedouble,text='s',font = ("Modern",10))
    dt3.place(x=630,y=230)

    # Bound show
    bound1 =Label(framedouble,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=260)
    global bound_entry
    bound_entry = Entry(framedouble,font = ("Modern",10))
    bound_entry.place(x=550,y=260,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(framedouble,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=290)
    global LE_entry
    LE_entry = Entry(framedouble,font = ("Modern",10))
    LE_entry.place(x=550,y=290,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(framedouble,text='V',font = ("Modern",10))
    LE3.place(x=630,y=290)

    # CFD show
    CFD1 =Label(framedouble,text='CFD threshold fraction  =',font = ("Modern",10))
    CFD1.place(x=372,y=320)
    global CFD_entry
    CFD_entry = Entry(framedouble,font = ("Modern",10))
    CFD_entry.place(x=550,y=320,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(framedouble,text='ARC method delay  =',font = ("Modern",10))
    ARC1.place(x=397,y=350)
    global ARC_entry
    ARC_entry = Entry(framedouble,font = ("Modern",10))
    ARC_entry.place(x=550,y=350,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(framedouble,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=350)

    
   

 #
def openTFgui():

    if(inputfile_selected.get()==1):

        global TF
        TF = Toplevel(root)
        TF.geometry("800x750")
        TF.resizable(0, 0)
        TF.tk.call('wm', 'iconphoto', TF._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

        TF.title("Configure Transfer Function") 
        TF_label = Label(TF, text='SELECT TRANSFER FUNCTION',font = ("Modern",9))
        TF_label.place(x=20,y=10)
        my_canvas = Canvas(TF,width=750,height=1,bg='black')
        my_canvas.place(x=20,y=27)
        
        btn1 = Button(TF, text="Single stage TIA",command=singlestg,font = ("Arial",12))
        btn1.place(x=100,y=50)

        btn2 = Button(TF, text="Dual stage TIA        ",command=doublestg,font = ("Arial",12))
        btn2.place(x=260,y=50)

        btn3 = Button(TF, text="Charge Sensitive Amplifier",command=csastg,font = ("Arial",12), width=20)
        btn3.place(x=435,y=50)

        btn4 = Button(TF, text="From File             ",command=FromFile, font = ("Arial",12))
        btn4.place(x=100,y=100)

        btn5 = Button(TF, text="Waveform Analysis", command=WaveAnalysis, font = ("Arial",12))
        btn5.place(x=260,y=100)

        btn6 = Button(TF, text="3D Diamond model",command=diamond,font = ("Arial",12), width=20)
        btn6.place(x=435,y=100)

        my_canvas1 = Canvas(TF,width=750,height=1,bg='black')
        my_canvas1.place(x=20,y=150)

        
        global framesingle
        framesingle = Frame(TF,height=630,width=780)

        global framedouble
        framedouble = Frame(TF,height=630,width=780)

        global framecsa
        framecsa = Frame(TF,height=630,width=780)

        global frameFF
        frameFF = Frame(TF,height=630,width=780)

        global frameWA
        frameWA = Frame(TF,height=630,width=780)

        global frame3D
        frame3D = Frame(TF,height=630,width=780)

        global logo2, logo3, link1, TF_label2, TF_label3

        global btnRM2
        btnRM2 = Button(TF, text="Get CSA Values\n and \n SET Transfer Function",command=rmcsa,font = ("Arial",9))

        global FF_label
        FF_label = Label(frameFF, text='',font = ("Modern",9))

        global btnRM3
        btnRM3 = Button(frameFF, text="SET \nTransfer Function",command=rm,font = ("Arial",9), width=15)
    
        global btn_dec
        btn_dec = Button(frameFF, text="Make \n   Deconvolution",command=deconv,font = ("Arial",9), width=15)

        global btn_samp
        btn_samp = Button(frameFF, text="Make \n   Resampling",command=resamp,font = ("Arial",9), width=15)

        global btnRM4b
        btnRM4b = Button(frameWA, text="SET \nWaveform Analysis",command=rm,font = ("Arial",9))

        global btnRM3d
        btnRM3d = Button(frame3D, text="Calculate Transfer Functions \n and Make Analysis \n or Make Currents Sum",command=write3D,font = ("Arial",9))

        global btnFromFile
        btnFromFile = Button(frameFF, text="Choose Transfer Function file",command=FromFileopen,font = ("Arial",9))

        global my_logo2
        my_logo2 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/singlestage.png"))
        global logo2
        logo2 =Label(TF,image=my_logo2)
        #logo2.place(x=20,y=170)

        global my_logo3
        my_logo3 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/doublestage_eq.png"))
        global logo3
        logo3 =Label(TF,image=my_logo3)
        #logo3.place(x=450,y=230)

        global my_logo4
        my_logo4 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/singlestage_eq.png"))
        global my_logoCSA
        my_logoCSA = ImageTk.PhotoImage(Image.open("TFB_guiFiles/csa_eq.png"))
        global logo4
        logo4 =Label(TF,image=my_logo4)

        global my_logo5
        my_logo5 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/eq_background.png"))
        global logo5
        logo5 =Label(TF,image=my_logo5)

        global my_logo7
        my_logo7 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/eq_background.png"))
        global logo7
        logo7 =Label(TF,image=my_logo7) #un altro sfondo

        global my_logo6
        my_logo6 = ImageTk.PhotoImage(Image.open("TFB_guiFiles/csastage.png"))
        global logo6
        logo6 =Label(TF,image=my_logo6) 

        doublestg()

    if(inputfile_selected.get()==0):
        warn = Label(root, text='PLEASE SELECT AN INPUT FOLDER',font = ("Modern",9), fg='red')
        warn.place(x=20,y=645)
    TF.mainloop()


def plotcheck():

    plt.clf()

    global Lines2Skip_entry
    global line2skip
    line2skip = Lines2Skip_entry.get()

    path1 =folder_selected1
    files1 = os.listdir(path1)
    index1 = random.randrange(0, len(files1))
    print(files1[index1])

    global Radice

    first_digit = re.search('\d', files1[index1])
    Radice=str(files1[index1][0 : first_digit.start()])

    print(Radice)

    data1 = pd.read_csv(path1+'/'+files1[index1],sep='\s+',header=None,skiprows=int(line2skip))
    data1 = pd.DataFrame(data1)
   
    numcol = len(data1.columns)
    print(numcol)

    x1 = data1[0]
    y1 = data1[1]

    global step
    step = data1.at[11,0]-data1.at[10,0]

    Nsamp = len(data1)

    fig, ax = plt.subplots()

    fig.set_figheight(3)
    fig.set_figwidth(6)

    plt.plot(x1, y1, 'r-')
    plt.title('Results')
    plt.xlabel('time (s)')
    plt.ylabel('Current [A]')
    plt.tight_layout(pad=1.0)

    global canvas
    canvas = FigureCanvasTkAgg(fig, master=CC) # Convert the Figure to a tkinter widget
    canvas.get_tk_widget().place(x=100, y=40) # Show the widget on the screen
    canvas.draw() # Draw the graph on the canvas?

    entrystep = Entry(CC,font=  ("Arial",12))
    entrystep.place(x=370,y=350,width=90)    
    entrystep.insert(END,format(step,"3.2e"))

    entryNsamp = Entry(CC,font=  ("Arial",12))
    entryNsamp.place(x=370,y=390,width=90)    
    entryNsamp.insert(END,int(Nsamp))

    dT=step

    global ext_entry 

    ext_entry = Entry(CC,font=  ("Arial",12))
    ext_entry.place(x=370,y=430,width=90)    
    ext_entry .delete(0,END)
    ext_entry .insert(END,my_ext)

    plt.close()

def plotcheckTF():

    path1 =FromFilePath
    data1 = pd.read_csv(path1,sep='\s+',header=None)
    data1 = pd.DataFrame(data1)

    x1 = data1[0]
    y1 = data1[1]

    global stepTF
    stepTF = data1.at[11,0]-data1.at[10,0]

    Nsamp = len(data1)

    fig, ax = plt.subplots()

    fig.set_figheight(3)
    fig.set_figwidth(6)

    plt.plot(x1, y1, 'r-')
    plt.title('Results')
    plt.xlabel('time (s)')
    plt.ylabel('Transfer Function [\u03A9]')
    plt.tight_layout(pad=1.0)

    global canvas
    canvas = FigureCanvasTkAgg(fig, master=CTF) # Convert the Figure to a tkinter widget
    canvas.get_tk_widget().place(x=100, y=40) # Show the widget on the screen
    canvas.draw() # Draw the graph on the canvas?

    entrystep = Entry(CTF,font=  ("Arial",12))
    entrystep.place(x=370,y=350,width=90)    
    entrystep.insert(END,format(stepTF,"3.2e"))

    entryNsamp = Entry(CTF,font=  ("Arial",12))
    entryNsamp.place(x=370,y=390,width=90)    
    entryNsamp.insert(END,int(Nsamp))

    if (stepTF!=step):
        label9 = Label(frameFF, text='Warning: Transfer function dT does not match input files dT \n do a resampling of the transfer function or resample all the input files', font=  ("Arial",11),fg='red')
        label9.place (x=180, y=130)
        dt_entry.delete(0,END)
        dt_entry.insert(END,format(step,"3.2e"))

    if (stepTF==step):
        label9 = Label(frameFF, text='                          TRANSFER FUNCTION TIME STEP OK!!                      \n                                                                                          ', font=  ("Arial",11),fg='green')
        label9.place (x=180, y=130)
        dt_entry.delete(0,END)
        dt_entry.insert(0,format(step,"3.2e"))


    plt.close()

def check_current():
    global CC
    CC = Toplevel(root)
    CC.geometry("800x750")
    CC.resizable(0, 0)
    CC.tk.call('wm', 'iconphoto', CC._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

    CC.title("Check input files") 
    TF_label = Label(CC, text='CHECK INPUT FILES TO ANALYZE',font = ("Modern",9))
    TF_label.place(x=20,y=10)
    my_canvas = Canvas(CC,width=750,height=1,bg='black')
    my_canvas.place(x=20,y=27)

    nextPlotBtn = Button (CC, text=" Next \nInput File  ",command=plotcheck,font = ("Arial",10))
    nextPlotBtn.place(x=705,y=295)

    global Lines2Skip_entry
    Lines2Skip_entry = Entry (CC,font= ("Arial",12))
    Lines2Skip_entry.place(x=370,y=470,width=90) 
    Lines2Skip_entry.insert(END,'0')
    
    plotcheck()

    label0 = Label(CC, text='dT =', font=  ("Arial",12))
    label0.place (x=322, y=350)
    label01 = Label(CC, text='s', font=  ("Arial",12))
    label01.place (x=470, y=350)

    labelN = Label(CC, text='Nsamples =', font=  ("Arial",12))
    labelN.place (x=270, y=390)

    labelN = Label(CC, text='File extension =', font=  ("Arial",12))
    labelN.place (x=243, y=430)

    labelNlines = Label(CC, text='N° of lines to skip =', font=  ("Arial",12))
    labelNlines.place (x=218, y=470)
  
    labelWarning0 = Label(CC, text='*****************************************   WARNING   ***************************************** ', font=  ("Arial",12), fg='red')
    labelWarning0.place(x=100,y=530)
    labelWarning = Label(CC, text='Please make sure  to set the correct timestep in the "dT" entry in the transfer function', font=  ("Arial",12), fg='red')
    labelWarning.place(x=100,y=560)
    labelWarning1 = Label(CC, text='section, and the number of samples in "N° Samples" for the duration of the signal.', font=  ("Arial",12), fg='red')
    labelWarning1.place(x=100,y=590)
    labelWarning1b = Label(CC, text='If the Transfer Function "From File" is chosen make sure the timestep is the same.', font=  ("Arial",12), fg='red')
    labelWarning1b.place(x=100,y=620)
    labelWarning2b = Label(CC, text='If the waveforms are negative please use the scale factor to invert them.', font=  ("Arial",12), fg='red')
    labelWarning2b.place(x=100,y=650)
    labelWarning2 = Label(CC, text='*************************************************************************************************** ', font=  ("Arial",12), fg='red')
    labelWarning2.place(x=100,y=680)

    CC.mainloop()

def check_TransferFunction():
    global CTF
    CTF = Toplevel(root)
    CTF.geometry("800x450")
    CTF.resizable(0, 0)
    CTF.tk.call('wm', 'iconphoto', CTF._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

    CTF.title("Check Transfer Function") 
    TF_label = Label(CTF, text='CHECK TRANSFER FUNCTION TO USE',font = ("Modern",9))
    TF_label.place(x=20,y=10)
    my_canvas = Canvas(CTF,width=750,height=1,bg='black')
    my_canvas.place(x=20,y=27)

    plotcheckTF()

    label0 = Label(CTF, text='dT =', font=  ("Arial",12))
    label0.place (x=322, y=350)
    label01 = Label(CTF, text='s', font=  ("Arial",12))
    label01.place (x=470, y=350)

    labelN = Label(CTF, text='Nsamples =', font=  ("Arial",12))
    labelN.place (x=270, y=390)

    CTF.mainloop()

def callback(url):
    webbrowser.open_new(url)

def resultON():
    subprocess.Popen('python3 Results.py ' + folder_selected1 + ' ' + folder_selected2 + ' ' + line2skip, shell=True)

def deconv():
    subprocess.Popen('python3 Deconvolution.py', shell=True)

def resamp():
    subprocess.Popen('python3 Resampling.py', shell=True)


def writeCFG( ):

    text_file = open("../etc/configuration.cfg", 'w+')
    text_file.write('\nInputDirectory     =' + '"' + folder_selected1 + '";\n')
    text_file.write('\nOutputDirectory     =' + '"' + folder_selected2 + '";\n')

    text_file.write('\nInputFileExtension = "' + my_ext  +'";\n') 
    text_file.write('NlinesToSkip       = ' + line2skip + ';\n')
    text_file.write('token              = " ";\n')
    text_file.write('column             = 1;\n')
    
    if len(maxfile_entry.get()) == 0:
        text_file.write('\nMaxInputFiles      ="10000";\n') 
    elif len(maxfile_entry.get()) != 0:
        text_file.write('\nMaxInputFiles      =' + maxfile_entry.get() + ';\n') 

    if len(offset_entry.get()) == 0:
        text_file.write('\noffset             = 0; \n') 
    elif len(maxfile_entry.get()) != 0:
        text_file.write('\noffset             =' + offset_entry.get() + ';\n') 

    if var3.get() == 0:
        text_file.write('\nUseSameCurve       = false;\n') 
        text_file.write('FileName           = "";\n') 
    elif var3.get() !=0:
        text_file.write('\nUseSameCurve       = true;\n') 
        text_file.write('FileName           = "' + os.path.split(filechosen.name)[1] + '";\n') 

    text_file.write('\nIdxConvtoSave      = 0;\n')   

    if var4.get() == 0:
        text_file.write('\nMakeTimeDigitization   = false;\n')  
    elif var4.get() !=0:
        text_file.write('\nMakeTimeDigitization   = true;\n')  

    if var4b.get() == 0:
        text_file.write('\nrandomphase            = false;\n')  
    elif var4b.get() !=0:
        text_file.write('randomphase            = true;\n')  

    if var5.get() == 0:
        text_file.write('\nMakeVoltageDigitization   = false;\n')  
    elif var5.get() !=0:
        text_file.write('\nMakeVoltageDigitization   = true;\n')  

    text_file.write('\nADCmin                  = 0.0;\n')  
    text_file.write('ADCmax                  = '+ adc_max.get() +';\n')
    text_file.write('ADCnbits                  = '+ adc_bit.get() +';\n')   

    text_file.write('\nLE_reject_nonoise  ='+ LE_noff.get() +';\n')
    text_file.write('LE_reject_noise  ='+ LE_non.get() +';\n')   
    
    text_file.write('\nSaveSinglePlotConvolution    = true;\n') 

    if var6.get() == 0:
        text_file.write('\nSaveConvDataToFile           = false;\n')  
    elif var6.get() !=0:
        text_file.write('SaveConvDataToFile           = true;\n')   

    if var7.get() == 0:
        text_file.write('MakeLinearFitNearThreshold   = false;\n')  
    elif var7.get() !=0:
        text_file.write('MakeLinearFitNearThreshold   = true;\n') 

    if var8.get() == 0:
        text_file.write('MakeGaussianFitNearVmax      = false;\n')  
    elif var8.get() !=0:
        text_file.write('MakeGaussianFitNearVmax      = true;\n') 

    text_file.write('MakeTheoreticalTOA           = false;\n') 
    text_file.write('\n')

    if var9.get() == 0:
        text_file.write('\nDoMeasurementsWithNoise      = false;\n')  
        text_file.write('AddSimulatedNoise            = false;\n')
    elif var9.get() !=0:
        text_file.write('DoMeasurementsWithNoise      = true;\n')
        text_file.write('AddSimulatedNoise            = true;\n')

    if var10.get() == 0:
        text_file.write('UseRedNoise                  = false;\n')  
    elif var10.get() !=0:
        text_file.write('UseRedNoise                  = true;\n')

    if var11.get() == 0:
        text_file.write('AddNoiseFromFiles            = false;\n') 
        text_file.write('NoiseDirectory               = "";\n') 
    elif var11.get() !=0:
        text_file.write('AddNoiseFromFiles            = true;\n')
        text_file.write('\nNoiseDirectory     =' + '"' + folder_selected3 + '";\n')

    a_float = float(scale_entry.get())
    formatted_float = "{:.2f}".format(a_float)
    text_file.write('\nscale_factor               =' + str(formatted_float) + ';\n')
    text_file.write('\nLandauFluctuation          = false;\n')
    text_file.write('landaufactor_mean            = 1.0;\n')
    text_file.write('landaufactor_sigma           = 0.0;\n')
    text_file.write('\n')

    if var12.get() == 0:
        text_file.write('\nTimeReferenceResolution      = false;\n')  
    elif var12.get() !=0:
        text_file.write('TimeReferenceResolution      = true;\n') 
    
    text_file.write('timeref_sigma                = '+ TR_entry.get() +';\n') 
    text_file.write('\n')

    if var13.get() == 0:
        text_file.write('\nLowPassFilter                = false;\n')  
    elif var13.get() !=0:
        text_file.write('LowPassFilter                = true;\n')

    if var14.get() == 0:
        text_file.write('\nFilterOnlyNoise              = false;\n')  
    elif var14.get() !=0:
        text_file.write('FilterOnlyNoise              = true;\n')
    
    
    text_file.write('LowPassFrequency             ='+ filterF_entry.get() +';\n')
    text_file.write('LowPassOrder                 ='+ filterN_entry.get() +';\n')
    text_file.write('\n')
    text_file.write('TOTcorrection                = false;\n')
    text_file.write('TOT_a                        = 0.0;\n')
    text_file.write('TOT_b                        = 0.0;\n')
    text_file.write('\n')
    text_file.write('PlotRMfit                    = false;\n')
    text_file.write('PlotLinFit                   = false;\n')
    text_file.write('PlotGausFit                  = false;\n')
    text_file.write('DelayMonitoring              = 0;\n')
    text_file.write('\n')

    if stat1.get() == 1:
        text_file.write('\nMakeConvolution        = true;\n')
        text_file.write('TransferFunction = "TIA_BJT_1stage";\n')
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')
        text_file.write('\nTIA_BJT_1stage :{\n')
        text_file.write('ID        = 3;\n')
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n')
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 4e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n') 
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n')
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('Rp              = ' + str(Rp.get()) +';\n')
        text_file.write('Rf              = ' + str(Rf.get()) +';\n')
        text_file.write('Rl              = ' + str(Rc.get()) +';\n') 
        text_file.write('gm             = ' + str(gm.get()) +';\n')  
        text_file.write('ci              = ' + str(CIN.get()) +';\n')
        text_file.write('co              = ' + str(CL.get()) +';\n')
        text_file.write('cf              = ' + str(CF.get()) +';\n')
        text_file.write('};\n') 
        text_file.write('\n')

        stat1cfg = open("TFB_guiFiles/stat1.cfg", "r")
        readstat1 = stat1cfg.read()

        text_file.write(readstat1)        

    if stat2.get() == 1:
        text_file.write('\nMakeConvolution        = true;\n')
        text_file.write('\nTransferFunction = "TIA_BJT_2stages";\n') 
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')

        text_file.write('\nTIA_BJT_2stages :{\n')
        text_file.write('ID        = 1;\n')
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n') 
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 4e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n') 
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n')
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('Rp              = ' + str(Rp.get()) +';\n')
        text_file.write('Rf              = ' + str(Rf.get()) +';\n')
        text_file.write('Rl              = ' + str(Rc.get()) +';\n') 
        text_file.write('gm1              = ' + str(gm.get()) +';\n') 
        text_file.write('ci              = ' + str(CIN.get()) +';\n')
        text_file.write('cf              = ' + str(CF.get()) +';\n')
        text_file.write('co              = ' + str(CL.get()) +';\n')
        text_file.write('Gain            = ' + str(Gain.get()) +';\n')
        text_file.write('tau2             = ' + str(tau2.get()) +';\n') 

        
        text_file.write('};\n') 
        text_file.write('\n')

        stat2cfg = open("TFB_guiFiles/stat2.cfg", "r")
        readstat2 = stat2cfg.read()

        text_file.write(readstat2)       

    if stat3.get() == 1:
        text_file.write('\nMakeConvolution        = true;\n')
        text_file.write('\nTransferFunction = "TIA_MOS";\n')
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')

        text_file.write('\nTIA_MOS :{\n')
        text_file.write('ID        = 0;\n')
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n') 
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 50e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n')
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n') 
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('Rf              = ' + str(Rf.get()) +';\n')
        text_file.write('Rl              = ' + str(Rc.get()) +';\n') 
        text_file.write('gm              = ' + str(gm.get()) +';\n') 
        text_file.write('ci              = ' + str(CIN.get()) +';\n')
        text_file.write('co              = ' + str(CL.get()) +';\n')
        text_file.write('cf              = ' + str(CF.get()) +';\n')
        text_file.write('};\n') 
        text_file.write('\n')

        stat3cfg = open("TFB_guiFiles/stat3.cfg", "r")
        readstat3 = stat3cfg.read()
        text_file.write(readstat3) 

    if stat4.get() == 1:

        text_file.write('\nMakeConvolution        = true;\n')
        text_file.write('\nTransferFunction = "FromFile";\n')
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')

        text_file.write('\nFromFile :{\n')
        text_file.write('ID        = 5;\n')
        text_file.write('TFFile      = "' + os.path.split(FromFilePath.name)[0] + '/' +os.path.split(FromFilePath.name)[1] + '";\n')
        text_file.write('NlinesToSkip= 0;\n') 
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n') 
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 10e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n') 
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n')
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('};\n') 
        text_file.write('\n')

        stat4cfg = open("TFB_guiFiles/stat4.cfg", "r")
        readstat4 = stat4cfg.read()
        text_file.write(readstat4) 

    if stat5.get() == 1:
        text_file.write('\nMakeConvolution        = false;\n')
        text_file.write('TransferFunction = "TIA_BJT_1stage";\n')
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')
        text_file.write('\nTIA_BJT_1stage :{\n')
        text_file.write('ID        = 3;\n')
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n')
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 4e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n') 
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n')
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('Rp              = ' + str(Rp.get()) +';\n')
        text_file.write('Rf              = ' + str(Rf.get()) +';\n')
        text_file.write('Rl              = ' + str(Rc.get()) +';\n') 
        text_file.write('gm             = ' + str(gm.get()) +';\n')  
        text_file.write('ci              = ' + str(CIN.get()) +';\n')
        text_file.write('co              = ' + str(CL.get()) +';\n')
        text_file.write('cf              = ' + str(CF.get()) +';\n')
        text_file.write('};\n') 
        text_file.write('\n')

        stat1cfg = open("TFB_guiFiles/stat1.cfg", "r")
        readstat1 = stat1cfg.read()

        text_file.write(readstat1)   

        #text_file.write('FileName           = "' + os.path.split(FromFilePath.name)[0] + '/' +os.path.split(FromFilePath.name)[1] +  '";\n')
    
    if stat6.get() == 1:

        text_file.write('\nMakeConvolution        = true;\n')
        text_file.write('\nTransferFunction = "FromFile";\n')
        text_file.write('TFFile = "dummy";\n')
        text_file.write('\n')

        text_file.write('\nFromFile :{\n')
        text_file.write('ID        = 5;\n')
        text_file.write('TFFile      = "../examples/conv_input_files/3Ddiamond.txt";\n')
        text_file.write('NlinesToSkip= 0;\n') 
        text_file.write('Nsamples        = ' + str(int(Nsamples.get())) +';\n') 
        text_file.write('Nbins       = 50;\n')
        text_file.write('minplot     = 0.0;\n')
        text_file.write('maxplot     = 10e-9;\n')
        text_file.write('dT              = ' + str(dT.get()) +';\n')
        text_file.write('LEthr           = ' + str(LE_thr.get()) +';\n')   
        text_file.write('CFD_fr          = ' + str(CFD_thr.get()) +';\n') 
        text_file.write('sigma_noise     = ' + str(noise_entry.get()) +';\n')
        text_file.write('r_rednoise      = ' + str(noiseR_entry.get()) +';\n')
        text_file.write('sampling_dT     = ' + str(dt_dgt.get()) +';\n')
        text_file.write('bound_fit = ' + str(int(Bound_fit.get())) +';\n')
        text_file.write('RM_delay        = ' + str(RM_delay.get()) +';\n') 
        text_file.write('};\n') 
        text_file.write('\n')

        stat4cfg = open("TFB_guiFiles/stat4.cfg", "r")
        readstat4 = stat4cfg.read()
        text_file.write(readstat4) 

    text_file.close()
    
    dir2 = directory + '/data'
    if os.path.exists(dir2):
        shutil.rmtree(directory + '/data')
        os.makedirs(dir2)

    subprocess.run(["./../build/analysis_tbb", "ls -l"])


my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_label7 = Label(root, text='TFBOOST CONFIGURATION GUI',font = ("Modern",9))
my_label7.place(x=20,y=85)

my_button = Button(root, text="Choose directory for the input files:  ",command=openFileInput,font = ("Arial",9))
my_button.place(x=20,y=110)

my_button2 = Button(root, text="Choose directory for the output files:",command=openFileOutput,font = ("Arial",9))
my_button2.place(x=20,y=150)

#---------------input file to process entry-----------------------------
my_label4 = Label(root, text='Maximum input files to process:',font = ("Arial",9))
my_label4.place(x=20,y=200)

maxfile_entry = Entry(root,font = ("Arial",10))
maxfile_entry.place(x=200,y=198,width=60)
maxfile_entry.insert(END,"25000")

my_label5 = Label(root, text='Time offset in unit of dT:',font = ("Arial",9))
my_label5.place(x=280,y=200)

offset_entry = Entry(root,font = ("Arial",10))
offset_entry.place(x=420,y=198,width=40)
offset_entry.insert(END,'0')

my_label50 = Label(root, text='Scale factor for the currents:',font = ("Arial",9))
my_label50.place(x=490,y=200)

scale_entry = Entry(root,font = ("Arial",10))
scale_entry.place(x=650,y=198,width=40)
scale_entry.insert(END,'1')

my_label6 = Label(root, text='Use always the same curve:',font = ("Arial",9))
my_label6.place(x=20,y=235)

var3 = IntVar()
Checkbutton(root, variable=var3,command = usesamecurve,font = ("Arial",8)).place(x=175, y=235)

my_canvas0 = Canvas(root,width=750,height=1,bg='black')
my_canvas0.place(x=20,y=100)

#---------------DIGITIZATION-----------------------------
my_canvas = Canvas(root,width=750,height=1,bg='black')
my_canvas.place(x=20,y=265)
my_label7 = Label(root, text='DIGITIZATION',font = ("Arial Bold",9))
my_label7.place(x=20,y=267)

my_label6 = Label(root, text='Time Digitization:',font = ("Arial",9))
my_label6.place(x=20,y=287)
var4 = IntVar()
var4.set(1)
Checkbutton(root, variable=var4,font = ("Arial",8)).place(x=115, y=286)

my_label6 = Label(root, text='Random phase:',font = ("Arial",9))
my_label6.place(x=140,y=287)
var4b = IntVar()
Checkbutton(root, variable=var4b,font = ("Arial",8)).place(x=230, y=286)

my_label6 = Label(root, text='dT :',font = ("Arial",9))
my_label6.place(x=275,y=287)
dt_dgt = Entry(root,font = ("Arial",10))
dt_dgt.place(x=305,y=286,width=50)
dt_dgt.insert(END,'20e-12')

my_label6 = Label(root, text='   Voltage Digitization:',font = ("Arial",9))
my_label6.place(x=360,y=287)
var5 = IntVar()
var5.set(1)
Checkbutton(root, variable=var5,font = ("Arial",8)).place(x=480, y=286)
my_label6 = Label(root, text='ADC step (V):',font = ("Arial",9))
my_label6.place(x=510,y=287)
adc_max = Entry(root,font = ("Arial",10))
adc_max.place(x=590,y=286,width=40)
adc_max.insert(END,'0.01')

my_label6 = Label(root, text='ADC n° Bit:',font = ("Arial",9))
my_label6.place(x=650,y=287)
adc_bit = Entry(root,font = ("Arial",10))
adc_bit.place(x=720,y=286,width=20)
adc_bit.insert(END,'8')

#---------------THRESHOLDS-----------------------------
my_canvas1 = Canvas(root,width=750,height=1,bg='black')
my_canvas1.place(x=20,y=315)

my_label7 = Label(root, text='INITIAL THRESHOLDS',font = ("Arial Bold",9))
my_label7.place(x=20,y=317)

my_label6 = Label(root, text='   Leading Edge noise-off (V):',font = ("Arial",9))
my_label6.place(x=200,y=337)
LE_noff = Entry(root,font = ("Arial",10))
LE_noff.place(x=370,y=337,width=40)
LE_noff.insert(END,'0.0')

my_label6 = Label(root, text='   Leading Edge noise-on (V):',font = ("Arial",9))
my_label6.place(x=460,y=337)
LE_non = Entry(root,font = ("Arial",10))
LE_non.place(x=630,y=337,width=40)
LE_non.insert(END,'0.0')

#---------------MEASUREMENTS AND SAVING OPTION-----------------------------
my_canvas1 = Canvas(root,width=750,height=1,bg='black')
my_canvas1.place(x=20,y=365)

my_label8 = Label(root, text='MEASUREMENTS AND SAVING OPTIONS',font = ("Arial Bold",9))
my_label8.place(x=20,y=367)

my_label6 = Label(root, text='Save output files:',font = ("Arial",9))
my_label6.place(x=20,y=390)
var6 = IntVar()
var6.set(1)
Checkbutton(root, variable=var6,font = ("Arial",8)).place(x=120, y=389)

my_label6 = Label(root, text='   Linear Fit near Threshold:',font = ("Arial",9))
my_label6.place(x=220,y=390)
var7 = IntVar()
var7.set(1)
Checkbutton(root, variable=var7,font = ("Arial",8)).place(x=375, y=389)

my_label6 = Label(root, text='   Gaussian Fit near Vmax:',font = ("Arial",9))
my_label6.place(x=460,y=390)
var8 = IntVar()
var8.set(1)
Checkbutton(root, variable=var8,font = ("Arial",8)).place(x=610, y=389)


#---------------NOISE-----------------------------
my_canvas2 = Canvas(root,width=750,height=1,bg='black')
my_canvas2.place(x=20,y=415)
my_label9 = Label(root, text='NOISE',font = ("Arial Bold",9))
my_label9.place(x=20,y=417)

my_label6 = Label(root, text='Add NOISE:',font = ("Arial",9))
my_label6.place(x=20,y=440)
var9 = IntVar()
Checkbutton(root, variable=var9,font = ("Arial",8)).place(x=90, y=439)

my_label6 = Label(root, text='Use RED NOISE:',font = ("Arial",9))
my_label6.place(x=140,y=440)
var10 = IntVar()
Checkbutton(root, variable=var10,font = ("Arial",8)).place(x=235, y=439)

my_label6 = Label(root, text='sigma (V):',font = ("Arial",9))
my_label6.place(x=310,y=440)
noise_entry = Entry(root,font = ("Arial",10))
noise_entry.place(x=375,y=440,width=50)
noise_entry.insert(END,'0.0028')

my_label6 = Label(root, text='   correlation factor r :',font = ("Arial",9))
my_label6.place(x=470,y=440)
noiseR_entry = Entry(root,font = ("Arial",10))
noiseR_entry.place(x=590,y=440,width=50)
noiseR_entry.insert(END,'0.985')

my_label6 = Label(root, text='Add noise from files:',font = ("Arial",9))
my_label6.place(x=20,y=471)
var11 = IntVar()
Checkbutton(root, variable=var11,command = noisefromfiles,font = ("Arial",8)).place(x=135, y=470)

#---------------OTHER OPTIONS-----------------------------
my_canvas2 = Canvas(root,width=750,height=1,bg='black')
my_canvas2.place(x=20,y=505)

my_label9 = Label(root, text='OTHER OPTIONS',font = ("Arial Bold",9))
my_label9.place(x=20,y=507)


my_label6 = Label(root, text='Gaussian Time Reference:',font = ("Arial",9))
my_label6.place(x=20,y=527)
var12 = IntVar()
Checkbutton(root, variable=var12,font = ("Arial",8)).place(x=170, y=526)
my_label6 = Label(root, text='sigma :',font = ("Arial",9))
my_label6.place(x=20,y=547)
TR_entry = Entry(root,font = ("Arial",10))
TR_entry.place(x=65,y=547,width=50)
TR_entry.insert(END,'15e-12')

my_label6 = Label(root, text='|  Low Pass Filter:',font = ("Arial",9))
my_label6.place(x=355,y=527)
var13 = IntVar()
Checkbutton(root, variable=var13,font = ("Arial",8)).place(x=470, y=526)

my_label6 = Label(root, text='|  Filter only noise:',font = ("Arial",9))
my_label6.place(x=355,y=547)
var14 = IntVar()
Checkbutton(root, variable=var14,font = ("Arial",8)).place(x=470, y=546)

my_label6 = Label(root, text='|  Filter Order n°:',font = ("Arial",9))
my_label6.place(x=355,y=567)
filterN_entry = Entry(root,font = ("Arial",10))
filterN_entry.place(x=475,y=566,width=20)
filterN_entry.insert(END,'2')

my_label6 = Label(root, text='|  Filter cut-off Frequency:',font = ("Arial",9))
my_label6.place(x=355,y=587)
filterF_entry = Entry(root,font = ("Arial",10))
filterF_entry.place(x=495,y=586,width=40)
filterF_entry.insert(END,'1e9')

#---------------MAKE CONVOLUTION-----------------------------
my_canvas2 = Canvas(root,width=750,height=1,bg='black')
my_canvas2.place(x=20,y=615)

my_label9 = Label(root, text='CONVOLUTION AND WAVEFORM ANALYSIS',font = ("Arial Bold",9))
my_label9.place(x=20,y=617)

""" my_label11 = Label(root, text='USE EXISTING CONFIGURATION FILE:',font = ("Arial",10))
my_label11.place(x=85,y=650)
var15 = IntVar()
Checkbutton(root, variable=var15,font = ("Arial",9)).place(x=335, y=649) """

make_btn = Button(root, text='MAKE NEW ANALYSIS',command=openTFgui,font = ("Arial",13))
make_btn.place(x=400,y=645)

btn_write = Button(root, text=" RUN ",command=writeCFG,font = ("Arial",16))
btn_write.place(x=400,y=690)

btn_result = Button(root, text="RESULTS",command=resultON,font = ("Arial",16))
btn_result.place(x=490,y=690)



root.mainloop()