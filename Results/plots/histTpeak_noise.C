void histTpeak_noise()
{
//=========Macro generated from canvas: histTpeak_noise/histTpeak_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histTpeak_noise = new TCanvas("histTpeak_noise", "histTpeak_noise",0,0,800,800);
   histTpeak_noise->SetHighLightColor(2);
   histTpeak_noise->Range(0,0,1,1);
   histTpeak_noise->SetFillColor(0);
   histTpeak_noise->SetBorderMode(0);
   histTpeak_noise->SetBorderSize(2);
   histTpeak_noise->SetFrameBorderMode(0);
   
   TH1D *histTpeak_noise__20 = new TH1D("histTpeak_noise__20","Time at peak with noise",2000,0,-1);
   histTpeak_noise__20->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTpeak_noise__20->SetLineColor(ci);
   histTpeak_noise__20->GetXaxis()->SetTitle("Time (s)");
   histTpeak_noise__20->GetXaxis()->SetRange(1,1);
   histTpeak_noise__20->GetXaxis()->SetLabelFont(42);
   histTpeak_noise__20->GetXaxis()->SetTitleOffset(1);
   histTpeak_noise__20->GetXaxis()->SetTitleFont(42);
   histTpeak_noise__20->GetYaxis()->SetTitle("counts");
   histTpeak_noise__20->GetYaxis()->SetLabelFont(42);
   histTpeak_noise__20->GetYaxis()->SetTitleFont(42);
   histTpeak_noise__20->GetZaxis()->SetLabelFont(42);
   histTpeak_noise__20->GetZaxis()->SetTitleOffset(1);
   histTpeak_noise__20->GetZaxis()->SetTitleFont(42);
   histTpeak_noise__20->Draw("");
   histTpeak_noise->Modified();
   histTpeak_noise->cd();
   histTpeak_noise->SetSelected(histTpeak_noise);
}
