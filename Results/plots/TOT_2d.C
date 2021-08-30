void TOT_2d()
{
//=========Macro generated from canvas: TOT_2d/TOT_2d
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *TOT_2d = new TCanvas("TOT_2d", "TOT_2d",0,0,800,800);
   TOT_2d->SetHighLightColor(2);
   TOT_2d->Range(0,0,1,1);
   TOT_2d->SetFillColor(0);
   TOT_2d->SetBorderMode(0);
   TOT_2d->SetBorderSize(2);
   TOT_2d->SetFrameBorderMode(0);
   
   TH2D *hist_TOTvsTOA = new TH2D("hist_TOTvsTOA","TOTvsTOA",100,16382.8,16385.2,100,-1.2,1.2);
   hist_TOTvsTOA->SetBinContent(5252,1);
   hist_TOTvsTOA->SetEntries(1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_TOTvsTOA->SetLineColor(ci);
   hist_TOTvsTOA->SetLineWidth(2);
   hist_TOTvsTOA->GetXaxis()->SetTitle("Time [s]");
   hist_TOTvsTOA->GetXaxis()->SetRange(1,100);
   hist_TOTvsTOA->GetXaxis()->SetLabelFont(42);
   hist_TOTvsTOA->GetXaxis()->SetTitleOffset(1);
   hist_TOTvsTOA->GetXaxis()->SetTitleFont(42);
   hist_TOTvsTOA->GetYaxis()->SetTitle("TOA [s]");
   hist_TOTvsTOA->GetYaxis()->SetRange(1,100);
   hist_TOTvsTOA->GetYaxis()->SetLabelFont(42);
   hist_TOTvsTOA->GetYaxis()->SetLabelSize(0.03);
   hist_TOTvsTOA->GetYaxis()->SetTitleOffset(1.5);
   hist_TOTvsTOA->GetYaxis()->SetTitleFont(42);
   hist_TOTvsTOA->GetZaxis()->SetLabelFont(42);
   hist_TOTvsTOA->GetZaxis()->SetTitleOffset(1);
   hist_TOTvsTOA->GetZaxis()->SetTitleFont(42);
   hist_TOTvsTOA->Draw("colz");
   TOT_2d->Modified();
   TOT_2d->cd();
   TOT_2d->SetSelected(TOT_2d);
}
