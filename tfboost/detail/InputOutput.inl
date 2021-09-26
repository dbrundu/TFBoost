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
 
#ifndef TFBOOST_READ_CONVOLUTION_INL_
#define TFBOOST_READ_CONVOLUTION_INL_

    
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


} //namespace tfboost
    
    
#endif
