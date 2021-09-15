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

samW = tk.Tk();
samW.title('TFBoost Resampling GUI')
samW.geometry("800x750")
samW.resizable(0, 0)
samW.tk.call('wm', 'iconphoto', samW._w, tk.PhotoImage(file='TFB_guiFiles/logoico.png'))

single = IntVar();
nfiles = IntVar();

def selectinput():
    global filechosen
    filechosen = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(samW, state='disabled',font = ("Arial",11), width=50, height=1)
    text1.place (x=185,y=173,width=200)
    text1.configure(state="normal")
    text1.insert('end', os.path.split(filechosen.name)[1])
    text1.configure(state="disabled")
    single.set(1)
    my_ext = str(os.path.splitext(filechosen.name)[1])
    ext_entry .delete(0,END)
    ext_entry .insert(END,my_ext)

def selectinputs():
    global folder_selectedx
    folder_selectedx = filedialog.askdirectory()
    global directory
    directory = os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1]
    text1 = Text(samW, state='disabled',font = ("Arial",11), width=150, height=1)
    text1.place (x=185,y=213,width=450)
    text1.configure(state="normal")
    text1.insert('end', directory)
    text1.configure(state="disabled")
    single.set(0)
    nfiles.set(len(os.listdir(directory)))

def selectinputspline():
    global filechosen2
    filechosen2 = filedialog.askopenfile(initialdir="", title="Select a File")
    #filelabel = Label(root, text=filechosen,font = ("Arial",))
    #directory = os.path.split(filechosen)[0] + '/' + os.path.split(filechosen)[1]
    text1 = Text(samW, state='disabled',font = ("Arial",11), width=50, height=1)
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
    print(token_entry.get())

def openFileOutput():
    global folder_selected3
    folder_selected3 = filedialog.askdirectory()
    directory = os.path.split(folder_selected3)[0] + '/' + os.path.split(folder_selected3)[1]
    text1 = Text(samW, state='disabled', width=50, height=1, font = ("Arial",11))
    text1.place (x=200,y=493)
    text1.configure(state="normal")
    text1.insert('end', directory)
    text1.configure(state="disabled")


def writeCFG( ):
    text_file = open("../../etc/resampling.cfg", 'w+')
    if(single.get()==1):
        text_file.write('\nSingleFile = true;\n') 
        text_file.write('\nInputFileCurrent  = ' + '"' + os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1] + '";')
        text_file.write('\nInputDirectory =' + '"none";\n')

    if(single.get()==0):
        text_file.write('\nSingleFile = false;\n') 
        text_file.write('\nInputFileCurrent  = ' + '"none";')
        text_file.write('\nInputDirectory =' + '"' + os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1] + '";\n')
       

    text_file.write('\ndT       = ' + dTdec.get()  +';\n')
    text_file.write('\nNlinestoskip       = ' + LTskip.get()  +';\n')
    a_float = float(scaleF.get())
    formatted_float = "{:.1f}".format(a_float)
    text_file.write('\nscale_factor      = ' + str(formatted_float)  +';\n')
    text_file.write('\nfile_extension      = "' + ext_entry.get()  +'";\n')    
    text_file.write('\ntoken    =' + '"' + token_entry.get() + '";\n')
    text_file.write('columnT  = 0;\n')
    text_file.write('columnI  = 1;\n')
    text_file.write('\nNsamples =' + nsamplesdrop.get() + ';\n')
    text_file.write('\nNfiles =' + str(nfiles.get()) + ';\n')

    text_file.close()

    os.chdir('../../build')    
    subprocess.run(["./resampling_tbb", "ls -l"])
    os.chdir('../gui/linux') 


