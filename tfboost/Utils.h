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
 
#ifndef TFBOOST_GENERAL_UTILS_H_
#define TFBOOST_GENERAL_UTILS_H_
 
/*------------------------------------------------------/
 *  Definition for message formatting
 *-----------------------------------------------------*/
 
#define _START_INFO_ "\033[1;34m"
#define _END_INFO_ "\033[0m"

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


#define RULE_LINE \
    std::cout << "===============================================" << "\n"\

#define RULE_LINE_LIGHT \
    std::cout << "-----------------------------------------------" << "\n"\

#define WARNING_CHECK(x, message)\
    if(x) {\
        std::cout<< "\033[1;33mWarning: \033[0m" << message << "\n";\
    }

#define SAFE_EXIT(x, message)\
    if(x) {\
        std::cout<< "\033[1;33m[EXIT:] \033[0m" << message << "\n";\
        std::exit(0);\
    }

#define PRINT_ELEMENTS(n, elements)\
    for( size_t i=0; i<n; i++ ) std::cout << elements[i] << "\n";\
    std::cout << "\n";
    
#define ERROR_RETURN(x, message, val)\
    if(x) {\
        std::cout<< "\033[1;33mWarning: \033[0m" << message << "\n";\
        return val;\
    }

    
    
    
namespace tfboost {


/*
 * Function to fill the signal, kernel or convolution
 * histograms
 */
template<typename HIST, typename FUNCTION>
inline void FillHistWithFunction(HIST& hist, FUNCTION const& fun)
{
    for(size_t i=1;  i < (size_t) hist.GetNbinsX(); ++i) 
        hist.SetBinContent(i, fun(hist.GetBinCenter(i)) );
}



/*
 * Function to save the canvas for monitoring graphs
 * passing the name of axes and title
 */
template<typename GRAPH>
inline void SaveMonitorGraph(TString const& title, 
                       TString const& xtitle, 
                       TString const& ytitle, 
                       GRAPH& gr)
{
    TCanvas canv(title, title, 800,800);
    gr.GetXaxis()->SetTitle(xtitle);
    gr.GetYaxis()->SetTitle(ytitle);
    gr.GetYaxis()->SetLabelSize(0.03);
    gr.GetYaxis()->SetTitleOffset(1.5);
    gr.Draw("APL");
    canv.SaveAs( "monitor/"+title+TString(".pdf") );
}




/*
 * Function to save a canvas 
 * passing the name of axes, title and 
 * drawing options
 */
template<typename HIST>
inline void SaveCanvas(TString const& directory, 
                       TString const& title, 
                       TString const& xtitle, 
                       TString const& ytitle, 
                       HIST& hist, 
                       const char* opt="", 
                       TF1* tf1ptr = nullptr)
{
    TCanvas canv(title, title, 800,800);
    hist.SetLineWidth(2);
    hist.GetXaxis()->SetTitle(xtitle);
    hist.GetYaxis()->SetTitle(ytitle);
    hist.GetYaxis()->SetLabelSize(0.03);
    hist.GetYaxis()->SetTitleOffset(1.5);
    hist.Draw(opt);
    if(tf1ptr) hist.Fit( tf1ptr , "R" );
    canv.SaveAs( directory+title+TString(".pdf") );
    canv.SaveAs( directory+title+TString(".C") );
}




/*
 * Function to save a canvas given
 * a fully configured histogram
 */
template<typename HIST>
inline void SaveCanvas(TString const& directory, 
                       HIST& hist)
{
    TString title = hist.GetName();
    TCanvas canv( title, title, 800,800);
    hist.Draw();
    canv.SaveAs( directory+title+TString(".pdf") );
    canv.SaveAs( directory+title+TString(".C") );
}



/*
 * Function to save the canvas
 * containing the signal, kernel 
 * and convolution curves
 */
inline void SaveConvolutionCanvas(TString const& directory, TString const& title, 
                          TH1D& hist_convol, TH1D& hist_signal, TH1D& hist_kernel)
{
    TCanvas canvas(title, title, 4000,1000);
    canvas.Divide(3,1);
    canvas.cd(1);
    
    hist_convol.SetStats(0);
    hist_convol.SetLineColor(4);
    hist_convol.SetLineWidth(1);
    hist_convol.GetXaxis()->SetTitle("sec");
    hist_convol.GetYaxis()->SetTitle("V");
    hist_convol.GetYaxis()->SetLabelSize(0.03);
    hist_convol.GetYaxis()->SetTitleOffset(1.5);
    hist_convol.Draw("L");

    canvas.cd(2);
    hist_signal.SetStats(0);
    hist_signal.SetLineColor(4);
    hist_signal.SetLineWidth(1);
    hist_signal.GetXaxis()->SetTitle("sec");
    hist_signal.GetYaxis()->SetTitle("A");
    hist_signal.GetYaxis()->SetLabelSize(0.03);
    hist_signal.GetYaxis()->SetTitleOffset(1.5);
    hist_signal.Draw("C");
            
    canvas.cd(3);
    hist_kernel.SetStats(0);
    hist_kernel.SetLineColor(4);
    hist_kernel.SetLineWidth(1);
    hist_kernel.GetXaxis()->SetTitle("sec");
    hist_kernel.GetYaxis()->SetLabelSize(0.03);
    hist_kernel.GetYaxis()->SetTitleOffset(1.5);
    hist_kernel.Draw("C");
    hist_convol.SetLineColor(1);
    hist_convol.Draw("same");

    canvas.SaveAs( directory+title+TString(".pdf") );

}


/*
 * Function to get a size_t
 * to the nearest upper power of 2
 */
inline size_t upper_power_of_two(size_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}


/*
 * Function to get a pointer
 * to a list of all the files given a path
 * NB: The list contains also other directories
 * and special directories as "./" and "../"
 */
TList* GetFileList(TString dirname_string)
{
    const char* dirname = dirname_string.Data();
    TSystemDirectory dir(dirname, dirname);

    if( !dir.GetListOfFiles() ) { // dir does not exist
        std::cerr << "Error: input dir does not exist" << std::endl;
        std::exit(1);
    }

    TList* list   = dir.GetListOfFiles();
    TIterator* it = list->MakeIterator();
    TSystemFile* currentfile; 

    while ( currentfile = (TSystemFile*) it->Next() ){
        if (currentfile->IsDirectory()) list->Remove(currentfile);
    }

    return list;
}


/*
 * Function to create recursively
 * all the directories given a path,
 * if they do not exist
 */
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



void TFBoostHeader()
{
    std::cout << "=====================================================" << "\n";
    std::cout << " TFBOOST, fast signals convolution and analyzer      " << "\n";
    std::cout << "  Developed by D.Brundu and G.M. Cossu               " << "\n";
    std::cout << "  https://github.com/dbrundu/TFBoost                 " << "\n";
    std::cout << "-----------------------------------------------------" << "\n";
    std::cout << "  Powered by HYDRA, developed by A.A.A Junior,       " << "\n";
    std::cout << "  https://github.com/MultithreadCorner               " << "\n";
    std::cout << "=====================================================" << "\n\n";
}


} //namespace tfboost
    
    
#endif
