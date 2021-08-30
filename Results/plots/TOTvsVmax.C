void TOTvsVmax()
{
//=========Macro generated from canvas: TOTvsVmax/TOTvsVmax
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *TOTvsVmax = new TCanvas("TOTvsVmax", "TOTvsVmax",0,0,800,800);
   TOTvsVmax->SetHighLightColor(2);
   TOTvsVmax->Range(0,0,1,1);
   TOTvsVmax->SetFillColor(0);
   TOTvsVmax->SetBorderMode(0);
   TOTvsVmax->SetBorderSize(2);
   TOTvsVmax->SetFrameBorderMode(0);
   
   TH2D *hist_TOTvsVmax = new TH2D("hist_TOTvsVmax","hist_TOTvsVmax",100,16382.8,16385.2,100,-1.15,1.2);
   hist_TOTvsVmax->SetBinContent(5048,1);
   hist_TOTvsVmax->SetEntries(1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_TOTvsVmax->SetLineColor(ci);
   hist_TOTvsVmax->SetLineWidth(2);
   hist_TOTvsVmax->GetXaxis()->SetTitle("TOT [s]");
   hist_TOTvsVmax->GetXaxis()->SetRange(1,100);
   hist_TOTvsVmax->GetXaxis()->SetLabelFont(42);
   hist_TOTvsVmax->GetXaxis()->SetTitleOffset(1);
   hist_TOTvsVmax->GetXaxis()->SetTitleFont(42);
   hist_TOTvsVmax->GetYaxis()->SetTitle("Vmax [V]");
   hist_TOTvsVmax->GetYaxis()->SetRange(1,100);
   hist_TOTvsVmax->GetYaxis()->SetLabelFont(42);
   hist_TOTvsVmax->GetYaxis()->SetLabelSize(0.03);
   hist_TOTvsVmax->GetYaxis()->SetTitleOffset(1.5);
   hist_TOTvsVmax->GetYaxis()->SetTitleFont(42);
   hist_TOTvsVmax->GetZaxis()->SetLabelFont(42);
   hist_TOTvsVmax->GetZaxis()->SetTitleOffset(1);
   hist_TOTvsVmax->GetZaxis()->SetTitleFont(42);
   hist_TOTvsVmax->Draw("colz");
   TOTvsVmax->Modified();
   TOTvsVmax->cd();
   TOTvsVmax->SetSelected(TOTvsVmax);
}
