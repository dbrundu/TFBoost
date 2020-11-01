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

    
#include <tfboost/Utils.h>
#include <tfboost/detail/Traits.h>
    
    
namespace tfboost {


namespace detail {

    inline void PushBack_helper(std::vector<double>& cols) {}

    template<typename Iterable, typename ...Iterables>
    inline void PushBack_helper(std::vector<double>& a, Iterable& first, Iterables&... args){
    
        first.push_back( a.back() );
        if(!a.empty() ) a.pop_back();
        
        PushBack_helper(a, args...);
    }


    template<typename ...Iterables, size_t N> 
    inline typename std::enable_if< N==sizeof...(Iterables) && 
                             tfboost::detail::all_true<std::is_floating_point<typename Iterables::value_type>::value...>::value, void>::type
    PushBackTokens(TString const& line,
                   TString const& token, 
                   std::array<int,N> const& columns, 
                   Iterables&... args){
    
        TObjArray *tokens = line.Tokenize( token.Data() );
        
        size_t Ncolumns = tokens->GetEntriesFast();
        
        SAFE_EXIT(Ncolumns < N, "No sufficient columns in file. Exit.")
        
        std::vector<double> data(Ncolumns);
        
        for(size_t i=0; i<Ncolumns; ++i){
            TString data_str  = ((TObjString*) tokens->At( i ) )->GetString();
            data[i] = atof(data_str);
        }
        
        std::reverse(data.begin(), data.end() );

        PushBack_helper(data, args...);
        
    }
    
} //end detail






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
        }  else { SAFE_EXIT(true, "Input file cannot be open.") }
        myFile.close();

        const size_t diff = Nsamples - k - 1;
        if(diff>0)
        {
            for(size_t j = k; j < Nsamples; ++j) 
                iterable[j] = 0.0;
        }
}





template<typename Iterable>
void ReadTF(TString const& file, 
            int Nlinestoskip, 
            Iterable& iterable_t, 
            Iterable& iterable_V, 
            double const& scale = 1.0, 
            bool doublerange=false)
{

        TString line;
        std::ifstream myFile( file.Data() );
        line.ReadLine(myFile);
        size_t k = 0;
        double dT = 0.0;
        double time0 = 0.0;

        
        Iterable iterable_t_temp, iterable_V_temp;

        if (myFile.is_open()) 
        {
        
            for(size_t i=0; i<(size_t)Nlinestoskip; ++i) line.ReadLine(myFile);
            
            while(1)
            {
                line.ReadLine(myFile);
                if (!myFile.good()) break;
            
                TObjArray *tokens = line.Tokenize( " " );
            
                TString time_str  = ((TObjString*) tokens->At( 0 ) )->GetString();
                TString data_str  = ((TObjString*) tokens->At( 1 ) )->GetString();
                
                const double time = atof(time_str);
                const double data = scale*atof(data_str);
                
                //if(k==0) time0 = time;
                if(k==1) dT = time - iterable_t_temp[0];
                
                iterable_t_temp.push_back(time - time0);
                iterable_V_temp.push_back(data);
                
                tokens->Delete();
                delete tokens;
                ++k;
                
            }
            
        } else { SAFE_EXIT(true, "In ReadTF(): Input file cannot be open.") }
        
        
        
        if(doublerange){
        
            double last_t = iterable_t_temp.back();
            
            for(size_t i=0; i<100; ++i ){
                iterable_t_temp.push_back( last_t + i*dT );
                iterable_V_temp.push_back(0.0);
            } 
        
            Iterable time_rev(iterable_t_temp.size());
            Iterable V_rev(iterable_V_temp.size(), 0.0);
            
            hydra::copy(iterable_t_temp, time_rev);
            
            for(double& x : time_rev) x *= -1.0 ;
            
            hydra_thrust::reverse(time_rev.begin() , time_rev.end() ); 

            iterable_t.insert( iterable_t.begin(), time_rev.begin(), time_rev.end() );
            iterable_t.insert( iterable_t.end(), iterable_t_temp.begin(), iterable_t_temp.end() );
            
            iterable_V.insert( iterable_V.begin(), V_rev.begin(), V_rev.end() );
            iterable_V.insert( iterable_V.end(), iterable_V_temp.begin(), iterable_V_temp.end() );
            
        } else {

            iterable_t.insert( iterable_t.begin(), iterable_t_temp.begin(), iterable_t_temp.end() );
            iterable_V.insert( iterable_V.begin(), iterable_V_temp.begin(), iterable_V_temp.end() );
        
        }

        myFile.close();

}




} //namespace tfboost
    
    
#endif
