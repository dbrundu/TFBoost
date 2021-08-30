void histdvdt_LE_noise()
{
//=========Macro generated from canvas: histdvdt_LE_noise/histdvdt_LE_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histdvdt_LE_noise = new TCanvas("histdvdt_LE_noise", "histdvdt_LE_noise",0,0,800,800);
   histdvdt_LE_noise->SetHighLightColor(2);
   histdvdt_LE_noise->Range(0,0,1,1);
   histdvdt_LE_noise->SetFillColor(0);
   histdvdt_LE_noise->SetBorderMode(0);
   histdvdt_LE_noise->SetBorderSize(2);
   histdvdt_LE_noise->SetFrameBorderMode(0);
   
   TH1D *histdvdt_LE_noise__22 = new TH1D("histdvdt_LE_noise__22","Slope (dV/dT) on LE threshold with noise",2000,0,-1);
   histdvdt_LE_noise__22->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histdvdt_LE_noise__22->SetLineColor(ci);
   histdvdt_LE_noise__22->GetXaxis()->SetTitle("Slope (uV/ps)");
   histdvdt_LE_noise__22->GetXaxis()->SetRange(1,1);
   histdvdt_LE_noise__22->GetXaxis()->SetLabelFont(42);
   histdvdt_LE_noise__22->GetXaxis()->SetTitleOffset(1);
   histdvdt_LE_noise__22->GetXaxis()->SetTitleFont(42);
   histdvdt_LE_noise__22->GetYaxis()->SetTitle("counts");
   histdvdt_LE_noise__22->GetYaxis()->SetLabelFont(42);
   histdvdt_LE_noise__22->GetYaxis()->SetTitleFont(42);
   histdvdt_LE_noise__22->GetZaxis()->SetLabelFont(42);
   histdvdt_LE_noise__22->GetZaxis()->SetTitleOffset(1);
   histdvdt_LE_noise__22->GetZaxis()->SetTitleFont(42);
   histdvdt_LE_noise__22->Draw("");
   histdvdt_LE_noise->Modified();
   histdvdt_LE_noise->cd();
   histdvdt_LE_noise->SetSelected(histdvdt_LE_noise);
}
