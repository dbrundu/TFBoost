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
 
#ifndef TFBOOST_INPUTOUTPUT_H_
#define TFBOOST_INPUTOUTPUT_H_

    
#include <tfboost/Utils.h>
#include <tfboost/detail/Traits.h>
#include <tfboost/detail/InputOutput.inl>
    
    
namespace tfboost {

/*
 * Read data from a file with one
 * or multiple columns separated by spaces
 * and fill a container with data
 * corresponding to a specific column
 */
template<typename Iterable>
void ReadSimple(TString const& file, int column, Iterable& iterable, size_t const& Nsamples, double const& scale=1.0)
{
        //size_t Nsamples = iterable.size();
        iterable.reserve(Nsamples);
        
        TString line;
        std::ifstream myFile( file.Data() );
        
        SAFE_EXIT(!myFile.is_open(), "Input file cannot be open.")
                
        line.ReadLine(myFile);
        
        while(myFile.good()) {
            detail::PushBackTokens(line, " ", column, iterable);
            line.ReadLine(myFile); }
        
        myFile.close();
        
        for(size_t j = iterable.size(); j < Nsamples; ++j)
          iterable.push_back(0.0);
          
        for(auto& x : iterable) x *= scale;
}



/*
 * Read data from a file with one
 * or multiple columns separated by spaces
 * and fill a container with data
 * corresponding to a specific column
 */
template<typename ARRAY, typename IterableT, typename IterableV>
void ReadSimple(TString const& file, ARRAY const& columns, const char* token, IterableT& iterable_t, IterableV& iterable_v, int Nlinestoskip = 0, double scale=1.0)
{
        
        TString line;
        std::ifstream myFile( file.Data() );
        
        SAFE_EXIT(!myFile.is_open(), "Input file cannot be open.")
                
        line.ReadLine(myFile);
        
        while(myFile.good()) {
            detail::PushBackTokens(line, token, columns, iterable_t, iterable_v);
            line.ReadLine(myFile); }
        
        myFile.close();
        
        if(scale!=1.0)
            for(auto& x : iterable_v) x *= scale;
}




/*
 * Read a transfer function from a file.
 * The format must be two columns separated by spaces
 * containing time and the transfer function response.
 * Fill the two correponfing containers.
 */
template<typename Iterable>
void ReadTF(TString const& file, 
            int Nlinestoskip, 
            Iterable& iterable_t, 
            Iterable& iterable_V, 
            double const& tmax,
            double const& dT0,
            double const& scale = 1.0, 
            bool reset_timeoffset = false, 
            bool doublerange=false)
{
        Iterable iterable_t_temp, iterable_V_temp;
        std::array<int,2> columns = {0,1};

        TString line;
        std::ifstream myFile( file.Data() );
        
        SAFE_EXIT(!myFile.is_open(), "In ReadTF(): Input file cannot be open.")
        
        for(size_t i=0; i<(size_t)Nlinestoskip+2; ++i) line.ReadLine(myFile);
        
        while(myFile.good()) {
            detail::PushBackTokens(line, " ", columns, iterable_t_temp, iterable_V_temp);
            line.ReadLine(myFile); }
            
        myFile.close();
        
        if(scale!=1.0) 
            for(auto& x : iterable_V_temp) x *= scale;
            
        if(reset_timeoffset){
            double t0 = iterable_t_temp[0];
            for(auto& x : iterable_t_temp) x = x-t0; }

        while(iterable_t_temp.back()<tmax){
            iterable_t_temp.push_back(iterable_t_temp.back()+dT0);
            iterable_V_temp.push_back(0.0);
            }


        double dT = iterable_t_temp[1] - iterable_t_temp[0];

        if(doublerange){
        
            double last_t = iterable_t_temp.back();
            
            // a small buffer befor doubling the range
            // in order to help the future spline
            for(size_t i=1; i<100; ++i ){
                iterable_t_temp.push_back( last_t + i*dT );
                iterable_V_temp.push_back(0.0); } 
        
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
}



template<typename Iterable, typename Iterable_t>
void SaveConvToFile(Iterable const& data, Iterable_t const& time, double dT, TString filename )
{
   std::ofstream outdata( filename.Data() ); 

    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        std::exit(1); }

    auto it     = hydra_thrust::max_element( data.begin() , data.end() );
    size_t k    = it - data.begin();
    double vmax = data[k];

    for(size_t i=0; i<data.size(); ++i) 
        //outdata << time[i] << " " << data[i] << " " << vmax <<  std::endl;
        outdata << time[i] << " " << data[i] <<  std::endl;
    
    outdata.close();
}



} //namespace tfboost
    
    
#endif
