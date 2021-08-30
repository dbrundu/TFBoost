void histdvdt_LE()
{
//=========Macro generated from canvas: histdvdt_LE/histdvdt_LE
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_LE = new TCanvas("histdvdt_LE", "histdvdt_LE",0,0,800,800);
   histdvdt_LE->SetHighLightColor(2);
   histdvdt_LE->Range(0,0,1,1);
   histdvdt_LE->SetFillColor(0);
   histdvdt_LE->SetBorderMode(0);
   histdvdt_LE->SetBorderSize(2);
   histdvdt_LE->SetFrameBorderMode(0);
   
   TH1D *histdvdt_LE__6 = new TH1D("histdvdt_LE__6","Slope (dV/dT) on LE threshold",2000,-1.2,1.2);
   histdvdt_LE__6->SetBinContent(1001,1);
   histdvdt_LE__6->SetEntries(1);
   histdvdt_LE__6->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_LE__6->SetLineColor(ci);
   histdvdt_LE__6->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_LE__6->GetXaxis()->SetRange(1,2000);
   histdvdt_LE__6->GetXaxis()->SetLabelFont(42);
   histdvdt_LE__6->GetXaxis()->SetTitleOffset(1);
   histdvdt_LE__6->GetXaxis()->SetTitleFont(42);
   histdvdt_LE__6->GetYaxis()->SetTitle("counts");
   histdvdt_LE__6->GetYaxis()->SetLabelFont(42);
   histdvdt_LE__6->GetYaxis()->SetTitleFont(42);
   histdvdt_LE__6->GetZaxis()->SetLabelFont(42);
   histdvdt_LE__6->GetZaxis()->SetTitleOffset(1);
   histdvdt_LE__6->GetZaxis()->SetTitleFont(42);
   histdvdt_LE__6->Draw("");
   histdvdt_LE->Modified();
   histdvdt_LE->cd();
   histdvdt_LE->SetSelected(histdvdt_LE);
}
