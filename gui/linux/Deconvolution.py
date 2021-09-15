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
 *  Created on: 05/08/2021
 *      Author:  Gian Matteo Cossu
 */ """





from tkinter import *
from tkinter import filedialog, constants
from tkinter import font,ttk
from PIL import ImageTk,Image
import os
import tkinter as tk
import subprocess
import webbrowser
import math
import random
import pandas as pd
import matplotlib.pyplot as plt

decW = tk.Tk();
decW.title('TFBoost Deconvolution GUI')
decW.geometry("800x750")
decW.resizable(0, 0)
decW.tk.call('wm', 'iconphoto', decW._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

def selectinput():
    global filechosen
    filechosen = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(decW, state='disabled',font = ("Arial",11), width=50, height=1)
    text1.place (x=185,y=173,width=200)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(filechosen.name)[1])
    text1.configure(state="disabled")


def selectoutput():
    global filechosen1
    filechosen1 = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(decW, state='disabled',font = ("Arial",11), width=50, height=1)
    text1.place (x=185,y=213,width=200)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(filechosen1.name)[1])
    text1.configure(state="disabled")

def selectinputspline():
    global filechosen2
    filechosen2 = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(decW, state='disabled',font = ("Arial",11), width=50, height=1)
    text1.place (x=185,y=663,width=200)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(filechosen2.name)[1])
    text1.configure(state="disabled")

def tokenget(event) :
     
    if (tokendrop.get()=='space'):
        token_entry.set(' ')
    if (tokendrop.get()=='tab'):
        token_entry.set('   ')
    if (tokendrop.get()=='semicolon'):
        token_entry.set(';')
    if (tokendrop.get()=='comma'):
        token_entry.set(',')
    if (tokendrop.get()=='LTSpice'):
        token_entry.set('\t')
    if (tokendrop2.get()=='space'):
        token_entry2.set(' ')
    if (tokendrop2.get()=='tab'):
        token_entry2.set('   ')
    if (tokendrop2.get()=='semicolon'):
        token_entry2.set(';')
    if (tokendrop2.get()=='comma'):
        token_entry2.set(',')
    if (tokendrop2.get()=='LTSpice'):
        token_entry2.set('\t')

    print(token_entry2.get())
    print(token_entry.get())

def openFileOutput():
    global folder_selected3
    folder_selected3 = filedialog.askdirectory()
    directory = os.path.split(folder_selected3)[0] + '/' + os.path.split(folder_selected3)[1]
    text1 = Text(decW, state='disabled', width=50, height=1, font = ("Arial",11))
    text1.place (x=200,y=493)
    text1.configure(state="normal")
    text1.insert('end', directory)
    text1.configure(state="disabled")


def writeCFG( ):
    text_file = open("../../etc/deconvolution.cfg", 'w+')
    text_file.write('\nDeconvolution = true;\n') 

    text_file.write('\nInputFileConvorTF =' + '"' + os.path.split(filechosen1.name)[0] + '/' + os.path.split(filechosen1.name)[1] + '";\n')

    text_file.write('dT       = ' + dTdec.get()  +';\n')
    text_file.write('Nlinestoskip1       = ' + LTskip.get()  +';\n')
    text_file.write('Nlinestoskip2       = ' + LTskip2.get()  +';\n')
    a_float = float(scaleF.get())
    formatted_float = "{:.1f}".format(a_float)
    text_file.write('scale_factor      = ' + str(formatted_float)  +';\n')

    text_file.write('\nInputFileCurrent  = ' + '"' + os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1] + '";\n')
    text_file.write('token1    =' + '"' + token_entry.get() + '";\n')
    text_file.write('token2    =' + '"' + token_entry.get() + '";\n')
    text_file.write('columnT  = 0;\n')
    text_file.write('columnI  = 1;\n')
    text_file.write('Nsamples =' + nsamplesdrop.get() + ';\n')
    text_file.write('\noffset          = 0; \n')

    if (var13.get()==1):
        text_file.write('\nfilter    = true; \n')
    if (var13.get()==0):
        text_file.write('filter    = false; \n')

    text_file.write('frequency =' + filterF_entry.get() +';\n')
    text_file.write('order     =' + filterN_entry.get() +';\n')

    text_file.write('\nOutputDirectory ='  + '"' + os.path.split(folder_selected3)[0] + '/' + os.path.split(folder_selected3)[1] +'/";\n')
    my_str = str(nameentry.get())
    my_str = my_str[:-4]
    text_file.write('\nOutputFileName  =' + '"'+ my_str +'";\n')
 
    text_file.close()

    subprocess.run(["./../../build/deconvolution_tbb", "ls -l"])


def run_results ():

    plt.clf()
    

    if (varlt.get()==0):
        data1 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1],sep='\s+',header=None)
        data1 = pd.DataFrame(data1)
        data2 = pd.read_csv(os.path.split(filechosen1.name)[0] + '/' + os.path.split(filechosen1.name)[1],sep='\s+',header=None)
        data2 = pd.DataFrame(data2)

    if (varlt.get()==1):
        data1 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1],sep='\s+',header=None, skiprows=1)
        data1 = pd.DataFrame(data1)
        data2 = pd.read_csv(os.path.split(filechosen1.name)[0] + '/' + os.path.split(filechosen1.name)[1],sep='\s+',header=None, skiprows=1)
        data2 = pd.DataFrame(data2)

    x1 = data1[0]
    y1 = data1[1]

    plt.subplot(3, 1, 1)
    plt.plot(x1, y1, 'r-',label=os.path.split(filechosen.name)[1])
    plt.title('Results')
    plt.xlabel('time (s)')
    plt.ylabel('Current [A]')

    plt.legend()

    x2 = data2[0]
    y2 = data2[1]
    plt.subplot(3, 1, 2)
    plt.plot(x2, y2, 'g-',label=os.path.split(filechosen1.name)[1])
    plt.xlabel('time (s)')
    plt.ylabel('Voltage [V]')

    plt.legend()
 
    data3 = pd.read_csv(os.path.split(folder_selected3)[0] + '/' + os.path.split(folder_selected3)[1] +'/'+ str(nameentry.get()),sep='\s+',header=None)
    data3 = pd.DataFrame(data3)

    x3 = data3[0]
    y3 = data3[1]
    plt.subplot(3, 1, 3)
    plt.plot(x3, y3, 'b-',label=str(nameentry.get()))
    plt.xlabel('time (s)')
    plt.ylabel('Trans [Ohm]')

    plt.legend()
    plt.tight_layout(pad=1.0)
    plt.show()

def ltspiceDEC ():

    if(varlt.get()==1):
        tokendrop.current(4)
        tokendrop2.current(4)
        token_entry.set('\t')
        token_entry2.set('\t')
        LTskip .delete(0,END)
        LTskip .insert(END,'1')
        LTskip2.delete(0,END)
        LTskip2 .insert(END,'1')

    if(varlt.get()==0):
        tokendrop.current(0)
        tokendrop2.current(0)
        token_entry.set(' ')
        token_entry2.set(' ')
        LTskip .delete(0,END)
        LTskip .insert(END,'0')
        LTskip2.delete(0,END)
        LTskip2 .insert(END,'0')


my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_label7 = Label(decW, text='TFBOOST DECONVOLUTION GUI',font = ("Modern",9))
my_label7.place(x=20,y=85)

my_canvas0 = Canvas(decW,width=750,height=1,bg='black')
my_canvas0.place(x=20,y=100)

lab2 = Label(decW, text='Perform deconvolution with TFBoost using a input signal and the corresponding output signal:',font = ("Modern",9))
lab2.place(x=30,y=120)
lab2b = Label(decW, text='(the time step is set to 1ps by default)',font = ("Modern",9))
lab2b.place(x=30,y=140)

btnIN = Button(decW,text='Choose Input file:',command = selectinput,font = ("Arial",11), width=15)
btnIN.place(x=30,y=170)

btnOUT = Button(decW,text='Choose Output file:',command = selectoutput,font = ("Arial",11), width=15)
btnOUT.place(x=30,y=210)

lab3 = Label(decW,text='Lines to skip in input file:',font = ("Arial",11))
lab3.place(x=30,y=260)

LTskip = Entry(decW,font = ("Arial",11))
LTskip .place(x=200,y=260,width=40)
LTskip .insert(END,'0')

lab3b = Label(decW,text='Lines to skip in output file:',font = ("Arial",11))
lab3b.place(x=300,y=260)

LTskip2 = Entry(decW,font = ("Arial",11))
LTskip2 .place(x=470,y=260,width=40)
LTskip2 .insert(END,'0')

lab3c = Label(decW,text='LTSpice signals:',font = ("Arial",11))
lab3c.place(x=535,y=260)
varlt = IntVar()
Checkbutton(decW, variable=varlt,command=ltspiceDEC,font = ("Arial",11)).place(x=645, y=258)

lab4 = Label(decW,text='Scale factor:',font = ("Arial",11))
lab4.place(x=30,y=290)

scaleF = Entry(decW,font = ("Arial",11))
scaleF .place(x=200,y=290,width=40)
scaleF .insert(END,'1')


#choose the token#################################
lab5 = Label(decW,text='Token1:',font = ("Arial",11))
lab5.place(x=30,y=320)

#choose the token of the output file #################################
lab5b = Label(decW,text='Token2:',font = ("Arial",11))
lab5b.place(x=350,y=320)

tokendrop = ttk.Combobox(decW, width = 10, font=("Arial",11))

tokendrop2 = ttk.Combobox(decW, width = 10, font=("Arial",11))
  
  
# Adding combobox drop down list
tokendrop['values'] = ('space', 
                          'tab',
                          'semicolon',
                          'comma',
                          'LTSpice'
)

# Adding combobox drop down list
tokendrop2['values'] = ('space', 
                          'tab',
                          'semicolon',
                          'comma',
                          'LTSpice'
)

global token_entry, token_entry2
token_entry = StringVar()
token_entry2 = StringVar()

tokendrop.current(0)
tokendrop2.current(0)
token_entry.set(' ')
token_entry2.set(' ')

tokendrop.place(x=200,y=320)
tokendrop2.place(x=415,y=320)
tokendrop.bind("<<ComboboxSelected>>", tokenget)
tokendrop2.bind("<<ComboboxSelected>>", tokenget)


# set Nsamples #################################
lab6 = Label(decW,text='Nsamples:',font = ("Arial",11))
lab6.place(x=30,y=350)

n1 = StringVar()
nsamplesdrop = ttk.Combobox(decW, width = 10,
                            textvariable = n1, font=("Arial",11))
  
# Adding combobox drop down list
nsamplesdrop['values'] = ('8192', 
                          '16384',
                          '32768',
                          '65536',
                          '131072',
                          '262144',
                          '524288')

nsamplesdrop.current(2)

nsamplesdrop.place(x=200,y=350)

lab6b = Label(decW,text='dT:',font = ("Arial",11))
lab6b.place(x=380,y=350)

dTdec = Entry(decW,font = ("Arial",11))
dTdec .place(x=415,y=350,width=70)
dTdec .insert(END,'1e-12')

#POSTPROCESSING FILTER
my_label6 = Label(decW, text='Filter transfer function:',font = ("Arial",11))
my_label6.place(x=30,y=380)
var13 = IntVar()
Checkbutton(decW, variable=var13,font = ("Arial",11)).place(x=192, y=380)
var13.set(1)

my_label7 = Label(decW, text='Filter Order n°:',font = ("Arial",11))
my_label7.place(x=30,y=410)
filterN_entry = Entry(decW,font = ("Arial",11))
filterN_entry.place(x=200,y=410,width=40)
filterN_entry.insert(END,'10')

my_label8 = Label(decW, text='Filter cut-off Frequency:',font = ("Arial",11))
my_label8.place(x=30,y=440)
filterF_entry = Entry(decW,font = ("Arial",11))
filterF_entry.place(x=200,y=440,width=40)
filterF_entry.insert(END,'10e9')

my_button2 = Button(decW, text="Set output folder:",command=openFileOutput,font = ("Arial",11), width=15)
my_button2.place(x=30,y=490)

nameLab = Label(decW, text='Set output Filename:',font = ("Arial",11))
nameLab.place(x=30,y=530)

nameentry = Entry(decW,font = ("Arial",11))
nameentry.place(x=200,y=530)
nameentry.insert(END,'filename.txt')


btnRun = Button(decW, text="Make \n deconvolution",command=writeCFG,font = ("Arial",13))
btnRun.place(x=490,y=380)

btnRes = Button(decW, text="Plot \n results", command= run_results, font = ("Arial",13))
btnRes.place(x=650,y=380)



decW.mainloop()
