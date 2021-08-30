void histdvdt_CFD()
{
//=========Macro generated from canvas: histdvdt_CFD/histdvdt_CFD
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_CFD = new TCanvas("histdvdt_CFD", "histdvdt_CFD",0,0,800,800);
   histdvdt_CFD->SetHighLightColor(2);
   histdvdt_CFD->Range(0,0,1,1);
   histdvdt_CFD->SetFillColor(0);
   histdvdt_CFD->SetBorderMode(0);
   histdvdt_CFD->SetBorderSize(2);
   histdvdt_CFD->SetFrameBorderMode(0);
   
   TH1D *histdvdt_CFD__7 = new TH1D("histdvdt_CFD__7","Slope (dV/dT) on CFD threshold",2000,0,2.27);
   histdvdt_CFD__7->SetBinContent(944,1);
   histdvdt_CFD__7->SetEntries(1);
   histdvdt_CFD__7->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_CFD__7->SetLineColor(ci);
   histdvdt_CFD__7->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_CFD__7->GetXaxis()->SetRange(1,2000);
   histdvdt_CFD__7->GetXaxis()->SetLabelFont(42);
   histdvdt_CFD__7->GetXaxis()->SetTitleOffset(1);
   histdvdt_CFD__7->GetXaxis()->SetTitleFont(42);
   histdvdt_CFD__7->GetYaxis()->SetTitle("counts");
   histdvdt_CFD__7->GetYaxis()->SetLabelFont(42);
   histdvdt_CFD__7->GetYaxis()->SetTitleFont(42);
   histdvdt_CFD__7->GetZaxis()->SetLabelFont(42);
   histdvdt_CFD__7->GetZaxis()->SetTitleOffset(1);
   histdvdt_CFD__7->GetZaxis()->SetTitleFont(42);
   histdvdt_CFD__7->Draw("");
   histdvdt_CFD->Modified();
   histdvdt_CFD->cd();
   histdvdt_CFD->SetSelected(histdvdt_CFD);
}