def run_results ():

    plt.clf()    

    global label1, label2

    if (varlt.get()==0):

        if(single.get()==1):
            data1 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1],sep='\s+',header=None)
            data1 = pd.DataFrame(data1)
            my_str = str(os.path.split(filechosen.name)[1])
            my_str = my_str[:-4]
            data2 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + my_str + '_new.txt',sep='\s+',header=None)
            data2 = pd.DataFrame(data2)
            label1 = os.path.split(filechosen.name)[1]
            label2 = my_str + '_new.txt'

        if(single.get()==0):
            path1 =os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1]
            files1 = os.listdir(path1)
            index1 = random.randrange(0, len(files1))
            print(files1[index1])

            path2 =os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1] + '/Resampled'
            files2 = os.listdir(path2)

            data1 = pd.read_csv(path1+'/'+files1[index1],sep='\s+',header=None)
            data1 = pd.DataFrame(data1)
            data2 = pd.read_csv(path2+'/'+files1[index1],sep='\s+',header=None)
            data2 = pd.DataFrame(data2)

            label1 = files1[index1]
            label2 = files1[index1]

    if (varlt.get()==1):

        if(single.get()==1):
            data1 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + os.path.split(filechosen.name)[1],sep='\s+',header=None,skiprows=1)
            data1 = pd.DataFrame(data1)
            my_str = str(os.path.split(filechosen.name)[1])
            my_str = my_str[:-4]
            data2 = pd.read_csv(os.path.split(filechosen.name)[0] + '/' + my_str + '_new.txt',sep='\s+',header=None)
            data2 = pd.DataFrame(data2)
            label1 = os.path.split(filechosen.name)[1]
            label2 = my_str + '_new.txt'

        if(single.get()==0):
            path1 =os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1]
            files1 = os.listdir(path1)
            index1 = random.randrange(0, len(files1))
            print(files1[index1])

            path2 =os.path.split(folder_selectedx)[0] + '/' + os.path.split(folder_selectedx)[1] + '/Resampled'
            files2 = os.listdir(path2)

            data1 = pd.read_csv(path1+'/'+files1[index1],sep='\s+',header=None,skiprows=1)
            data1 = pd.DataFrame(data1)
            data2 = pd.read_csv(path2+'/'+files1[index1],sep='\s+',header=None)
            data2 = pd.DataFrame(data2)

            label1 = files1[index1]
            label2 = files1[index1]

    x1 = data1[0]
    y1 = data1[1]

    plt.plot(x1, y1, 'r*',label=label1)
    plt.title('Results')
    plt.xlabel('time (s)')
    plt.ylabel('Arbitrary []')

    plt.legend()

    x2 = data2[0]
    y2 = data2[1]

    plt.plot(x2, y2, 'g+',label=label2)
    plt.xlabel('time (s)')
    plt.ylabel('Arbitrary []')

    plt.legend()
 
    plt.tight_layout(pad=1.0)
    plt.show()

def ltspiceDEC ():

    if(varlt.get()==1):
        tokendrop.current(4)
        token_entry.set('\t')
        LTskip .delete(0,END)
        LTskip .insert(END,'1')

    if(varlt.get()==0):
        tokendrop.current(0)
        token_entry.set(' ')
        LTskip .delete(0,END)
        LTskip .insert(END,'0')


my_logo = ImageTk.PhotoImage(Image.open("TFB_guiFiles/logo.png"))
logo =Label(image=my_logo)
logo.place(x=20,y=10)

my_label7 = Label(samW, text='TFBOOST RESAMPLING GUI',font = ("Modern",9))
my_label7.place(x=20,y=85)

my_canvas0 = Canvas(samW,width=750,height=1,bg='black')
my_canvas0.place(x=20,y=100)

lab2 = Label(samW, text='Perform resampling with TFBoost',font = ("Modern",9))
lab2.place(x=30,y=120)
lab2b = Label(samW, text='Resampling can be done on a single waveform "Choose Input file" or on set of files "Choose Input folder":',font = ("Modern",9))
lab2b.place(x=30,y=140)

btnIN = Button(samW,text='Choose Input file:',command = selectinput,font = ("Arial",11), width=15)
btnIN.place(x=30,y=170)

