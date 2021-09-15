/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gian Matteo Cossu
 *
 *   This file is part of TFBoost Library.
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
 *  Created on: 22/08/2021
 *    Author: Gian Matteo Cossu
 */

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TSystem.h"
#include "TRandom3.h"


/* 
THIS IS A ROOT MACRO
root -l currents.c (to run)

TOY MODEL FOR AN IDEAL 3D DETECTOR WITH 2 ELECTRODES AT DISTANCE d
ASSUMING DRIFT VELOCITY AT SATURATION FOR BOTH CARRIERS:

THE PROGRAM ALLOW TO MAKE A DEPOSIT WITH RANDOM DIRECTION
AND SAVE THE CURRENTS INDUCED AT DIFFERENT Z POSIZIONS 
*/


using namespace std;
void currents();

int Main(){
    
currents();
	
return 0;

}

void currents(){


cout << "Create folders:"<< endl;
gSystem->Exec("mkdir Currents");
gSystem->Exec("mkdir Summed");
cout << "folder 'Currents' created"<< endl;
cout << "folder 'Summed' created"<< endl;
	
// NUMBER OF SAMPLES (1 ps STEP SIZE)
int N=1000;

//NUMBER OF TRACKS
int NTracks=1000;

//NUMBER OF SLICES IN Z
int Ndz=20;

// CONTAINERS FOR CURRENTS AT EVERY Z
double Ie[N][Ndz];
double Ih[N][Ndz];


//CONTAINERS FOR TOTAL CURRENTS
double IeT[N];
double IhT[N];

for(int i=0; i<N; i++){
IeT[i]=0.0;
IhT[i]=0.0;
}

// CHARGE COLLECTION TIME FOR THE CARRIERS
double te, th;

// DISTANCE BETWEEN ELECTRODES (~39um ATLAS geometry)
double d=39; 

// POSITION VARIABLE
double x=0;

// SATURATION VELOCITIES (diamond)
double ve_sat=0.27; // um/ps
double vh_sat=0.10; // um/ps

// DEPOSITED CHARGE: 3.8*1e-3= 3.8 fC							
float qi=3.8*1e-3/Ndz;	

	for(int k=1; k<=NTracks; k++){

    // CASUAL POSITION AT Z=0 AND Z=h (RANDOM DIRETION FOR EVERY TRACK)
	double x1 = gRandom->Rndm() * d;
	//double x2 = gRandom->Rndm() * d;

	// if x1=x2 tracks parallel to columns
	double x2 = x1;

		for (int j=1; j<=Ndz; j++){
 
            // GET X POSITION OF CLUSTER OF CHARGE AT Z
			x = x1 - (x1-x2)*j/Ndz;

			//cout<<" x =" << x<< endl;
 
            //CALCULATE DURATION OF DRIFT
			te = x/ve_sat;
			th = (d-x)/vh_sat;

			//cout<<" te =" << te << endl;
			//cout<<" th =" << th << endl;

            // SAVE A FILE FOR EVERY CURRENT AT DIFFERENT Z FOR EVERY TRACK
			ofstream out;
			string file;
			if(j<10){
			file="Currents/TCODE"+ to_string(k) +"_z0" + to_string(j)  + ".txt";
			out.open(file);
			}

			if(j>=10){
			file="Currents/TCODE"+ to_string(k) +"_z" + to_string(j)  + ".txt";
			out.open(file);
			}

			for (int i=0; i<N; i++){

				if(i<te) {
				Ie[i][j] = ( qi* ve_sat/d );
				IeT[i]+=( qi* ve_sat/d );
				}
				else if (i>=te){
				Ie[i][j]= 0.0;
				}

				if(i<th){
				Ih[i][j] = ( qi* vh_sat/d );
				IhT[i]+=( qi* vh_sat/d );
				}
				else if (i>=th){
				Ih[i][j]= 0.0;
				}

			if(i==0)
			out<< i*1e-12 <<" "<< 0.<< " " << 0. << " " << 0. << endl;	 
			else
			out<< i*1e-12 <<" "<< Ie[i][j] + Ih[i][j]<< " " << Ie[i][j] << " " << Ih[i][j] << endl;	
			

			//AFTER THE FILE IS WRITTEN EMPTY THE CONTAINERS
			for(int i=0; i<N; i++){

			Ie[i][j]=0;
			Ih[i][j]=0;

			}

			}

			

		}

	int plotTot=1;
	

	//SAVE ALSO THE TRACK FILE AS A SUM OF THE CURRENTS AT ALL Z POSITIONS
	if(plotTot){
	ofstream out1;
	string file1;
	file1="Summed/TCODE" + to_string(k) + ".txt";
	out1.open(file1);  

	double Qe=0,Qh=0;
	for(int i=0; i<N; i++){
		if(i==0)
			out1<< i*1e-12 <<" "<< 0.<< " " << 0. << " " << 0. << " "<< 0. << " " << x1 << " "<< 100.  << " "<< x2 << endl;
		else
		out1<< i*1e-12 <<" "<< IeT[i] + IhT[i]<< " " << IeT[i] << " " << IhT[i] <<endl;
		Qe+=IeT[i];
		Qh+=IhT[i];
	}

	//AFTER THE FILE IS WRITTEN EMPTY THE CONTAINERS
	for(int i=0; i<N; i++){

	IeT[i]=0;
	IhT[i]=0;

	}

	cout<<" summed file n°" << k << endl;
	cout<<" total charge =" << (Qe+Qh) * 1e-12<< endl;
	//cout<<" x1 =" << x1<< endl;
	//cout<<" x2 =" << x2<< endl;
	

	}

	}

}
