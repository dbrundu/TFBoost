void histTOA_CFD()
{
//=========Macro generated from canvas: histTOA_CFD/histTOA_CFD
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histTOA_CFD = new TCanvas("histTOA_CFD", "histTOA_CFD",0,0,800,800);
   histTOA_CFD->SetHighLightColor(2);
   histTOA_CFD->Range(0,0,1,1);
   histTOA_CFD->SetFillColor(0);
   histTOA_CFD->SetBorderMode(0);
   histTOA_CFD->SetBorderSize(2);
   histTOA_CFD->SetFrameBorderMode(0);
   
   TH1D *histTOA_CFD__2 = new TH1D("histTOA_CFD__2","Time of arrival CFD",2000,-1.2,1.2);
   histTOA_CFD__2->SetBinContent(1001,1);
   histTOA_CFD__2->SetEntries(1);
   histTOA_CFD__2->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_CFD__2->SetLineColor(ci);
   histTOA_CFD__2->GetXaxis()->SetTitle("Time (s)");
   histTOA_CFD__2->GetXaxis()->SetRange(1,2000);
   histTOA_CFD__2->GetXaxis()->SetLabelFont(42);
   histTOA_CFD__2->GetXaxis()->SetTitleOffset(1);
   histTOA_CFD__2->GetXaxis()->SetTitleFont(42);
   histTOA_CFD__2->GetYaxis()->SetTitle("counts");
   histTOA_CFD__2->GetYaxis()->SetLabelFont(42);
   histTOA_CFD__2->GetYaxis()->SetTitleFont(42);
   histTOA_CFD__2->GetZaxis()->SetLabelFont(42);
   histTOA_CFD__2->GetZaxis()->SetTitleOffset(1);
   histTOA_CFD__2->GetZaxis()->SetTitleFont(42);
   histTOA_CFD__2->Draw("");
   histTOA_CFD->Modified();
   histTOA_CFD->cd();
   histTOA_CFD->SetSelected(histTOA_CFD);
}
