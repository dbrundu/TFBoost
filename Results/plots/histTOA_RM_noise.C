void histTOA_RM_noise()
{
//=========Macro generated from canvas: histTOA_RM_noise/histTOA_RM_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histTOA_RM_noise = new TCanvas("histTOA_RM_noise", "histTOA_RM_noise",0,0,800,800);
   histTOA_RM_noise->SetHighLightColor(2);
   histTOA_RM_noise->Range(0,0,1,1);
   histTOA_RM_noise->SetFillColor(0);
   histTOA_RM_noise->SetBorderMode(0);
   histTOA_RM_noise->SetBorderSize(2);
   histTOA_RM_noise->SetFrameBorderMode(0);
   
   TH1D *histTOA_RM_noise__19 = new TH1D("histTOA_RM_noise__19","Time of arrival Ref.Method with noise",2000,0,-1);
   histTOA_RM_noise__19->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_RM_noise__19->SetLineColor(ci);
   histTOA_RM_noise__19->GetXaxis()->SetTitle("Time (s)");
   histTOA_RM_noise__19->GetXaxis()->SetRange(1,1);
   histTOA_RM_noise__19->GetXaxis()->SetLabelFont(42);
   histTOA_RM_noise__19->GetXaxis()->SetTitleOffset(1);
   histTOA_RM_noise__19->GetXaxis()->SetTitleFont(42);
   histTOA_RM_noise__19->GetYaxis()->SetTitle("counts");
   histTOA_RM_noise__19->GetYaxis()->SetLabelFont(42);
   histTOA_RM_noise__19->GetYaxis()->SetTitleFont(42);
   histTOA_RM_noise__19->GetZaxis()->SetLabelFont(42);
   histTOA_RM_noise__19->GetZaxis()->SetTitleOffset(1);
   histTOA_RM_noise__19->GetZaxis()->SetTitleFont(42);
   histTOA_RM_noise__19->Draw("");
   histTOA_RM_noise->Modified();
   histTOA_RM_noise->cd();
   histTOA_RM_noise->SetSelected(histTOA_RM_noise);
}
