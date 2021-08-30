void JitterCFD()
{
//=========Macro generated from canvas: JitterCFD/JitterCFD
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *JitterCFD = new TCanvas("JitterCFD", "JitterCFD",0,0,800,800);
   JitterCFD->SetHighLightColor(2);
   JitterCFD->Range(0,0,1,1);
   JitterCFD->SetFillColor(0);
   JitterCFD->SetBorderMode(0);
   JitterCFD->SetBorderSize(2);
   JitterCFD->SetFrameBorderMode(0);
   
   TH1D *hist_JitterCFD__14 = new TH1D("hist_JitterCFD__14","hist_JitterCFD",2000,-1e-09,1e-09);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_JitterCFD__14->SetLineColor(ci);
   hist_JitterCFD__14->SetLineWidth(2);
   hist_JitterCFD__14->GetXaxis()->SetTitle("Time[s]");
   hist_JitterCFD__14->GetXaxis()->SetLabelFont(42);
   hist_JitterCFD__14->GetXaxis()->SetTitleOffset(1);
   hist_JitterCFD__14->GetXaxis()->SetTitleFont(42);
   hist_JitterCFD__14->GetYaxis()->SetTitle("Counts");
   hist_JitterCFD__14->GetYaxis()->SetLabelFont(42);
   hist_JitterCFD__14->GetYaxis()->SetLabelSize(0.03);
   hist_JitterCFD__14->GetYaxis()->SetTitleOffset(1.5);
   hist_JitterCFD__14->GetYaxis()->SetTitleFont(42);
   hist_JitterCFD__14->GetZaxis()->SetLabelFont(42);
   hist_JitterCFD__14->GetZaxis()->SetTitleOffset(1);
   hist_JitterCFD__14->GetZaxis()->SetTitleFont(42);
   hist_JitterCFD__14->Draw("");
   JitterCFD->Modified();
   JitterCFD->cd();
   JitterCFD->SetSelected(JitterCFD);
}
