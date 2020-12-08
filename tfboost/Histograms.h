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
 *  Created on: 22/11/2020
 *      Author: Davide Brundu
 */
 
#ifndef TFBOOST_HISTOGRAMS_H_
#define TFBOOST_HISTOGRAMS_H_

#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/ConfigParser.h>


namespace tfboost { 


/*
 * The class HistogramsManager is a singleton class
 * that allocate the memory for all the instograms
 * and provides an interface to HistConfigParser, 
 * to configure and format the histograms, and an interface
 * to the measurements, in order to fill the histograms.
 *
 */
class HistogramsManager { 

    public:
    
        HistogramsManager(HistogramsManager const&) = delete;
        
        void operator=(HistogramsManager const&)    = delete;
        
        static HistogramsManager& getInstance() {
            static HistogramsManager instance; 
            return instance;
        }
        
        
        
        inline void SetConfig( tfboost::HistConfigParser const& config_parser){
            
            for(auto const& key : MeasuresInitializer::get_keys() ) {
            
                _setconfig( histograms[key], config_parser.configs[key] );
                
                _setconfig( histograms_noise[key], config_parser.configs_noise[key] );
            }
        }
        
        
        
        inline void FillMeasures( Measures_t const& measures){
        
            for(auto const& key : MeasuresInitializer::get_keys() ) { 
                if(measures[key]>-1.0)
                    histograms[key] -> Fill ( measures[key] );
            }
        }
        
        
        
        inline void FillMeasures_noise( Measures_t const& measures_n){
        
            for(auto const& key : MeasuresInitializer::get_keys() ) { 
                if(measures_n[key]>-1.0)
                    histograms_noise[key] -> Fill ( measures_n[key] );
            }
        }
        
        
        
        inline void SaveHistograms(TString const& outputdir){
        
            for(auto const& key : MeasuresInitializer::get_keys() ) { 
                tfboost::SaveCanvas( outputdir, *histograms[key] );
            }
        }
        
        
        
        inline void SaveHistograms_noise( TString const& outputdir ){
        
            for(auto const& key : MeasuresInitializer::get_keys() ) { 
                tfboost::SaveCanvas( outputdir, *histograms_noise[key] );
            }
        }
        
    
    private:
    
        HistogramsManager() { 
            for(auto& hist : histograms) hist = new TH1D();
            for(auto& hist : histograms_noise) hist = new TH1D();
        } 
        
        ~HistogramsManager() { 
            for(auto& hist : histograms) delete hist;
            for(auto& hist : histograms_noise) delete hist;
        } 
        
        
        inline void _setconfig(TH1D* hist, HistConfig const& conf) const {
        
            hist->SetNameTitle (conf.name, conf.title);
            hist->SetBins( conf.Nbins, conf.min, conf.max);
            if(conf.max < conf.min) hist->SetBuffer(1000);
            hist->GetXaxis()->SetTitle(conf.xtitle);
            hist->GetYaxis()->SetTitle(conf.ytitle);
            hist->GetYaxis()->SetLabelSize(0.03);
            hist->GetYaxis()->SetTitleOffset(1.5);
            
        }
        
        Histograms_t histograms;
        Histograms_t histograms_noise;
        


};



} //namespace tfboost

#endif



