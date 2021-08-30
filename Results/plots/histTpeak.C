void histTpeak()
{
//=========Macro generated from canvas: histTpeak/histTpeak
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histTpeak = new TCanvas("histTpeak", "histTpeak",0,0,800,800);
   histTpeak->SetHighLightColor(2);
   histTpeak->Range(0,0,1,1);
   histTpeak->SetFillColor(0);
   histTpeak->SetBorderMode(0);
   histTpeak->SetBorderSize(2);
   histTpeak->SetFrameBorderMode(0);
   
   TH1D *histTpeak__4 = new TH1D("histTpeak__4","Time at peak",2000,-1.2,1.2);
   histTpeak__4->SetBinContent(1001,1);
   histTpeak__4->SetEntries(1);
   histTpeak__4->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTpeak__4->SetLineColor(ci);
   histTpeak__4->GetXaxis()->SetTitle("Time (s)");
   histTpeak__4->GetXaxis()->SetRange(1,2000);
   histTpeak__4->GetXaxis()->SetLabelFont(42);
   histTpeak__4->GetXaxis()->SetTitleOffset(1);
   histTpeak__4->GetXaxis()->SetTitleFont(42);
   histTpeak__4->GetYaxis()->SetTitle("counts");
   histTpeak__4->GetYaxis()->SetLabelFont(42);
   histTpeak__4->GetYaxis()->SetTitleFont(42);
   histTpeak__4->GetZaxis()->SetLabelFont(42);
   histTpeak__4->GetZaxis()->SetTitleOffset(1);
   histTpeak__4->GetZaxis()->SetTitleFont(42);
   histTpeak__4->Draw("");
   histTpeak->Modified();
   histTpeak->cd();
   histTpeak->SetSelected(histTpeak);
}
