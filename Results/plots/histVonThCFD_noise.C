void histVonThCFD_noise()
{
//=========Macro generated from canvas: histVonThCFD_noise/histVonThCFD_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histVonThCFD_noise = new TCanvas("histVonThCFD_noise", "histVonThCFD_noise",0,0,800,800);
   histVonThCFD_noise->SetHighLightColor(2);
   histVonThCFD_noise->Range(0,0,1,1);
   histVonThCFD_noise->SetFillColor(0);
   histVonThCFD_noise->SetBorderMode(0);
   histVonThCFD_noise->SetBorderSize(2);
   histVonThCFD_noise->SetFrameBorderMode(0);
   
   TH1D *histVonThCFD_noise__26 = new TH1D("histVonThCFD_noise__26","V on CFD threshold with noise",2000,0,-1);
   histVonThCFD_noise__26->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThCFD_noise__26->SetLineColor(ci);
   histVonThCFD_noise__26->GetXaxis()->SetTitle("Voltage (V)");
   histVonThCFD_noise__26->GetXaxis()->SetRange(1,1);
   histVonThCFD_noise__26->GetXaxis()->SetLabelFont(42);
   histVonThCFD_noise__26->GetXaxis()->SetTitleOffset(1);
   histVonThCFD_noise__26->GetXaxis()->SetTitleFont(42);
   histVonThCFD_noise__26->GetYaxis()->SetTitle("counts");
   histVonThCFD_noise__26->GetYaxis()->SetLabelFont(42);
   histVonThCFD_noise__26->GetYaxis()->SetTitleFont(42);
   histVonThCFD_noise__26->GetZaxis()->SetLabelFont(42);
   histVonThCFD_noise__26->GetZaxis()->SetTitleOffset(1);
   histVonThCFD_noise__26->GetZaxis()->SetTitleFont(42);
   histVonThCFD_noise__26->Draw("");
   histVonThCFD_noise->Modified();
   histVonThCFD_noise->cd();
   histVonThCFD_noise->SetSelected(histVonThCFD_noise);
}
