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

root = tk.Tk();
root.title('TFBoost GUI')
root.geometry("800x750")
root.resizable(0, 0)
root.tk.call('wm', 'iconphoto', root._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))


global stat1,stat2,stat3,stat4,stat5
stat1 =IntVar()
stat2 =IntVar()
stat3 =IntVar()
stat4 =IntVar()
stat5 =IntVar()

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


global Nsamples, dT, LE_thr, CFD_thr, RM_delay, Bound_fit
Nsamples = DoubleVar()
dT = DoubleVar()
LE_thr = DoubleVar()
CFD_thr = DoubleVar()
RM_delay = DoubleVar()
Bound_fit = DoubleVar()


def openFileInput():
    global folder_selected1
    folder_selected1 = filedialog.askdirectory()
    directory0 = os.path.split(folder_selected1)[0] + '/' + os.path.split(folder_selected1)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=300,y=112)
    text1.configure(state="normal")
    text1.insert('end', directory0)
    text1.configure(state="disabled")
    

def openFileOutput():
    global folder_selected2
    folder_selected2 = filedialog.askdirectory()
    directory = os.path.split(folder_selected2)[0] + '/' + os.path.split(folder_selected2)[1]
    text1 = Text(root, state='disabled', width=50, height=1)
    text1.place (x=300,y=160)
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
    text1 = Text(TF, state='disabled', width=50, height=1)
    text1.place (x=220,y=203,width=400)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(FromFilePath.name)[0] + '/' +os.path.split(FromFilePath.name)[1])
    text1.configure(state="disabled")

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

    if(logo3.winfo_exists() or logo6.winfo_exists()):
        logo3.place_forget()
        logo6.place_forget()
        G1.place_forget()
        G2.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        tauo1.place_forget()
        tauo2.place_forget()
        tauo3.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        grm3.place_forget()
        gG1.place_forget()
        gG2.place_forget()
        G_entry.place_forget()
        tauo_entry.place_forget()
        gmr_entry.place_forget() 
        btnRM.place_forget()
        btnRM2.place_forget()

    TF.update()

    stat1.set(1)
    stat2.set(0)
    stat3.set(0)
    stat4.set(0)
    stat5.set(0)
        
    logo5 =Label(TF,image=my_logo5)
    logo4 =Label(TF,image=my_logo4)
    logo5.place(x=400,y=230)
    logo4.place(x=500,y=230)

    logo2 =Label(TF,image=my_logo2)
    logo2.place(x=20,y=170)
    btnRM1.place(x=500,y=620)
    
    TF_label2 = Label(TF, text='Single Stage TIA with Bipolar transistor',font = ("Modern",9))
    TF_label2.place(x=380,y=170,width=400)
    TF_label3 = Label(TF, text='only critical damping case is considered',font = ("Modern",9))
    TF_label3.place(x=380,y=190,width=400)

    link1 = Label(TF, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=305,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))
    
    # r_pi show
    global rpi1,rpi2,rpi3
    rpi1 =Label(TF,text='r   =',font = ("Modern",10))
    rpi2 =Label(TF,text=' \u03C0',font = ("Modern",6))
    global rpi_entry 
    rpi1.place_forget()
    rpi2.place_forget()
    rpi3.place_forget()
    rpi_entry.place_forget()
    rpi_entry = Entry(TF,font = ("Modern",10))
    rpi1.place(x=135,y=380)
    rpi2.place(x=140,y=390)
    rpi_entry.place(x=170,y=380,width=60)
    rpi_entry.insert(END,'150')
    rpi3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rpi3.place(x=230,y=380)

    # R_F show
    rF1 =Label(TF,text='R    =',font = ("Modern",10))
    rF2 =Label(TF,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=410)
    rF2.place(x=140,y=420)
    global rF_entry
    rF_entry = Entry(TF,font = ("Modern",10))
    rF_entry.place(x=170,y=410,width=60)
    rF_entry.insert(END,'4300')
    rF3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=410)

    # R_C show
    rC1 =Label(TF,text='R    =',font = ("Modern",10))
    rC2 =Label(TF,text=' C',font = ("Modern",6))
    rC1.place(x=127,y=440)
    rC2.place(x=140,y=450)
    global rC_entry
    rC_entry = Entry(TF,font = ("Modern",10))
    rC_entry.place(x=170,y=440,width=60)
    rC_entry.insert(END,'50')
    rC3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=440)

    # C_in show
    Cin1 =Label(TF,text='C    =',font = ("Modern",10))
    Cin2 =Label(TF,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=470)
    Cin2.place(x=140,y=480)
    global Cin_entry
    Cin_entry = Entry(TF,font = ("Modern",10))
    Cin_entry.place(x=170,y=470,width=60)
    Cin_entry.insert(END,'2e-12')
    Cin3 =Label(TF,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=470)

    # C_f show
    Cf1 =Label(TF,text='C    =',font = ("Modern",10))
    Cf2 =Label(TF,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=500)
    Cf2.place(x=140,y=510)
    global Cf_entry
    Cf_entry = Entry(TF,font = ("Modern",10))
    Cf_entry.place(x=170,y=500,width=60)
    Cf_entry.insert(END,'50e-15')
    Cf3 =Label(TF,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=500)

    # C_L show
    CL1 =Label(TF,text='C    =',font = ("Modern",10))
    CL2 =Label(TF,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=530)
    CL2.place(x=140,y=540)
    global CL_entry
    CL_entry = Entry(TF,font = ("Modern",10))
    CL_entry.place(x=170,y=530,width=60)
    CL_entry.insert(END,'4e-12')
    CL3 =Label(TF,text='F',font = ("Modern",10))
    CL3.place(x=230,y=530)

    # gm show
    gm1 =Label(TF,text='g    =',font = ("Modern",10))
    gm2 =Label(TF,text='m',font = ("Modern",6))
    gm1.place(x=127,y=560)
    gm2.place(x=140,y=570)
    global gm_entry
    gm_entry = Entry(TF,font = ("Modern",10))
    gm_entry.place(x=170,y=560,width=60)
    gm_entry.insert(END,'0.5')
    gm3 =Label(TF,text='S',font = ("Modern",10))
    gm3.place(x=230,y=560)

    # R_m calculation
    rm1 =Label(TF,text='R     =',font = ("Modern",10))
    rm2 =Label(TF,text='m0',font = ("Modern",6))
    rm1.place(x=123,y=650)
    rm2.place(x=137,y=660)
    global rm_entry
    rm_entry = Entry(TF,font = ("Modern",10))
    rm_entry.place(x=170,y=650,width=75)
    rm3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rm3.place(x=250,y=650)

    # tau calculation
    tau1 =Label(TF,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=123,y=680)
    global tau_entry
    tau_entry = Entry(TF,font = ("Modern",10))
    tau_entry.place(x=170,y=680,width=75)
    tau3 =Label(TF,text='s',font = ("Modern",10))
    tau3.place(x=250,y=680)
    
    # NSAMPLES show
    nsamples =Label(TF,text='N° Samples',font = ("Modern",10))
    nsamples.place(x=450,y=380)
    global nsamples_entry 
    nsamples_entry = Entry(TF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=380,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(TF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=410)
    global dt_entry
    dt_entry = Entry(TF,font = ("Modern",10))
    dt_entry.place(x=550,y=410,width=70)
    dt_entry.insert(END,'1e-12')
    dt3 =Label(TF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=410)

    # Bound show
    bound1 =Label(TF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=440)
    global bound_entry
    bound_entry = Entry(TF,font = ("Modern",10))
    bound_entry.place(x=550,y=440,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(TF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=500)
    global LE_entry
    LE_entry = Entry(TF,font = ("Modern",10))
    LE_entry.place(x=550,y=500,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(TF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=500)

    # CFD show
    CFD1 =Label(TF,text='CFD threshold fraction    =',font = ("Modern",10))
    CFD1.place(x=362,y=530)
    global CFD_entry
    CFD_entry = Entry(TF,font = ("Modern",10))
    CFD_entry.place(x=550,y=530,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(TF,text='ARC method delay        =',font = ("Modern",10))
    ARC1.place(x=365,y=560)
    global ARC_entry
    ARC_entry = Entry(TF,font = ("Modern",10))
    ARC_entry.place(x=550,y=560,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(TF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=560)

def csastg():

    if(rpi1.winfo_exists() ):
        logo3.place_forget()
        logo2.place_forget()
        G1.place_forget()
        G2.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        tauo1.place_forget()
        tauo2.place_forget()
        tauo3.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        grm3.place_forget()
        gG1.place_forget()
        gG2.place_forget()
        G_entry.place_forget()
        tauo_entry.place_forget()
        gmr_entry.place_forget() 
        btnRM.place_forget()
        btnRM1.place_forget()
        rpi1.place_forget()
        rpi2.place_forget()
        rpi3.place_forget()
        rpi_entry.place_forget()
    
    TF.update()

    stat1.set(0)
    stat2.set(0)
    stat3.set(1)
    stat4.set(0)
    stat5.set(0)
        
    logo5 =Label(TF,image=my_logo5)
    logo4 =Label(TF,image=my_logo4)
    logo5.place(x=400,y=230)
    logo4.place(x=500,y=230)

    logo6 =Label(TF,image=my_logo6)
    logo6.place(x=20,y=170)

    btnRM2.place(x=500,y=620)
    
    TF_label2 = Label(TF, text='Single Stage MOS Charge Sensitive Amplifier',font = ("Modern",9))
    TF_label2.place(x=380,y=170,width=400)
    TF_label3 = Label(TF, text='all damping behavior are possible with values provided',font = ("Modern",9))
    TF_label3.place(x=380,y=190,width=400)

    link1 = Label(TF, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=305,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))
    
    logo7 =Label(TF,image=my_logo7)
    logo7.place(x=50,y=400)
    rpi1.place_forget()
    rpi2.place_forget()
    rpi3.place_forget()
    rpi_entry.place_forget()
    # R_F show
    rF1 =Label(TF,text='R    =',font = ("Modern",10))
    rF2 =Label(TF,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=410)
    rF2.place(x=140,y=420)
    global rF_entry
    rF_entry = Entry(TF,font = ("Modern",10))
    rF_entry.place(x=170,y=410,width=60)
    rF_entry.insert(END,'3e6')
    rF3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=410)

    # R_C show
    rC1 =Label(TF,text='R    =',font = ("Modern",10))
    rC2 =Label(TF,text=' D',font = ("Modern",6))
    rC1.place(x=127,y=440)
    rC2.place(x=140,y=450)
    global rC_entry
    rC_entry = Entry(TF,font = ("Modern",10))
    rC_entry.place(x=170,y=440,width=60)
    rC_entry.insert(END,'570e3')
    rC3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=440)

    # C_in show
    Cin1 =Label(TF,text='C    =',font = ("Modern",10))
    Cin2 =Label(TF,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=470)
    Cin2.place(x=140,y=480)
    global Cin_entry
    Cin_entry = Entry(TF,font = ("Modern",10))
    Cin_entry.place(x=170,y=470,width=60)
    Cin_entry.insert(END,'105e-15')
    Cin3 =Label(TF,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=470)

    # C_f show
    Cf1 =Label(TF,text='C    =',font = ("Modern",10))
    Cf2 =Label(TF,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=500)
    Cf2.place(x=140,y=510)
    global Cf_entry
    Cf_entry = Entry(TF,font = ("Modern",10))
    Cf_entry.place(x=170,y=500,width=60)
    Cf_entry.insert(END,'5e-15')
    Cf3 =Label(TF,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=500)

    # C_L show
    CL1 =Label(TF,text='C    =',font = ("Modern",10))
    CL2 =Label(TF,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=530)
    CL2.place(x=140,y=540)
    global CL_entry
    CL_entry = Entry(TF,font = ("Modern",10))
    CL_entry.place(x=170,y=530,width=60)
    CL_entry.insert(END,'20e-15')
    CL3 =Label(TF,text='F',font = ("Modern",10))
    CL3.place(x=230,y=530)

    # gm show
    gm1 =Label(TF,text='g    =',font = ("Modern",10))
    gm2 =Label(TF,text='m',font = ("Modern",6))
    gm1.place(x=127,y=560)
    gm2.place(x=140,y=570)
    global gm_entry
    gm_entry = Entry(TF,font = ("Modern",10))
    gm_entry.place(x=170,y=560,width=60)
    gm_entry.insert(END,'55e-6')
    gm3 =Label(TF,text='S',font = ("Modern",10))
    gm3.place(x=230,y=560)

    # R_m calculation
    rm1 =Label(TF,text='R     =',font = ("Modern",10))
    rm2 =Label(TF,text='m0',font = ("Modern",6))
    rm1.place(x=123,y=650)
    rm2.place(x=137,y=660)
    global rm_entry
    rm_entry = Entry(TF,font = ("Modern",10))
    rm_entry.place(x=170,y=650,width=75)
    rm3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rm3.place(x=250,y=650)

    # tau calculation
    tau1 =Label(TF,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=123,y=680)
    global tau_entry
    tau_entry = Entry(TF,font = ("Modern",10))
    tau_entry.place(x=170,y=680,width=75)
    tau3 =Label(TF,text='s',font = ("Modern",10))
    tau3.place(x=250,y=680)
    
    # NSAMPLES show
    nsamples =Label(TF,text='N° Samples',font = ("Modern",10))
    nsamples.place(x=450,y=380)
    global nsamples_entry 
    nsamples_entry = Entry(TF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=380,width=70)
    nsamples_entry.insert(END,'262144')

    # dT show
    dt1 =Label(TF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=410)
    global dt_entry
    dt_entry = Entry(TF,font = ("Modern",10))
    dt_entry.place(x=550,y=410,width=70)
    dt_entry.insert(END,'1e-12')
    dt3 =Label(TF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=410)

    # Bound show
    bound1 =Label(TF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=440)
    global bound_entry
    bound_entry = Entry(TF,font = ("Modern",10))
    bound_entry.place(x=550,y=440,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(TF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=500)
    global LE_entry
    LE_entry = Entry(TF,font = ("Modern",10))
    LE_entry.place(x=550,y=500,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(TF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=500)

    # CFD show
    CFD1 =Label(TF,text='CFD threshold fraction    =',font = ("Modern",10))
    CFD1.place(x=362,y=530)
    global CFD_entry
    CFD_entry = Entry(TF,font = ("Modern",10))
    CFD_entry.place(x=550,y=530,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(TF,text='ARC method delay        =',font = ("Modern",10))
    ARC1.place(x=365,y=560)
    global ARC_entry
    ARC_entry = Entry(TF,font = ("Modern",10))
    ARC_entry.place(x=550,y=560,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(TF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=560)

def FromFile():

    if(logo3.winfo_exists() ):
        logo3.place_forget()
        logo2.place_forget()
        G1.place_forget()
        G2.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        tauo1.place_forget()
        tauo2.place_forget()
        tauo3.place_forget()
        tauo_entry.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        grm3.place_forget()
        gG1.place_forget()
        gG2.place_forget()
        G_entry.place_forget()
        gm_entry.place_forget()
        gm1.place_forget()
        gm2.place_forget()
        gm3.place_forget()
        rm1.place_forget()
        rm2.place_forget()
        rm3.place_forget()
        rm_entry.place_forget()
        Cin1.place_forget()
        Cin2.place_forget()
        Cin3.place_forget()
        Cin_entry.place_forget()
        Cf1.place_forget()
        Cf2.place_forget()
        Cf3.place_forget()
        Cf_entry.place_forget()
        CL1.place_forget()
        CL2.place_forget()
        CL3.place_forget()
        CL_entry.place_forget()
        rpi1.place_forget()
        rpi2.place_forget()
        rpi3.place_forget()
        rpi_entry.place_forget()
        rF1.place_forget()
        rF2.place_forget()
        rF3.place_forget()
        rF_entry.place_forget()
        rC1.place_forget()
        rC2.place_forget()
        rC3.place_forget()
        rC_entry.place_forget()
        tau1.place_forget()
        tau3.place_forget()
        tau_entry.place_forget()
        rm_entry.place_forget()
        tauo_entry.place_forget()
        gmr_entry.place_forget() 
        btnRM.place_forget()
        btnRM1.place_forget()
        btnRM2.place_forget()
        TF_label2.place_forget()
        TF_label3.place_forget()
        link1.place_forget()

  

    TF.update()

    stat1.set(0)
    stat2.set(0)
    stat3.set(0)
    stat4.set(1)
    stat5.set(0)
        
    logo3.place_forget()
    logo2.place_forget()

    btnFromFile.place(x=20,y=200)

    btnRM3.place(x=50,y=500)

    
    # NSAMPLES show
    nsamples =Label(TF,text='N° Samples',font = ("Modern",10))
    nsamples.place(x=450,y=380)
    global nsamples_entry 
    nsamples_entry = Entry(TF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=380,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(TF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=410)
    global dt_entry
    dt_entry = Entry(TF,font = ("Modern",10))
    dt_entry.place(x=550,y=410,width=70)
    dt_entry.insert(END,'1e-12')
    dt3 =Label(TF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=410)

    # Bound show
    bound1 =Label(TF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=440)
    global bound_entry
    bound_entry = Entry(TF,font = ("Modern",10))
    bound_entry.place(x=550,y=440,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(TF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=500)
    global LE_entry
    LE_entry = Entry(TF,font = ("Modern",10))
    LE_entry.place(x=550,y=500,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(TF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=500)

    # CFD show
    CFD1 =Label(TF,text='CFD threshold fraction    =',font = ("Modern",10))
    CFD1.place(x=362,y=530)
    global CFD_entry
    CFD_entry = Entry(TF,font = ("Modern",10))
    CFD_entry.place(x=550,y=530,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(TF,text='ARC method delay        =',font = ("Modern",10))
    ARC1.place(x=365,y=560)
    global ARC_entry
    ARC_entry = Entry(TF,font = ("Modern",10))
    ARC_entry.place(x=550,y=560,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(TF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=560)


def WaveAnalysis():

    if(logo3.winfo_exists() ):
        logo3.place_forget()
        logo2.place_forget()
        G1.place_forget()
        G2.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        tauo1.place_forget()
        tauo2.place_forget()
        tauo3.place_forget()
        tauo_entry.place_forget()
        grm1.place_forget()
        grm2.place_forget()
        grm3.place_forget()
        gG1.place_forget()
        gG2.place_forget()
        G_entry.place_forget()
        gm_entry.place_forget()
        gm1.place_forget()
        gm2.place_forget()
        gm3.place_forget()
        rm1.place_forget()
        rm2.place_forget()
        rm3.place_forget()
        rm_entry.place_forget()
        Cin1.place_forget()
        Cin2.place_forget()
        Cin3.place_forget()
        Cin_entry.place_forget()
        Cf1.place_forget()
        Cf2.place_forget()
        Cf3.place_forget()
        Cf_entry.place_forget()
        CL1.place_forget()
        CL2.place_forget()
        CL3.place_forget()
        CL_entry.place_forget()
        rpi1.place_forget()
        rpi2.place_forget()
        rpi3.place_forget()
        rpi_entry.place_forget()
        rF1.place_forget()
        rF2.place_forget()
        rF3.place_forget()
        rF_entry.place_forget()
        rC1.place_forget()
        rC2.place_forget()
        rC3.place_forget()
        rC_entry.place_forget()
        tau1.place_forget()
        tau3.place_forget()
        tau_entry.place_forget()
        rm_entry.place_forget()
        tauo_entry.place_forget()
        gmr_entry.place_forget() 
        btnRM.place_forget()
        btnRM1.place_forget()
        btnRM2.place_forget()

        link1.place_forget()

  

    TF.update()

    stat1.set(0)
    stat2.set(0)
    stat3.set(0)
    stat4.set(0)
    stat5.set(1)
        
    #logo5 =Label(TF,image=my_logo5)
    #logo4 =Label(TF,image=my_logo4)

    #logo2 =Label(TF,image=my_logo2)

    logo3.place_forget()
    logo2.place_forget()

    btnRM3.place(x=500,y=620)
    
    TF_label2 = Label(TF, text='Single Stage TIA with Bipolar transistor',font = ("Modern",9))
    TF_label3 = Label(TF, text='only critical damping case is considered',font = ("Modern",9))

    #link1 = Label(TF, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    #link1.place(x=380,y=305,width=400)
    #link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))
    

    # NSAMPLES show
    nsamples =Label(TF,text='N° Samples',font = ("Modern",10))
    nsamples.place(x=450,y=380)
    global nsamples_entry 
    nsamples_entry = Entry(TF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=380,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(TF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=410)
    global dt_entry
    dt_entry = Entry(TF,font = ("Modern",10))
    dt_entry.place(x=550,y=410,width=70)
    dt_entry.insert(END,'1e-12')
    dt3 =Label(TF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=410)

    # Bound show
    bound1 =Label(TF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=440)
    global bound_entry
    bound_entry = Entry(TF,font = ("Modern",10))
    bound_entry.place(x=550,y=440,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(TF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=500)
    global LE_entry
    LE_entry = Entry(TF,font = ("Modern",10))
    LE_entry.place(x=550,y=500,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(TF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=500)

    # CFD show
    CFD1 =Label(TF,text='CFD threshold fraction    =',font = ("Modern",10))
    CFD1.place(x=362,y=530)
    global CFD_entry
    CFD_entry = Entry(TF,font = ("Modern",10))
    CFD_entry.place(x=550,y=530,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(TF,text='ARC method delay        =',font = ("Modern",10))
    ARC1.place(x=365,y=560)
    global ARC_entry
    ARC_entry = Entry(TF,font = ("Modern",10))
    ARC_entry.place(x=550,y=560,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(TF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=560)





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

    print(zeta)
    
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



def doublestg():

    if(logo4.winfo_exists() or logo6.winfo_exists() ):
        logo4.place_forget()
        btnRM1.place_forget()
        btnRM2.place_forget()

    stat1.set(0)
    stat2.set(1)
    stat3.set(0)
    stat4.set(0)
    stat5.set(0) 
  

    # r_pi show
    global rpi1,rpi2,rpi3, rF1,rF2,rF3
    global rC1,rC2,rC3, Cin1,Cin2,Cin3
    global Cf1,Cf2,Cf3, CL1,CL2,CL3
    global gm1,gm2,gm3, G1,G2
    global tauo1,tauo2,tauo3, rm1,rm2,rm3
    global tau1,tau2,tau3, grm1,grm2,grm3
    global gG1,gG2

    global logo2, logo3, link1, TF_label2, TF_label3

    logo2 =Label(TF,image=my_logo2)
    logo2.place(x=20,y=170)
    logo3 =Label(TF,image=my_logo3)
    logo3.place(x=450,y=230)

    TF_label2 = Label(TF, text='Double Stage TIA defined as a single stage TIA',font = ("Modern",9))
    TF_label2.place(x=380,y=170,width=400)
    TF_label3 = Label(TF, text='followed by a single pole voltage gain transfer function',font = ("Modern",9))
    TF_label3.place(x=380,y=190,width=400)

    link1 = Label(TF, text='see link: "High-resolution timing electronics for fast pixel sensors "', fg="blue", cursor="hand2",font = ("Modern",8))
    link1.place(x=380,y=305,width=400)
    link1.bind("<Button-1>", lambda e: callback("https://arxiv.org/abs/2008.09867"))


    rpi1 =Label(TF,text='r   =',font = ("Modern",10))
    rpi2 =Label(TF,text=' \u03C0',font = ("Modern",6))
    rpi1.place(x=135,y=380)
    rpi2.place(x=140,y=390)
    global rpi_entry 
    rpi_entry = Entry(TF,font = ("Modern",10))
    rpi_entry.place(x=170,y=380,width=60)
    rpi_entry.insert(END,'220')
    rpi3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rpi3.place(x=230,y=380)

    # R_F show
    rF1 =Label(TF,text='R    =',font = ("Modern",10))
    rF2 =Label(TF,text=' f',font = ("Modern",6))
    rF1.place(x=127,y=410)
    rF2.place(x=140,y=420)
    global rF_entry
    rF_entry = Entry(TF,font = ("Modern",10))
    rF_entry.place(x=170,y=410,width=60)
    rF_entry.insert(END,'4300')
    rF3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rF3.place(x=230,y=410)

    # R_C show
    rC1 =Label(TF,text='R    =',font = ("Modern",10))
    rC2 =Label(TF,text=' C',font = ("Modern",6))
    rC1.place(x=127,y=440)
    rC2.place(x=140,y=450)
    global rC_entry
    rC_entry = Entry(TF,font = ("Modern",10))
    rC_entry.place(x=170,y=440,width=60)
    rC_entry.insert(END,'50')
    rC3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rC3.place(x=230,y=440)

    # C_in show
    Cin1 =Label(TF,text='C    =',font = ("Modern",10))
    Cin2 =Label(TF,text=' in',font = ("Modern",6))
    Cin1.place(x=127,y=470)
    Cin2.place(x=140,y=480)
    global Cin_entry
    Cin_entry = Entry(TF,font = ("Modern",10))
    Cin_entry.place(x=170,y=470,width=60)
    Cin_entry.insert(END,'2e-12')
    Cin3 =Label(TF,text='F',font = ("Modern",10))
    Cin3.place(x=230,y=470)

    # C_f show
    Cf1 =Label(TF,text='C    =',font = ("Modern",10))
    Cf2 =Label(TF,text=' f',font = ("Modern",6))
    Cf1.place(x=127,y=500)
    Cf2.place(x=140,y=510)
    global Cf_entry
    Cf_entry = Entry(TF,font = ("Modern",10))
    Cf_entry.place(x=170,y=500,width=60)
    Cf_entry.insert(END,'50e-15')
    Cf3 =Label(TF,text='F',font = ("Modern",10))
    Cf3.place(x=230,y=500)

    # C_L show
    CL1 =Label(TF,text='C    =',font = ("Modern",10))
    CL2 =Label(TF,text=' L',font = ("Modern",6))
    CL1.place(x=127,y=530)
    CL2.place(x=140,y=540)
    global CL_entry
    CL_entry = Entry(TF,font = ("Modern",10))
    CL_entry.place(x=170,y=530,width=60)
    CL_entry.insert(END,'4e-12')
    CL3 =Label(TF,text='F',font = ("Modern",10))
    CL3.place(x=230,y=530)

    # gm show
    gm1 =Label(TF,text='g    =',font = ("Modern",10))
    gm2 =Label(TF,text='m',font = ("Modern",6))
    gm1.place(x=127,y=560)
    gm2.place(x=140,y=570)
    global gm_entry
    gm_entry = Entry(TF,font = ("Modern",10))
    gm_entry.place(x=170,y=560,width=60)
    gm_entry.insert(END,'0.5')
    gm3 =Label(TF,text='S',font = ("Modern",10))
    gm3.place(x=230,y=560)

    # G0 show
    global G1
    global G2
    G1 =Label(TF,text='G    =',font = ("Modern",10))
    G2 =Label(TF,text='0',font = ("Modern",6))
    G1.place(x=127,y=590)
    G2.place(x=140,y=600)
    global G_entry
    G_entry = Entry(TF,font = ("Modern",10))
    G_entry.place(x=170,y=590,width=60)
    G_entry.insert(END,'10')
    #G3 =Label(TF,text='S',font = ("Modern",10))
    #G3.place(x=230,y=590)

    # tau2 show
    global tauo_entry
    tauo1 =Label(TF,text='\u03C4    =',font = ("Modern",10))
    tauo2 =Label(TF,text='*',font = ("Modern",7))
    tauo1.place(x=130,y=620)
    tauo2.place(x=140,y=618)
    tauo_entry = Entry(TF,font = ("Modern",10))
    tauo_entry.place(x=170,y=620,width=75)
    tauo_entry.insert(END,'150e-12')
    tauo3 =Label(TF,text='s',font = ("Modern",10))
    tauo3.place(x=250,y=620)

    # R_m calculation
    rm1 =Label(TF,text='R     =',font = ("Modern",10))
    rm2 =Label(TF,text='m0',font = ("Modern",6))
    rm1.place(x=123,y=650)
    rm2.place(x=137,y=660)
    global rm_entry
    rm_entry = Entry(TF,font = ("Modern",10))
    rm_entry.place(x=170,y=650,width=75)
    rm3 =Label(TF,text='\u03A9',font = ("Modern",10))
    rm3.place(x=250,y=650)

    # tau calculation
    tau1 =Label(TF,text='\u03C4    =',font = ("Modern",10))
    tau1.place(x=123,y=680)
    global tau_entry
    tau_entry = Entry(TF,font = ("Modern",10))
    tau_entry.place(x=170,y=680,width=75)
    tau3 =Label(TF,text='s',font = ("Modern",10))
    tau3.place(x=250,y=680)

    # G0Rm0 calculation

    grm1 =Label(TF,text='R     =',font = ("Modern",10))
    grm2 =Label(TF,text='m0',font = ("Modern",6))
    grm1.place(x=123,y=710)
    grm2.place(x=137,y=720)
    gG1 =Label(TF,text='G',font = ("Modern",10))
    gG2 =Label(TF,text='0',font = ("Modern",6))
    gG1.place(x=100,y=710)
    gG2.place(x=115,y=720)
    global gmr_entry
    gmr_entry = Entry(TF,font = ("Modern",10))
    gmr_entry.place(x=170,y=710,width=75)
    grm3 =Label(TF,text='\u03A9',font = ("Modern",10))
    grm3.place(x=250,y=710)

    btnRM.place(x=500,y=620)

    # NSAMPLES show
    nsamples =Label(TF,text='N° Samples',font = ("Modern",10))
    nsamples.place(x=450,y=380)
    global nsamples_entry 
    nsamples_entry = Entry(TF,font = ("Modern",10))
    nsamples_entry.place(x=550,y=380,width=70)
    nsamples_entry.insert(END,'16384')

    # dT show
    dt1 =Label(TF,text='dT  =',font = ("Modern",10))
    dt1.place(x=500,y=410)
    global dt_entry
    dt_entry = Entry(TF,font = ("Modern",10))
    dt_entry.place(x=550,y=410,width=70)
    dt_entry.insert(END,'1e-12')
    dt3 =Label(TF,text='s',font = ("Modern",10))
    dt3.place(x=630,y=410)

    # Bound show
    bound1 =Label(TF,text='Bound Fit integer  =',font = ("Modern",10))
    bound1.place(x=405,y=440)
    global bound_entry
    bound_entry = Entry(TF,font = ("Modern",10))
    bound_entry.place(x=550,y=440,width=70)
    bound_entry.insert(END,'5')

    # Leading Edge show
    LE1 =Label(TF,text='Leading Edge threshold  =',font = ("Modern",10))
    LE1.place(x=365,y=500)
    global LE_entry
    LE_entry = Entry(TF,font = ("Modern",10))
    LE_entry.place(x=550,y=500,width=70)
    LE_entry.insert(END,'0.01')
    LE3 =Label(TF,text='V',font = ("Modern",10))
    LE3.place(x=630,y=500)

    # CFD show
    CFD1 =Label(TF,text='CFD threshold fraction    =',font = ("Modern",10))
    CFD1.place(x=362,y=530)
    global CFD_entry
    CFD_entry = Entry(TF,font = ("Modern",10))
    CFD_entry.place(x=550,y=530,width=70)
    CFD_entry.insert(END,'0.35')

    # ARC method show
    ARC1 =Label(TF,text='ARC method delay        =',font = ("Modern",10))
    ARC1.place(x=365,y=560)
    global ARC_entry
    ARC_entry = Entry(TF,font = ("Modern",10))
    ARC_entry.place(x=550,y=560,width=70)
    ARC_entry.insert(END,'200e-12')
    ARC1 =Label(TF,text='s',font = ("Modern",10))
    ARC1.place(x=630,y=560)

   

 #
def openTFgui():
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

    btn3 = Button(TF, text="Charge Sensitive Amplifier",command=csastg,font = ("Arial",12))
    btn3.place(x=435,y=50)

    btn4 = Button(TF, text="From File             ",command=FromFile, font = ("Arial",12))
    btn4.place(x=100,y=100)

    btn5 = Button(TF, text="Waveform Analysis", command=WaveAnalysis, font = ("Arial",12))
    btn5.place(x=260,y=100)

    my_canvas1 = Canvas(TF,width=750,height=1,bg='black')
    my_canvas1.place(x=20,y=150)

    global btnRM1
    btnRM1 = Button(TF, text="Get TIA Values\n and \n SET Transfer Function",command=rm,font = ("Arial",9))

    global btnRM2
    btnRM2 = Button(TF, text="Get CSA Values\n and \n SET Transfer Function",command=rmcsa,font = ("Arial",9))

    global btnRM
    btnRM = Button(TF, text="Get TIA Values\n and \n SET Transfer Function",command=rm,font = ("Arial",9))

    global btnRM3
    btnRM3 = Button(TF, text="SET \nTransfer Function",command=rm,font = ("Arial",9))

    global btnFromFile
    btnFromFile = Button(TF, text="Choose Transfer Function file",command=FromFileopen,font = ("Arial",9))

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

    TF.mainloop()

def callback(url):
    webbrowser.open_new(url)

def writeCFG( ):
    text_file = open("../etc/configuration.cfg", 'w+')
    text_file.write('\nInputDirectory     =' + '"' + folder_selected1 + '";\n')
    text_file.write('\nOutputDirectory     =' + '"' + folder_selected2 + '";\n')
    
    if (var1.get() == 1 and var2.get() == 0):
        text_file.write('\nInputFileExtension = ".txt";\n')
        text_file.write('NlinesToSkip       = 0;\n')
        text_file.write('token              = " ";\n')
        text_file.write('column             = 1;\n')

    elif (var1.get() == 0 and var2.get() == 1):
        text_file.write('\nInputFileExtension = ".dat";\n')
        text_file.write('NlinesToSkip       = 0;\n')
        text_file.write('token              = " ";\n')
        text_file.write('column             = 1;\n')

    elif (var1.get()==var2.get()):
        text_file.write('\nInputFileExtension = ".txt";\n')
        text_file.write('NlinesToSkip       = 0;\n')
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

    text_file.write('\nLandauFluctuation            = false;\n')
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

    text_file.close()
    
    subprocess.run(["./../build/analysis_tbb", "ls -l"])


my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_label7 = Label(root, text='TFBOOST CONFIGURATION GUI',font = ("Modern",9))
my_label7.place(x=20,y=85)

my_button = Button(root, text="Choose directory for the input currents:",command=openFileInput,font = ("Arial",9))
my_button.place(x=20,y=110)

my_label3 = Label(root, text='Select file extention:',font = ("Arial",8))
my_label3.place(x=20,y=141)

def switchstate2():

    if var2.get()==1:
        var1.set(0)

def switchstate1():

    if var1.get()==1:
        var2.set(0)

var1 = IntVar()
var1.set(1)
selfcheck1 = Checkbutton(root, text="txt", variable=var1,command=switchstate1, font = ("Arial",8)).place(x=120, y=140)
var2 = IntVar()
selfcheck2 = Checkbutton(root, text="dat", variable=var2,command=switchstate2,font = ("Arial",8)).place(x=150, y=140)

my_button2 = Button(root, text="Choose directory for the output files and plots:",command=openFileOutput,font = ("Arial",9))
my_button2.place(x=20,y=160)

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
LE_noff.insert(END,'0.001')

my_label6 = Label(root, text='   Leading Edge noise-on (V):',font = ("Arial",9))
my_label6.place(x=460,y=337)
LE_non = Entry(root,font = ("Arial",10))
LE_non.place(x=630,y=337,width=40)
LE_non.insert(END,'0.001')

#---------------MEASUREMENTS AND SAVING OPTION-----------------------------
my_canvas1 = Canvas(root,width=750,height=1,bg='black')
my_canvas1.place(x=20,y=365)

my_label8 = Label(root, text='MEASUREMENTS AND SAVING OPTIONS',font = ("Arial Bold",9))
my_label8.place(x=20,y=367)

my_label6 = Label(root, text='Save output files:',font = ("Arial",9))
my_label6.place(x=20,y=390)
var6 = IntVar()
Checkbutton(root, variable=var6,font = ("Arial",8)).place(x=120, y=389)

my_label6 = Label(root, text='   Linear Fit near Threshold:',font = ("Arial",9))
my_label6.place(x=220,y=390)
var7 = IntVar()
Checkbutton(root, variable=var7,font = ("Arial",8)).place(x=375, y=389)

my_label6 = Label(root, text='   Gaussian Fit near Vmax:',font = ("Arial",9))
my_label6.place(x=460,y=390)
var8 = IntVar()
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

make_btn = Button(root, text='MAKE NEW ANALYSIS',command=openTFgui,font = ("Arial",11))
make_btn.place(x=400,y=645)

btn_write = Button(root, text=" RUN ",command=writeCFG,font = ("Arial",16))
btn_write.place(x=400,y=700)

#btn_result = Button(root, text="RESULTS",font = ("Arial",16))
#btn_result.place(x=400,y=700)



root.mainloop()