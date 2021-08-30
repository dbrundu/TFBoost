void histTOA_CFD_noise()
{
//=========Macro generated from canvas: histTOA_CFD_noise/histTOA_CFD_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histTOA_CFD_noise = new TCanvas("histTOA_CFD_noise", "histTOA_CFD_noise",0,0,800,800);
   histTOA_CFD_noise->SetHighLightColor(2);
   histTOA_CFD_noise->Range(0,0,1,1);
   histTOA_CFD_noise->SetFillColor(0);
   histTOA_CFD_noise->SetBorderMode(0);
   histTOA_CFD_noise->SetBorderSize(2);
   histTOA_CFD_noise->SetFrameBorderMode(0);
   
   TH1D *histTOA_CFD_noise__18 = new TH1D("histTOA_CFD_noise__18","Time of arrival CFD with noise",2000,0,-1);
   histTOA_CFD_noise__18->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_CFD_noise__18->SetLineColor(ci);
   histTOA_CFD_noise__18->GetXaxis()->SetTitle("Time (s)");
   histTOA_CFD_noise__18->GetXaxis()->SetRange(1,1);
   histTOA_CFD_noise__18->GetXaxis()->SetLabelFont(42);
   histTOA_CFD_noise__18->GetXaxis()->SetTitleOffset(1);
   histTOA_CFD_noise__18->GetXaxis()->SetTitleFont(42);
   histTOA_CFD_noise__18->GetYaxis()->SetTitle("counts");
   histTOA_CFD_noise__18->GetYaxis()->SetLabelFont(42);
   histTOA_CFD_noise__18->GetYaxis()->SetTitleFont(42);
   histTOA_CFD_noise__18->GetZaxis()->SetLabelFont(42);
   histTOA_CFD_noise__18->GetZaxis()->SetTitleOffset(1);
   histTOA_CFD_noise__18->GetZaxis()->SetTitleFont(42);
   histTOA_CFD_noise__18->Draw("");
   histTOA_CFD_noise->Modified();
   histTOA_CFD_noise->cd();
   histTOA_CFD_noise->SetSelected(histTOA_CFD_noise);
}
