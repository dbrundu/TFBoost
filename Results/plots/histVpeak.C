void histVpeak()
{
//=========Macro generated from canvas: histVpeak/histVpeak
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histVpeak = new TCanvas("histVpeak", "histVpeak",0,0,800,800);
   histVpeak->SetHighLightColor(2);
   histVpeak->Range(0,0,1,1);
   histVpeak->SetFillColor(0);
   histVpeak->SetBorderMode(0);
   histVpeak->SetBorderSize(2);
   histVpeak->SetFrameBorderMode(0);
   
   TH1D *histVpeak__5 = new TH1D("histVpeak__5","V at peak",2000,-1.198,1.2);
   histVpeak__5->SetBinContent(1000,1);
   histVpeak__5->SetEntries(1);
   histVpeak__5->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVpeak__5->SetLineColor(ci);
   histVpeak__5->GetXaxis()->SetTitle("Voltage (V)");
   histVpeak__5->GetXaxis()->SetRange(1,2000);
   histVpeak__5->GetXaxis()->SetLabelFont(42);
   histVpeak__5->GetXaxis()->SetTitleOffset(1);
   histVpeak__5->GetXaxis()->SetTitleFont(42);
   histVpeak__5->GetYaxis()->SetTitle("counts");
   histVpeak__5->GetYaxis()->SetLabelFont(42);
   histVpeak__5->GetYaxis()->SetTitleFont(42);
   histVpeak__5->GetZaxis()->SetLabelFont(42);
   histVpeak__5->GetZaxis()->SetTitleOffset(1);
   histVpeak__5->GetZaxis()->SetTitleFont(42);
   histVpeak__5->Draw("");
   histVpeak->Modified();
   histVpeak->cd();
   histVpeak->SetSelected(histVpeak);
}
