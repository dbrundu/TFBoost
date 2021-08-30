void histdvdt_CFD_noise()
{
//=========Macro generated from canvas: histdvdt_CFD_noise/histdvdt_CFD_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_CFD_noise = new TCanvas("histdvdt_CFD_noise", "histdvdt_CFD_noise",0,0,800,800);
   histdvdt_CFD_noise->SetHighLightColor(2);
   histdvdt_CFD_noise->Range(0,0,1,1);
   histdvdt_CFD_noise->SetFillColor(0);
   histdvdt_CFD_noise->SetBorderMode(0);
   histdvdt_CFD_noise->SetBorderSize(2);
   histdvdt_CFD_noise->SetFrameBorderMode(0);
   
   TH1D *histdvdt_CFD_noise__23 = new TH1D("histdvdt_CFD_noise__23","Slope (dV/dT) on CFD threshold with noise",2000,0,-1);
   histdvdt_CFD_noise__23->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_CFD_noise__23->SetLineColor(ci);
   histdvdt_CFD_noise__23->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_CFD_noise__23->GetXaxis()->SetRange(1,1);
   histdvdt_CFD_noise__23->GetXaxis()->SetLabelFont(42);
   histdvdt_CFD_noise__23->GetXaxis()->SetTitleOffset(1);
   histdvdt_CFD_noise__23->GetXaxis()->SetTitleFont(42);
   histdvdt_CFD_noise__23->GetYaxis()->SetTitle("counts");
   histdvdt_CFD_noise__23->GetYaxis()->SetLabelFont(42);
   histdvdt_CFD_noise__23->GetYaxis()->SetTitleFont(42);
   histdvdt_CFD_noise__23->GetZaxis()->SetLabelFont(42);
   histdvdt_CFD_noise__23->GetZaxis()->SetTitleOffset(1);
   histdvdt_CFD_noise__23->GetZaxis()->SetTitleFont(42);
   histdvdt_CFD_noise__23->Draw("");
   histdvdt_CFD_noise->Modified();
   histdvdt_CFD_noise->cd();
   histdvdt_CFD_noise->SetSelected(histdvdt_CFD_noise);
}
