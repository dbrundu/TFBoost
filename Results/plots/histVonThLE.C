void histVonThLE()
{
//=========Macro generated from canvas: histVonThLE/histVonThLE
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histVonThLE = new TCanvas("histVonThLE", "histVonThLE",0,0,800,800);
   histVonThLE->SetHighLightColor(2);
   histVonThLE->Range(0,0,1,1);
   histVonThLE->SetFillColor(0);
   histVonThLE->SetBorderMode(0);
   histVonThLE->SetBorderSize(2);
   histVonThLE->SetFrameBorderMode(0);
   
   TH1D *histVonThLE__9 = new TH1D("histVonThLE__9","V on LE threshold",2000,-1.2,1.2);
   histVonThLE__9->SetBinContent(1001,1);
   histVonThLE__9->SetEntries(1);
   histVonThLE__9->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThLE__9->SetLineColor(ci);
   histVonThLE__9->GetXaxis()->SetTitle("Voltage (V)");
   histVonThLE__9->GetXaxis()->SetRange(1,2000);
   histVonThLE__9->GetXaxis()->SetLabelFont(42);
   histVonThLE__9->GetXaxis()->SetTitleOffset(1);
   histVonThLE__9->GetXaxis()->SetTitleFont(42);
   histVonThLE__9->GetYaxis()->SetTitle("counts");
   histVonThLE__9->GetYaxis()->SetLabelFont(42);
   histVonThLE__9->GetYaxis()->SetTitleFont(42);
   histVonThLE__9->GetZaxis()->SetLabelFont(42);
   histVonThLE__9->GetZaxis()->SetTitleOffset(1);
   histVonThLE__9->GetZaxis()->SetTitleFont(42);
   histVonThLE__9->Draw("");
   histVonThLE->Modified();
   histVonThLE->cd();
   histVonThLE->SetSelected(histVonThLE);
}
