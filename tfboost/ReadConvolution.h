/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gianmatteo Cossu
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
 *  Created on: 07/05/2020
 *      Author: Davide Brundu
 */
 
#ifndef READ_CONVOLUTION_H_
#define READ_CONVOLUTION_H_
 
    
    
    
namespace tfboost {

template<typename Iterable>
void ReadConvolution(TString const& file, Iterable& iterable)
{
        size_t Nsamples = iterable.size();
        TString line;
        std::ifstream myFile( file.Data() );
        line.ReadLine(myFile);
        size_t k = 0;

        if (myFile.is_open()) 
        {
            for(size_t j = 0; j < Nsamples ; ++j)
            {
                k = j;

                line.ReadLine(myFile);
                if (!myFile.good()) break;
            
                TObjArray *tokens = line.Tokenize( " " );
            
                TString data_str  = ((TObjString*) tokens->At( 1 ) )->GetString();
                
                const double data = atof(data_str);

                iterable[j] = data;
                
                tokens->Delete();
                delete tokens;
            }
        }
        myFile.close();

        const size_t diff = Nsamples - k - 1;
        if(diff>0)
        {
            for(size_t j = k; j < Nsamples; ++j)
            {
                iterable[j] = 0.0;
            }
        }

}


template<typename Iterable>
void ReadTF(TString const& file, Iterable& iterable_t, Iterable& iterable_V, double dT = 1e-12)
{

        size_t Nsamples = iterable_t.size();
        TString line;
        std::ifstream myFile( file.Data() );
        line.ReadLine(myFile);
        size_t k = 0;

        if (myFile.is_open()) 
        {
            for(size_t j = 0; j < Nsamples ; ++j)
            {
                k = j;

                line.ReadLine(myFile);
                if (!myFile.good()) break;
            
                TObjArray *tokens = line.Tokenize( " " );
            
                TString data_str  = ((TObjString*) tokens->At( 1 ) )->GetString();
                
                const double data = atof(data_str);
                
                iterable_t[j] = data;
                iterable_V[j] = data;
                
                tokens->Delete();
                delete tokens;
            }
        }
        myFile.close();

        const size_t diff = Nsamples - k - 1;
        if(diff>0)
        {
            for(size_t j = k; j < Nsamples; ++j)
            {
                iterable_t[j] = j*dT;
                iterable_V[j] = 0.0;
            }
        }

}




} //namespace tfboost
    
    
#endif
