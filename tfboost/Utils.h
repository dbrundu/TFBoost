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
 
#ifndef GENERAL_UTILS_H_
#define GENERAL_UTILS_H_
 
/*------------------------------------------------------/
 *  Definition for message formatting
 *-----------------------------------------------------*/

#define VERBOSE_LINE(flag, message)\
if(flag){\
    std::cout<< "\033[1;35mVerbose: \033[0m" << message << "\n";\
}

#define INFO_LINE(message)\
    std::cout<< "\033[1;34m[INFO]: \033[0m" << message << "\n";\

#define WARNING_LINE(message)\
    std::cout<< "\033[1;33mWarning: \033[0m" << message << "\n";\

#define DEBUG(message)\
    std::cout<< "\033[1;33mDEBUG: \033[0m" << message << "\n";\


#define RULE_LINE(message)\
    std::cout << " <==== " << message << " ====>" << "\n";\


#define WARNING_CHECK(x, message)\
    if(x) {\
        std::cout<< "\033[1;33mWarning: \033[0m" << message << "\n";\
    }

#define SAFE_EXIT(x, message)\
    if(x) {\
        std::cout<< "\033[1;33m[EXIT:] \033[0m" << message << std::endl;\
        std::exit(0);\
    }

#define PRINT_ELEMENTS(n, elements)\
    for( size_t i=0; i<n; i++ ) std::cout << elements[i] << "\n";\
    std::cout << "\n";
    
    
    
    
    
    
    
namespace tfboost {


void SaveCanvas(TString const& directory, TString const& title, TString const& xtitle, TString const& ytitle, TH1D& hist)
{
    TCanvas canv(title, title, 800,800);
    hist.SetLineWidth(2);
    hist.GetXaxis()->SetTitle(xtitle);
    hist.GetYaxis()->SetTitle(ytitle);
    hist.GetYaxis()->SetLabelSize(0.03);
    hist.GetYaxis()->SetTitleOffset(1.5);
    hist.Draw();
    canv.SaveAs( directory+title+TString(".pdf") );

}


TList* GetFileList(TString dirname_string)
{
    const char* dirname = dirname_string.Data();
    TSystemDirectory dir(dirname, dirname);

    return dir.GetListOfFiles();

}


template<typename Iterable>
void SaveConvToFile(Iterable data, double dT, TString filename )
{
   std::ofstream outdata( filename.Data() ); 

    if( !outdata ) { // file couldn't be opened
        std::cerr << "Error: file could not be opened" << std::endl;
        std::exit(1);
    }

    for (size_t i=0; i<data.size(); ++i) outdata << i*dT << " " << data[i] << std::endl;
    outdata.close();
 
    return;
}


void CreateDirectories(TString path)
{
    TObjArray *tokens = path.Tokenize("/");
    TIter next(tokens);
    TObjString* element;
    TString dir = "";
    while( element = (TObjString*) next() ) {
        dir =  dir + TString(element->GetString()) + "/";
        mkdir( dir.Data() ,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH);
    }
}

} //namespace tfboost
    
    
#endif
