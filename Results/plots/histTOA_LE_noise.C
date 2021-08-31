void histTOA_LE_noise()
{
//=========Macro generated from canvas: histTOA_LE_noise/histTOA_LE_noise
//=========  (Tue Aug 31 02:17:56 2021) by ROOT version 6.24/02
   TCanvas *histTOA_LE_noise = new TCanvas("histTOA_LE_noise", "histTOA_LE_noise",0,0,800,800);
   histTOA_LE_noise->SetHighLightColor(2);
   histTOA_LE_noise->Range(0,0,1,1);
   histTOA_LE_noise->SetFillColor(0);
   histTOA_LE_noise->SetBorderMode(0);
   histTOA_LE_noise->SetBorderSize(2);
   histTOA_LE_noise->SetFrameBorderMode(0);
   
   TH1D *histTOA_LE_noise__17 = new TH1D("histTOA_LE_noise__17","Time of arrival Leading Edge with noise",2000,-1.41,15.51);
   histTOA_LE_noise__17->SetBinContent(167,999);
   histTOA_LE_noise__17->SetBinContent(1834,1);
   histTOA_LE_noise__17->SetEntries(1000);
   histTOA_LE_noise__17->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histTOA_LE_noise__17->SetLineColor(ci);
   histTOA_LE_noise__17->GetXaxis()->SetTitle("Time (s)");
   histTOA_LE_noise__17->GetXaxis()->SetRange(1,2000);
   histTOA_LE_noise__17->GetXaxis()->SetLabelFont(42);
   histTOA_LE_noise__17->GetXaxis()->SetTitleOffset(1);
   histTOA_LE_noise__17->GetXaxis()->SetTitleFont(42);
   histTOA_LE_noise__17->GetYaxis()->SetTitle("counts");
   histTOA_LE_noise__17->GetYaxis()->SetLabelFont(42);
   histTOA_LE_noise__17->GetYaxis()->SetTitleFont(42);
   histTOA_LE_noise__17->GetZaxis()->SetLabelFont(42);
   histTOA_LE_noise__17->GetZaxis()->SetTitleOffset(1);
   histTOA_LE_noise__17->GetZaxis()->SetTitleFont(42);
   histTOA_LE_noise__17->Draw("");
   histTOA_LE_noise->Modified();
   histTOA_LE_noise->cd();
   histTOA_LE_noise->SetSelected(histTOA_LE_noise);
}
