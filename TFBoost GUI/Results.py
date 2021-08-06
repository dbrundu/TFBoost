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

resW = tk.Tk();
resW.title('TFBoost GUI')
resW.geometry("800x750")
resW.resizable(0, 0)
resW.tk.call('wm', 'iconphoto', resW._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

InputDone = IntVar();
OutputDone = IntVar();

def openFileInput():
    global folder_selected1
    folder_selected1 = filedialog.askdirectory()
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
    directory = os.path.split(folder_selected2)[0] + '/' + os.path.split(folder_selected2)[1]
    text1 = Text(resW, state='disabled', width=50, height=1)
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

    OutputDone.set(1)

def run_results ():

    if InputDone.get()==1 and OutputDone.get()==1:
        path1 =folder_selected1
        files1 = os.listdir(path1)
        index1 = random.randrange(0, len(files1))
        print(files1[index1])

        path2 =folder_selected2 + '/data'
        files2 = os.listdir(path2)
        index2 = random.randrange(0, len(files2))
        print(files2[index2])

    plt.clf()

    data1 = pd.read_csv(path1+'/'+files2[index2],sep='\s+',header=None)
    data1 = pd.DataFrame(data1)
    data2 = pd.read_csv(path2+'/'+files2[index2],sep='\s+',header=None)
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
    
def openHistTOACFD ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_CFD.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTOALE ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_LE.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTOAARC ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_RM.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtCFD ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_CFD.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtLE ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_LE.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtARC ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_RM.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTpeak ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTpeak.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVpeak ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVpeak.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThCFD ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThCFD.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThLE ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThLE.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThARC ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThRM.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTOACFDnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_CFD_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTOALEnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_LE_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTOAARCnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTOA_RM_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtCFDnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_CFD_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtLEnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_LE_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistdVdtARCnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histdvdt_RM_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistTpeaknoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histTpeak_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVpeaknoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVpeak_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThCFDnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThCFD_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThLEnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThLE_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a

def openHistVonThARCnoise ():
    if OutputDone.get()==1:
        shellCommand = ['root']
        shellCommand.append(" " + folder_selected2 + "/plots/histVonThRM_noise.C")
        print("Run Macro", shellCommand)
        a = subprocess.Popen(shellCommand)
        return a



my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=105)

my_label7 = Label(resW, text='TFBOOST RESULTS GUI',font = ("Modern",9))
my_label7.place(x=20,y=85)

my_button = Button(resW, text="Choose directory for the input currents:",command=openFileInput,font = ("Arial",9))
my_button.place(x=20,y=110)

my_label3 = Label(resW, text='Select file extention:',font = ("Arial",8))
my_label3.place(x=20,y=141)

def switchstate2():

    if var2.get()==1:
        var1.set(0)

def switchstate1():

    if var1.get()==1:
        var2.set(0)

var1 = IntVar()
var1.set(1)
selfcheck1 = Checkbutton(resW, text="txt", variable=var1,command=switchstate1, font = ("Arial",8)).place(x=120, y=140)
var2 = IntVar()
selfcheck2 = Checkbutton(resW, text="dat", variable=var2,command=switchstate2,font = ("Arial",8)).place(x=150, y=140)

my_button2 = Button(resW, text="Choose directory for the output files and plots:",command=openFileOutput,font = ("Arial",9))
my_button2.place(x=20,y=160)

res_btn = Button(resW, text="\nPRESS TO VISUALIZE RANDOM WAVEFORM\n",command=run_results,font = ("Arial",9))
res_btn.place(x=270,y=200)


my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=260)

my_label7 = Label(resW, text='HISTOGRAMS WITHOUT NOISE',font = ("Modern",9))
my_label7.place(x=20,y=270)

ToaCFDbtn = Button(resW, text="\n TOA with CFD \n",command=openHistTOACFD,font = ("Arial",9))
ToaCFDbtn.place(x=60,y=300)

ToaLEbtn = Button(resW, text="\n TOA with LE \n",command=openHistTOALE,font = ("Arial",9))
ToaLEbtn.place(x=175,y=300)

