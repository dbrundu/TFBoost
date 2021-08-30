void histdvdt_RM()
{
//=========Macro generated from canvas: histdvdt_RM/histdvdt_RM
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_RM = new TCanvas("histdvdt_RM", "histdvdt_RM",0,0,800,800);
   histdvdt_RM->SetHighLightColor(2);
   histdvdt_RM->Range(0,0,1,1);
   histdvdt_RM->SetFillColor(0);
   histdvdt_RM->SetBorderMode(0);
   histdvdt_RM->SetBorderSize(2);
   histdvdt_RM->SetFrameBorderMode(0);
   
   TH1D *histdvdt_RM__8 = new TH1D("histdvdt_RM__8","Slope (dV/dT) on RM threshold",2000,-0.342,2.056);
   histdvdt_RM__8->SetBinContent(1001,1);
   histdvdt_RM__8->SetEntries(1);
   histdvdt_RM__8->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_RM__8->SetLineColor(ci);
   histdvdt_RM__8->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_RM__8->GetXaxis()->SetRange(1,2000);
   histdvdt_RM__8->GetXaxis()->SetLabelFont(42);
   histdvdt_RM__8->GetXaxis()->SetTitleOffset(1);
   histdvdt_RM__8->GetXaxis()->SetTitleFont(42);
   histdvdt_RM__8->GetYaxis()->SetTitle("counts");
   histdvdt_RM__8->GetYaxis()->SetLabelFont(42);
   histdvdt_RM__8->GetYaxis()->SetTitleFont(42);
   histdvdt_RM__8->GetZaxis()->SetLabelFont(42);
   histdvdt_RM__8->GetZaxis()->SetTitleOffset(1);
   histdvdt_RM__8->GetZaxis()->SetTitleFont(42);
   histdvdt_RM__8->Draw("");
   histdvdt_RM->Modified();
   histdvdt_RM->cd();
   histdvdt_RM->SetSelected(histdvdt_RM);
}
