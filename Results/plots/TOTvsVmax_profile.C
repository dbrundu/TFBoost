void TOTvsVmax_profile()
{
//=========Macro generated from canvas: TOTvsVmax_profile/TOTvsVmax_profile
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *TOTvsVmax_profile = new TCanvas("TOTvsVmax_profile", "TOTvsVmax_profile",0,0,800,800);
   TOTvsVmax_profile->SetHighLightColor(2);
   TOTvsVmax_profile->Range(0,0,1,1);
   TOTvsVmax_profile->SetFillColor(0);
   TOTvsVmax_profile->SetBorderMode(0);
   TOTvsVmax_profile->SetBorderSize(2);
   TOTvsVmax_profile->SetFrameBorderMode(0);
   
   TProfile *hist_TOTvsVmax_pfx = new TProfile("hist_TOTvsVmax_pfx","hist_TOTvsVmax",100,16382.8,16385.2,"");
   hist_TOTvsVmax_pfx->SetBinEntries(50,1);
   hist_TOTvsVmax_pfx->SetBinContent(50,-0.01025);
   hist_TOTvsVmax_pfx->SetBinError(50,0.01025);
   hist_TOTvsVmax_pfx->SetEntries(1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_TOTvsVmax_pfx->SetLineColor(ci);
   hist_TOTvsVmax_pfx->SetLineWidth(2);
   hist_TOTvsVmax_pfx->GetXaxis()->SetTitle("TOT [s]");
   hist_TOTvsVmax_pfx->GetXaxis()->SetLabelFont(42);
   hist_TOTvsVmax_pfx->GetXaxis()->SetTitleOffset(1);
   hist_TOTvsVmax_pfx->GetXaxis()->SetTitleFont(42);
   hist_TOTvsVmax_pfx->GetYaxis()->SetTitle("Vmax [V]");
   hist_TOTvsVmax_pfx->GetYaxis()->SetLabelFont(42);
   hist_TOTvsVmax_pfx->GetYaxis()->SetLabelSize(0.03);
   hist_TOTvsVmax_pfx->GetYaxis()->SetTitleOffset(1.5);
   hist_TOTvsVmax_pfx->GetYaxis()->SetTitleFont(42);
   hist_TOTvsVmax_pfx->GetZaxis()->SetLabelFont(42);
   hist_TOTvsVmax_pfx->GetZaxis()->SetTitleOffset(1);
   hist_TOTvsVmax_pfx->GetZaxis()->SetTitleFont(42);
   hist_TOTvsVmax_pfx->Draw("");
   TOTvsVmax_profile->Modified();
   TOTvsVmax_profile->cd();
   TOTvsVmax_profile->SetSelected(TOTvsVmax_profile);
}
