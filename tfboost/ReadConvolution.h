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

    inline void PushBack_helper(std::vector<double>& data) {}

    template<typename Iterable, typename ...Iterables>
    inline void PushBack_helper(std::vector<double>& data, Iterable& first, Iterables&... args)
    {
        first.push_back( data.back() );
        if(!data.empty()) data.pop_back();
        
        PushBack_helper(data, args...);
    }


    template<typename ...Iterables, size_t N> 
    inline typename std::enable_if< N==sizeof...(Iterables) && 
                             tfboost::detail::all_true<std::is_floating_point<typename Iterables::value_type>::value...>::value, void>::type
    PushBackTokens(TString const& line,
                   TString const& token, 
                   std::array<int,N> const& columns, 
                   Iterables&... args)
    {
        TObjArray *tokens = line.Tokenize( token.Data() );
        
        size_t Ncolumns = tokens->GetEntriesFast();
        
        SAFE_EXIT(Ncolumns < N, "No sufficient columns in file. Exit.")
        
        for(auto k : columns) { 
            SAFE_EXIT(Ncolumns < (size_t)k, "Requested to read a column not present in file. Exit.") }
        
        std::vector<double> data(N);
        
        for(size_t i=0; i<N; ++i){
            int col = columns[i];
            TString data_str  = ((TObjString*) tokens->At( col ) )->GetString();
            data[i] = atof(data_str); }
        
        std::reverse(data.begin(), data.end() );

        PushBack_helper(data, args...);
        
        tokens->Delete();
        delete tokens;
    }
    
    
    
    template<typename Iterable> 
    inline typename std::enable_if< std::is_floating_point<typename Iterable::value_type>::value, void>::type
    PushBackTokens(TString const& line,
                   TString const& token, 
                   int const& column, 
                   Iterable& arg)
    {
        TObjArray *tokens = line.Tokenize( token.Data() );
        
        size_t Ncolumns = tokens->GetEntriesFast();
        
        SAFE_EXIT(Ncolumns < 1, "No sufficient columns in file. Exit.")
        SAFE_EXIT(Ncolumns < (size_t)column, "Requested to read a column not present in file. Exit.") 
        
        TString data_str  = ((TObjString*) tokens->At( column ) )->GetString();
        double data = atof(data_str); 
        
        arg.push_back(data);
        
        tokens->Delete();
        delete tokens;
    }
    
} //end detail






template<typename Iterable>
void ReadConvolution(TString const& file, Iterable& iterable, size_t const& Nsamples)
{
        //size_t Nsamples = iterable.size();
        iterable.reserve(Nsamples);
        
        TString line;
        std::ifstream myFile( file.Data() );
        
        SAFE_EXIT(!myFile.is_open(), "Input file cannot be open.")
                
        line.ReadLine(myFile);
        
        while(myFile.good()) {
            detail::PushBackTokens(line, " ", 1, iterable);
            line.ReadLine(myFile); }
        
        myFile.close();
        
        size_t diff = Nsamples - iterable.size();
        
        if(diff>0) 
          for(size_t j = iterable.size(); j < Nsamples; ++j)
            iterable.push_back(0.0);
}





template<typename Iterable>
void ReadTF(TString const& file, 
            int Nlinestoskip, 
            Iterable& iterable_t, 
            Iterable& iterable_V, 
            double const& scale = 1.0, 
            bool doublerange=false)
{
        Iterable iterable_t_temp, iterable_V_temp;
        std::array<int,2> columns = {0,1};

        TString line;
        std::ifstream myFile( file.Data() );
        
        SAFE_EXIT(!myFile.is_open(), "In ReadTF(): Input file cannot be open.")
        
        for(size_t i=0; i<(size_t)Nlinestoskip+1; ++i) line.ReadLine(myFile);
        
        while(myFile.good()) {
            detail::PushBackTokens(line, " ", columns, iterable_t_temp, iterable_V_temp);
            line.ReadLine(myFile); }
            
        myFile.close();
        
        if(scale!=1.0) 
            for(auto& x : iterable_V_temp) x *= scale;

        double dT = iterable_t_temp[1] - iterable_t_temp[0];

        
        if(doublerange){
        
            double last_t = iterable_t_temp.back();
            
            // a small buffer befor doubling the range
            // in order to help the future spline
            for(size_t i=0; i<100; ++i ){
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




} //namespace tfboost
    
    
#endif