ToaARCbtn = Button(resW, text="\n TOA with ARC \n",command=openHistTOAARC,font = ("Arial",9))
ToaARCbtn.place(x=280,y=300)

dvdtCFDbtn = Button(resW, text="\n dVdt with CFD \n",command=openHistdVdtCFD,font = ("Arial",9))
dvdtCFDbtn.place(x=395,y=300)

dvdtLEbtn = Button(resW, text="\n dVdt with LE  \n",command=openHistdVdtLE,font = ("Arial",9))
dvdtLEbtn.place(x=510,y=300)

dvdtARCbtn = Button(resW, text="\n dVdt with ARC \n",command=openHistdVdtARC,font = ("Arial",9))
dvdtARCbtn.place(x=618,y=300)

Tpeakbtn = Button(resW, text="\n  Time at peak  \n",command=openHistTpeak,font = ("Arial",9))
Tpeakbtn.place(x=60,y=360)

Vpeakbtn = Button(resW, text="\n Volt. at peak \n",command=openHistVpeak,font = ("Arial",9))
Vpeakbtn .place(x=175,y=360)

VonThCFDbtn = Button(resW, text="\n V on Th (CFD) \n",command=openHistVonThCFD,font = ("Arial",9))
VonThCFDbtn.place(x=280,y=360)

VonThLEbtn = Button(resW, text="\n   V on Th (LE)  \n",command=openHistVonThLE,font = ("Arial",9))
VonThLEbtn.place(x=395,y=360)

VonThRMbtn = Button(resW, text="\n V on Th (RM) \n",command=openHistVonThARC,font = ("Arial",9))
VonThRMbtn.place(x=510,y=360)


my_canvas = Canvas(resW,width=750,height=1,bg='black')
my_canvas.place(x=20,y=460)

my_label7 = Label(resW, text='HISTOGRAMS WITH NOISE',font = ("Modern",9))
my_label7.place(x=20,y=470)

ToaCFD_noisebtn = Button(resW, text="\n TOA with CFD \n",command=openHistTOACFDnoise,font = ("Arial",9))
ToaCFD_noisebtn.place(x=60,y=500)

ToaLE_noisebtn = Button(resW, text="\n TOA with LE \n",command=openHistTOALEnoise,font = ("Arial",9))
ToaLE_noisebtn.place(x=175,y=500)

ToaARC_noisebtn = Button(resW, text="\n TOA with ARC \n",command=openHistTOAARCnoise,font = ("Arial",9))
ToaARC_noisebtn.place(x=280,y=500)

dvdtCFD_noisebtn = Button(resW, text="\n dVdt with CFD \n",command=openHistdVdtCFDnoise,font = ("Arial",9))
dvdtCFD_noisebtn.place(x=395,y=500)

dvdtLE_noisebtn = Button(resW, text="\n dVdt with LE  \n",command=openHistdVdtLEnoise,font = ("Arial",9))
dvdtLE_noisebtn.place(x=510,y=500)

dvdtARCbtn = Button(resW, text="\n dVdt with ARC \n",command=openHistdVdtARCnoise,font = ("Arial",9))
dvdtARCbtn.place(x=618,y=500)

Tpeak_noisebtn = Button(resW, text="\n  Time at peak  \n",command=openHistTpeaknoise,font = ("Arial",9))
Tpeak_noisebtn.place(x=60,y=560)

Vpeak_noisebtn = Button(resW, text="\n Volt. at peak \n",command=openHistVpeaknoise,font = ("Arial",9))
Vpeak_noisebtn .place(x=175,y=560)

VonThCFD_noisebtn = Button(resW, text="\n V on Th (CFD) \n",command=openHistVonThCFDnoise,font = ("Arial",9))
VonThCFD_noisebtn.place(x=280,y=560)

VonThLE_noisebtn = Button(resW, text="\n   V on Th (LE)  \n",command=openHistVonThLEnoise,font = ("Arial",9))
VonThLE_noisebtn.place(x=395,y=560)

VonThRM_noisebtn = Button(resW, text="\n V on Th (RM) \n",command=openHistVonThARCnoise,font = ("Arial",9))
VonThRM_noisebtn.place(x=510,y=560)




resW.mainloop()