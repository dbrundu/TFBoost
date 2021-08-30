void histTOA_LE()
{
//=========Macro generated from canvas: histTOA_LE/histTOA_LE
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histTOA_LE = new TCanvas("histTOA_LE", "histTOA_LE",0,0,800,800);
   histTOA_LE->SetHighLightColor(2);
   histTOA_LE->Range(0,0,1,1);
   histTOA_LE->SetFillColor(0);
   histTOA_LE->SetBorderMode(0);
   histTOA_LE->SetBorderSize(2);
   histTOA_LE->SetFrameBorderMode(0);
   
   TH1D *histTOA_LE__1 = new TH1D("histTOA_LE__1","Time of arrival Leading Edge",2000,-1.2,1.2);
   histTOA_LE__1->SetBinContent(1001,1);
   histTOA_LE__1->SetEntries(1);
   histTOA_LE__1->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_LE__1->SetLineColor(ci);
   histTOA_LE__1->GetXaxis()->SetTitle("Time (s)");
   histTOA_LE__1->GetXaxis()->SetRange(1,2000);
   histTOA_LE__1->GetXaxis()->SetLabelFont(42);
   histTOA_LE__1->GetXaxis()->SetTitleOffset(1);
   histTOA_LE__1->GetXaxis()->SetTitleFont(42);
   histTOA_LE__1->GetYaxis()->SetTitle("counts");
   histTOA_LE__1->GetYaxis()->SetLabelFont(42);
   histTOA_LE__1->GetYaxis()->SetTitleFont(42);
   histTOA_LE__1->GetZaxis()->SetLabelFont(42);
   histTOA_LE__1->GetZaxis()->SetTitleOffset(1);
   histTOA_LE__1->GetZaxis()->SetTitleFont(42);
   histTOA_LE__1->Draw("");
   histTOA_LE->Modified();
   histTOA_LE->cd();
   histTOA_LE->SetSelected(histTOA_LE);
}
