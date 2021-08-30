void histVonThLE_noise()
{
//=========Macro generated from canvas: histVonThLE_noise/histVonThLE_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histVonThLE_noise = new TCanvas("histVonThLE_noise", "histVonThLE_noise",0,0,800,800);
   histVonThLE_noise->SetHighLightColor(2);
   histVonThLE_noise->Range(0,0,1,1);
   histVonThLE_noise->SetFillColor(0);
   histVonThLE_noise->SetBorderMode(0);
   histVonThLE_noise->SetBorderSize(2);
   histVonThLE_noise->SetFrameBorderMode(0);
   
   TH1D *histVonThLE_noise__25 = new TH1D("histVonThLE_noise__25","V on LE threshold with noise",2000,0,-1);
   histVonThLE_noise__25->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThLE_noise__25->SetLineColor(ci);
   histVonThLE_noise__25->GetXaxis()->SetTitle("Voltage (V)");
   histVonThLE_noise__25->GetXaxis()->SetRange(1,1);
   histVonThLE_noise__25->GetXaxis()->SetLabelFont(42);
   histVonThLE_noise__25->GetXaxis()->SetTitleOffset(1);
   histVonThLE_noise__25->GetXaxis()->SetTitleFont(42);
   histVonThLE_noise__25->GetYaxis()->SetTitle("counts");
   histVonThLE_noise__25->GetYaxis()->SetLabelFont(42);
   histVonThLE_noise__25->GetYaxis()->SetTitleFont(42);
   histVonThLE_noise__25->GetZaxis()->SetLabelFont(42);
   histVonThLE_noise__25->GetZaxis()->SetTitleOffset(1);
   histVonThLE_noise__25->GetZaxis()->SetTitleFont(42);
   histVonThLE_noise__25->Draw("");
   histVonThLE_noise->Modified();
   histVonThLE_noise->cd();
   histVonThLE_noise->SetSelected(histVonThLE_noise);
}
