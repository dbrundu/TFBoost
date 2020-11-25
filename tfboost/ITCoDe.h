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
 *  Created on: 05/11/2020
 *      Author: Davide Brundu
 */
 
 
#ifndef INTERFACE_TCODE_H_
#define INTERFACE_TCODE_H_


#define TCODE_ENABLE false

#define TCODE_PIXEL_XMAX 56
#define TCODE_PIXEL_YMAX 56
 

    
namespace tfboost { 

namespace tcode {


    inline std::pair< int , int > GetHitPosition(TString const& filename)
    {

        size_t i = filename.First("_");
        size_t k = filename.First(".");

        TString str = (TString) filename(i+1, k-i-1);
        DEBUG(str)

        int pos  = atoi(str);

        int y    = std::floor(pos/56);
        int x    = pos % 56;

        return std::pair<int,int>(x,y); 

    }



    inline std::pair< double , double > GetHitPosition2(TString const& filename)
    {

        TObjArray *tokens = filename.Tokenize( "_" );
        
        TString x_str  = ((TObjString*) tokens->At( 1 ) )->GetString();
        TString y_str  = ((TObjString*) tokens->At( 2 ) )->GetString();
        
        y_str = y_str(0, y_str.Length()-4 );


        return std::pair<double,double>( atof(x_str) , atof(y_str) ); 

    }


}

} 
    
    
    
    
#endif //INTERFACE_TCODE_H_