btnOUT = Button(samW,text='Choose Input folder:',command = selectinputs,font = ("Arial",11), width=15)
btnOUT.place(x=30,y=210)

lab3 = Label(samW,text='Lines to skip in input file:',font = ("Arial",11))
lab3.place(x=30,y=260)

LTskip = Entry(samW,font = ("Arial",11))
LTskip .place(x=200,y=260,width=40)
LTskip .insert(END,'0')

lab3c = Label(samW,text='LTSpice signals:',font = ("Arial",11))
lab3c.place(x=335,y=260)
varlt = IntVar()
Checkbutton(samW, variable=varlt,command=ltspiceDEC,font = ("Arial",11)).place(x=445, y=258)

lab4 = Label(samW,text='Scale factor:',font = ("Arial",11))
lab4.place(x=30,y=290)

scaleF = Entry(samW,font = ("Arial",11))
scaleF .place(x=200,y=290,width=40)
scaleF .insert(END,'1')


#choose the token#################################
lab5 = Label(samW,text='Token:',font = ("Arial",11))
lab5.place(x=30,y=320)

tokendrop = ttk.Combobox(samW, width = 10, font=("Arial",11))

# Adding combobox drop down list
tokendrop['values'] = ('space', 
                          'tab',
                          'semicolon',
                          'comma',
                          'LTSpice'
)

global token_entry
token_entry = StringVar()

tokendrop.current(0)
token_entry.set(' ')

tokendrop.place(x=200,y=320)
tokendrop.bind("<<ComboboxSelected>>", tokenget)


# set Nsamples #################################
lab6 = Label(samW,text='Nsamples:',font = ("Arial",11))
lab6.place(x=30,y=350)

n1 = StringVar()
nsamplesdrop = ttk.Combobox(samW, width = 10,
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

lab6b = Label(samW,text='dT:',font = ("Arial",11))
lab6b.place(x=380,y=350)

dTdec = Entry(samW,font = ("Arial",11))
dTdec .place(x=415,y=350,width=70)
dTdec .insert(END,'1e-12')


lab6e = Label(samW,text='File extension:',font = ("Arial",11))
lab6e.place(x=30,y=380)

ext_entry = Entry(samW,font = ("Arial",11))
ext_entry .place(x=200,y=380,width=40)
ext_entry .insert(END,'.txt')

""" #POSTPROCESSING FILTER
my_label6 = Label(samW, text='Filter transfer function:',font = ("Arial",11))
my_label6.place(x=30,y=380)
var13 = IntVar()
Checkbutton(samW, variable=var13,font = ("Arial",11)).place(x=192, y=380)
var13.set(1)

my_label7 = Label(samW, text='Filter Order n°:',font = ("Arial",11))
my_label7.place(x=30,y=410)
filterN_entry = Entry(samW,font = ("Arial",11))
filterN_entry.place(x=200,y=410,width=40)
filterN_entry.insert(END,'10')

my_label8 = Label(samW, text='Filter cut-off Frequency:',font = ("Arial",11))
my_label8.place(x=30,y=440)
filterF_entry = Entry(samW,font = ("Arial",11))
filterF_entry.place(x=200,y=440,width=40)
filterF_entry.insert(END,'10e9')


# SET OUTPUT FOLDER
my_button2 = Button(samW, text="Set output folder:",command=openFileOutput,font = ("Arial",11), width=15)
my_button2.place(x=30,y=490)

nameLab = Label(samW, text='Set output Filename:',font = ("Arial",11))
nameLab.place(x=30,y=530)

nameentry = Entry(samW,font = ("Arial",11))
nameentry.place(x=200,y=530)
nameentry.insert(END,'filename.txt') """


btnRun = Button(samW, text="Make \n Resampling",command=writeCFG,font = ("Arial",13))
btnRun.place(x=30,y=430)

btnRes = Button(samW, text="Plot \n results", command= run_results, font = ("Arial",13))
btnRes.place(x=180,y=430)

samW.mainloop()
