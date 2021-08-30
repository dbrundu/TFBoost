void JitterRM()
{
//=========Macro generated from canvas: JitterRM/JitterRM
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *JitterRM = new TCanvas("JitterRM", "JitterRM",0,0,800,800);
   JitterRM->SetHighLightColor(2);
   JitterRM->Range(0,0,1,1);
   JitterRM->SetFillColor(0);
   JitterRM->SetBorderMode(0);
   JitterRM->SetBorderSize(2);
   JitterRM->SetFrameBorderMode(0);
   
   TH1D *hist_JitterRM__16 = new TH1D("hist_JitterRM__16","hist_JitterRM",2000,-1e-09,1e-09);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_JitterRM__16->SetLineColor(ci);
   hist_JitterRM__16->SetLineWidth(2);
   hist_JitterRM__16->GetXaxis()->SetTitle("Time[s]");
   hist_JitterRM__16->GetXaxis()->SetLabelFont(42);
   hist_JitterRM__16->GetXaxis()->SetTitleOffset(1);
   hist_JitterRM__16->GetXaxis()->SetTitleFont(42);
   hist_JitterRM__16->GetYaxis()->SetTitle("Counts");
   hist_JitterRM__16->GetYaxis()->SetLabelFont(42);
   hist_JitterRM__16->GetYaxis()->SetLabelSize(0.03);
   hist_JitterRM__16->GetYaxis()->SetTitleOffset(1.5);
   hist_JitterRM__16->GetYaxis()->SetTitleFont(42);
   hist_JitterRM__16->GetZaxis()->SetLabelFont(42);
   hist_JitterRM__16->GetZaxis()->SetTitleOffset(1);
   hist_JitterRM__16->GetZaxis()->SetTitleFont(42);
   hist_JitterRM__16->Draw("");
   JitterRM->Modified();
   JitterRM->cd();
   JitterRM->SetSelected(JitterRM);
}
