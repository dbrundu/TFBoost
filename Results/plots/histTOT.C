void histTOT()
{
//=========Macro generated from canvas: histTOT/histTOT
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histTOT = new TCanvas("histTOT", "histTOT",0,0,800,800);
   histTOT->SetHighLightColor(2);
   histTOT->Range(0,0,1,1);
   histTOT->SetFillColor(0);
   histTOT->SetBorderMode(0);
   histTOT->SetBorderSize(2);
   histTOT->SetFrameBorderMode(0);
   
   TH1D *histTOT__12 = new TH1D("histTOT__12","Time over threshold",2000,16382.8,16385.2);
   histTOT__12->SetBinContent(1000,1);
   histTOT__12->SetEntries(1);
   histTOT__12->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOT__12->SetLineColor(ci);
   histTOT__12->GetXaxis()->SetTitle("Time (s)");
   histTOT__12->GetXaxis()->SetRange(1,2000);
   histTOT__12->GetXaxis()->SetLabelFont(42);
   histTOT__12->GetXaxis()->SetTitleOffset(1);
   histTOT__12->GetXaxis()->SetTitleFont(42);
   histTOT__12->GetYaxis()->SetTitle("counts");
   histTOT__12->GetYaxis()->SetLabelFont(42);
   histTOT__12->GetYaxis()->SetTitleFont(42);
   histTOT__12->GetZaxis()->SetLabelFont(42);
   histTOT__12->GetZaxis()->SetTitleOffset(1);
   histTOT__12->GetZaxis()->SetTitleFont(42);
   histTOT__12->Draw("");
   histTOT->Modified();
   histTOT->cd();
   histTOT->SetSelected(histTOT);
}
