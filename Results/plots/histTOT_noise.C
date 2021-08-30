void histTOT_noise()
{
//=========Macro generated from canvas: histTOT_noise/histTOT_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histTOT_noise = new TCanvas("histTOT_noise", "histTOT_noise",0,0,800,800);
   histTOT_noise->SetHighLightColor(2);
   histTOT_noise->Range(0,0,1,1);
   histTOT_noise->SetFillColor(0);
   histTOT_noise->SetBorderMode(0);
   histTOT_noise->SetBorderSize(2);
   histTOT_noise->SetFrameBorderMode(0);
   
   TH1D *histTOT_noise__28 = new TH1D("histTOT_noise__28","Time over threshold with noise",2000,0,-1);
   histTOT_noise__28->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOT_noise__28->SetLineColor(ci);
   histTOT_noise__28->GetXaxis()->SetTitle("Time (s)");
   histTOT_noise__28->GetXaxis()->SetRange(1,1);
   histTOT_noise__28->GetXaxis()->SetLabelFont(42);
   histTOT_noise__28->GetXaxis()->SetTitleOffset(1);
   histTOT_noise__28->GetXaxis()->SetTitleFont(42);
   histTOT_noise__28->GetYaxis()->SetTitle("counts");
   histTOT_noise__28->GetYaxis()->SetLabelFont(42);
   histTOT_noise__28->GetYaxis()->SetTitleFont(42);
   histTOT_noise__28->GetZaxis()->SetLabelFont(42);
   histTOT_noise__28->GetZaxis()->SetTitleOffset(1);
   histTOT_noise__28->GetZaxis()->SetTitleFont(42);
   histTOT_noise__28->Draw("");
   histTOT_noise->Modified();
   histTOT_noise->cd();
   histTOT_noise->SetSelected(histTOT_noise);
}
