void histVpeak_noise()
{
//=========Macro generated from canvas: histVpeak_noise/histVpeak_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histVpeak_noise = new TCanvas("histVpeak_noise", "histVpeak_noise",0,0,800,800);
   histVpeak_noise->SetHighLightColor(2);
   histVpeak_noise->Range(0,0,1,1);
   histVpeak_noise->SetFillColor(0);
   histVpeak_noise->SetBorderMode(0);
   histVpeak_noise->SetBorderSize(2);
   histVpeak_noise->SetFrameBorderMode(0);
   
   TH1D *histVpeak_noise__21 = new TH1D("histVpeak_noise__21","V at peak with noise",2000,0,-1);
   histVpeak_noise__21->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVpeak_noise__21->SetLineColor(ci);
   histVpeak_noise__21->GetXaxis()->SetTitle("Voltage (V)");
   histVpeak_noise__21->GetXaxis()->SetRange(1,1);
   histVpeak_noise__21->GetXaxis()->SetLabelFont(42);
   histVpeak_noise__21->GetXaxis()->SetTitleOffset(1);
   histVpeak_noise__21->GetXaxis()->SetTitleFont(42);
   histVpeak_noise__21->GetYaxis()->SetTitle("counts");
   histVpeak_noise__21->GetYaxis()->SetLabelFont(42);
   histVpeak_noise__21->GetYaxis()->SetTitleFont(42);
   histVpeak_noise__21->GetZaxis()->SetLabelFont(42);
   histVpeak_noise__21->GetZaxis()->SetTitleOffset(1);
   histVpeak_noise__21->GetZaxis()->SetTitleFont(42);
   histVpeak_noise__21->Draw("");
   histVpeak_noise->Modified();
   histVpeak_noise->cd();
   histVpeak_noise->SetSelected(histVpeak_noise);
}
