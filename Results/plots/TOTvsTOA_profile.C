void TOTvsTOA_profile()
{
//=========Macro generated from canvas: TOTvsTOA_profile/TOTvsTOA_profile
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *TOTvsTOA_profile = new TCanvas("TOTvsTOA_profile", "TOTvsTOA_profile",0,0,800,800);
   TOTvsTOA_profile->SetHighLightColor(2);
   TOTvsTOA_profile->Range(0,0,1,1);
   TOTvsTOA_profile->SetFillColor(0);
   TOTvsTOA_profile->SetBorderMode(0);
   TOTvsTOA_profile->SetBorderSize(2);
   TOTvsTOA_profile->SetFrameBorderMode(0);
   
   TProfile *hist_TOTvsTOA_pfx = new TProfile("hist_TOTvsTOA_pfx","TOTvsTOA",100,16382.8,16385.2,"");
   hist_TOTvsTOA_pfx->SetBinEntries(50,1);
   hist_TOTvsTOA_pfx->SetBinContent(50,0.012);
   hist_TOTvsTOA_pfx->SetBinError(50,0.012);
   hist_TOTvsTOA_pfx->SetEntries(1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_TOTvsTOA_pfx->SetLineColor(ci);
   hist_TOTvsTOA_pfx->SetLineWidth(2);
   hist_TOTvsTOA_pfx->GetXaxis()->SetTitle("TOT [s]");
   hist_TOTvsTOA_pfx->GetXaxis()->SetLabelFont(42);
   hist_TOTvsTOA_pfx->GetXaxis()->SetTitleOffset(1);
   hist_TOTvsTOA_pfx->GetXaxis()->SetTitleFont(42);
   hist_TOTvsTOA_pfx->GetYaxis()->SetTitle("Vmax [V]");
   hist_TOTvsTOA_pfx->GetYaxis()->SetLabelFont(42);
   hist_TOTvsTOA_pfx->GetYaxis()->SetLabelSize(0.03);
   hist_TOTvsTOA_pfx->GetYaxis()->SetTitleOffset(1.5);
   hist_TOTvsTOA_pfx->GetYaxis()->SetTitleFont(42);
   hist_TOTvsTOA_pfx->GetZaxis()->SetLabelFont(42);
   hist_TOTvsTOA_pfx->GetZaxis()->SetTitleOffset(1);
   hist_TOTvsTOA_pfx->GetZaxis()->SetTitleFont(42);
   hist_TOTvsTOA_pfx->Draw("");
   TOTvsTOA_profile->Modified();
   TOTvsTOA_profile->cd();
   TOTvsTOA_profile->SetSelected(TOTvsTOA_profile);
}
