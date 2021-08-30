void histdvdt_RM_noise()
{
//=========Macro generated from canvas: histdvdt_RM_noise/histdvdt_RM_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_RM_noise = new TCanvas("histdvdt_RM_noise", "histdvdt_RM_noise",0,0,800,800);
   histdvdt_RM_noise->SetHighLightColor(2);
   histdvdt_RM_noise->Range(0,0,1,1);
   histdvdt_RM_noise->SetFillColor(0);
   histdvdt_RM_noise->SetBorderMode(0);
   histdvdt_RM_noise->SetBorderSize(2);
   histdvdt_RM_noise->SetFrameBorderMode(0);
   
   TH1D *histdvdt_RM_noise__24 = new TH1D("histdvdt_RM_noise__24","Slope (dV/dT) on RM threshold with noise",2000,0,-1);
   histdvdt_RM_noise__24->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_RM_noise__24->SetLineColor(ci);
   histdvdt_RM_noise__24->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_RM_noise__24->GetXaxis()->SetRange(1,1);
   histdvdt_RM_noise__24->GetXaxis()->SetLabelFont(42);
   histdvdt_RM_noise__24->GetXaxis()->SetTitleOffset(1);
   histdvdt_RM_noise__24->GetXaxis()->SetTitleFont(42);
   histdvdt_RM_noise__24->GetYaxis()->SetTitle("counts");
   histdvdt_RM_noise__24->GetYaxis()->SetLabelFont(42);
   histdvdt_RM_noise__24->GetYaxis()->SetTitleFont(42);
   histdvdt_RM_noise__24->GetZaxis()->SetLabelFont(42);
   histdvdt_RM_noise__24->GetZaxis()->SetTitleOffset(1);
   histdvdt_RM_noise__24->GetZaxis()->SetTitleFont(42);
   histdvdt_RM_noise__24->Draw("");
   histdvdt_RM_noise->Modified();
   histdvdt_RM_noise->cd();
   histdvdt_RM_noise->SetSelected(histdvdt_RM_noise);
}
