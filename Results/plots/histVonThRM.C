void histVonThRM()
{
//=========Macro generated from canvas: histVonThRM/histVonThRM
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histVonThRM = new TCanvas("histVonThRM", "histVonThRM",0,0,800,800);
   histVonThRM->SetHighLightColor(2);
   histVonThRM->Range(0,0,1,1);
   histVonThRM->SetFillColor(0);
   histVonThRM->SetBorderMode(0);
   histVonThRM->SetBorderSize(2);
   histVonThRM->SetFrameBorderMode(0);
   
   TH1D *histVonThRM__11 = new TH1D("histVonThRM__11","V on RM threshold",2000,-1.198,1.2);
   histVonThRM__11->SetBinContent(1000,1);
   histVonThRM__11->SetEntries(1);
   histVonThRM__11->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThRM__11->SetLineColor(ci);
   histVonThRM__11->GetXaxis()->SetTitle("Voltage (V)");
   histVonThRM__11->GetXaxis()->SetRange(1,2000);
   histVonThRM__11->GetXaxis()->SetLabelFont(42);
   histVonThRM__11->GetXaxis()->SetTitleOffset(1);
   histVonThRM__11->GetXaxis()->SetTitleFont(42);
   histVonThRM__11->GetYaxis()->SetTitle("counts");
   histVonThRM__11->GetYaxis()->SetLabelFont(42);
   histVonThRM__11->GetYaxis()->SetTitleFont(42);
   histVonThRM__11->GetZaxis()->SetLabelFont(42);
   histVonThRM__11->GetZaxis()->SetTitleOffset(1);
   histVonThRM__11->GetZaxis()->SetTitleFont(42);
   histVonThRM__11->Draw("");
   histVonThRM->Modified();
   histVonThRM->cd();
   histVonThRM->SetSelected(histVonThRM);
}
