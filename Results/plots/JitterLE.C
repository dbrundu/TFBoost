void JitterLE()
{
//=========Macro generated from canvas: JitterLE/JitterLE
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *JitterLE = new TCanvas("JitterLE", "JitterLE",0,0,800,800);
   JitterLE->SetHighLightColor(2);
   JitterLE->Range(0,0,1,1);
   JitterLE->SetFillColor(0);
   JitterLE->SetBorderMode(0);
   JitterLE->SetBorderSize(2);
   JitterLE->SetFrameBorderMode(0);
   
   TH1D *hist_JitterLE__15 = new TH1D("hist_JitterLE__15","hist_JitterLE",2000,-1e-09,1e-09);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_JitterLE__15->SetLineColor(ci);
   hist_JitterLE__15->SetLineWidth(2);
   hist_JitterLE__15->GetXaxis()->SetTitle("Time[s]");
   hist_JitterLE__15->GetXaxis()->SetLabelFont(42);
   hist_JitterLE__15->GetXaxis()->SetTitleOffset(1);
   hist_JitterLE__15->GetXaxis()->SetTitleFont(42);
   hist_JitterLE__15->GetYaxis()->SetTitle("Counts");
   hist_JitterLE__15->GetYaxis()->SetLabelFont(42);
   hist_JitterLE__15->GetYaxis()->SetLabelSize(0.03);
   hist_JitterLE__15->GetYaxis()->SetTitleOffset(1.5);
   hist_JitterLE__15->GetYaxis()->SetTitleFont(42);
   hist_JitterLE__15->GetZaxis()->SetLabelFont(42);
   hist_JitterLE__15->GetZaxis()->SetTitleOffset(1);
   hist_JitterLE__15->GetZaxis()->SetTitleFont(42);
   hist_JitterLE__15->Draw("");
   JitterLE->Modified();
   JitterLE->cd();
   JitterLE->SetSelected(JitterLE);
}
