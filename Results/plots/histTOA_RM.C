void histTOA_RM()
{
//=========Macro generated from canvas: histTOA_RM/histTOA_RM
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histTOA_RM = new TCanvas("histTOA_RM", "histTOA_RM",0,0,800,800);
   histTOA_RM->SetHighLightColor(2);
   histTOA_RM->Range(0,0,1,1);
   histTOA_RM->SetFillColor(0);
   histTOA_RM->SetBorderMode(0);
   histTOA_RM->SetBorderSize(2);
   histTOA_RM->SetFrameBorderMode(0);
   
   TH1D *histTOA_RM__3 = new TH1D("histTOA_RM__3","Time of arrival Ref.Method",2000,-1.2,1.2);
   histTOA_RM__3->SetBinContent(1001,1);
   histTOA_RM__3->SetEntries(1);
   histTOA_RM__3->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_RM__3->SetLineColor(ci);
   histTOA_RM__3->GetXaxis()->SetTitle("Time (s)");
   histTOA_RM__3->GetXaxis()->SetRange(1,2000);
   histTOA_RM__3->GetXaxis()->SetLabelFont(42);
   histTOA_RM__3->GetXaxis()->SetTitleOffset(1);
   histTOA_RM__3->GetXaxis()->SetTitleFont(42);
   histTOA_RM__3->GetYaxis()->SetTitle("counts");
   histTOA_RM__3->GetYaxis()->SetLabelFont(42);
   histTOA_RM__3->GetYaxis()->SetTitleFont(42);
   histTOA_RM__3->GetZaxis()->SetLabelFont(42);
   histTOA_RM__3->GetZaxis()->SetTitleOffset(1);
   histTOA_RM__3->GetZaxis()->SetTitleFont(42);
   histTOA_RM__3->Draw("");
   histTOA_RM->Modified();
   histTOA_RM->cd();
   histTOA_RM->SetSelected(histTOA_RM);
}
