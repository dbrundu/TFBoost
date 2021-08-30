void histVonThRM_noise()
{
//=========Macro generated from canvas: histVonThRM_noise/histVonThRM_noise
//=========  (Mon Aug 30 16:25:59 2021) by ROOT version 6.24/02
   TCanvas *histVonThRM_noise = new TCanvas("histVonThRM_noise", "histVonThRM_noise",0,0,800,800);
   histVonThRM_noise->SetHighLightColor(2);
   histVonThRM_noise->Range(0,0,1,1);
   histVonThRM_noise->SetFillColor(0);
   histVonThRM_noise->SetBorderMode(0);
   histVonThRM_noise->SetBorderSize(2);
   histVonThRM_noise->SetFrameBorderMode(0);
   
   TH1D *histVonThRM_noise__27 = new TH1D("histVonThRM_noise__27","V on RM threshold with noise",2000,0,-1);
   histVonThRM_noise__27->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThRM_noise__27->SetLineColor(ci);
   histVonThRM_noise__27->GetXaxis()->SetTitle("Voltage (V)");
   histVonThRM_noise__27->GetXaxis()->SetRange(1,1);
   histVonThRM_noise__27->GetXaxis()->SetLabelFont(42);
   histVonThRM_noise__27->GetXaxis()->SetTitleOffset(1);
   histVonThRM_noise__27->GetXaxis()->SetTitleFont(42);
   histVonThRM_noise__27->GetYaxis()->SetTitle("counts");
   histVonThRM_noise__27->GetYaxis()->SetLabelFont(42);
   histVonThRM_noise__27->GetYaxis()->SetTitleFont(42);
   histVonThRM_noise__27->GetZaxis()->SetLabelFont(42);
   histVonThRM_noise__27->GetZaxis()->SetTitleOffset(1);
   histVonThRM_noise__27->GetZaxis()->SetTitleFont(42);
   histVonThRM_noise__27->Draw("");
   histVonThRM_noise->Modified();
   histVonThRM_noise->cd();
   histVonThRM_noise->SetSelected(histVonThRM_noise);
}
