void histVonThCFD()
{
//=========Macro generated from canvas: histVonThCFD/histVonThCFD
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *histVonThCFD = new TCanvas("histVonThCFD", "histVonThCFD",0,0,800,800);
   histVonThCFD->SetHighLightColor(2);
   histVonThCFD->Range(0,0,1,1);
   histVonThCFD->SetFillColor(0);
   histVonThCFD->SetBorderMode(0);
   histVonThCFD->SetBorderSize(2);
   histVonThCFD->SetFrameBorderMode(0);
   
   TH1D *histVonThCFD__10 = new TH1D("histVonThCFD__10","V on CFD threshold",2000,-1.198,1.2);
   histVonThCFD__10->SetBinContent(1000,1);
   histVonThCFD__10->SetEntries(1);
   histVonThCFD__10->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   histVonThCFD__10->SetLineColor(ci);
   histVonThCFD__10->GetXaxis()->SetTitle("Voltage (V)");
   histVonThCFD__10->GetXaxis()->SetRange(1,2000);
   histVonThCFD__10->GetXaxis()->SetLabelFont(42);
   histVonThCFD__10->GetXaxis()->SetTitleOffset(1);
   histVonThCFD__10->GetXaxis()->SetTitleFont(42);
   histVonThCFD__10->GetYaxis()->SetTitle("counts");
   histVonThCFD__10->GetYaxis()->SetLabelFont(42);
   histVonThCFD__10->GetYaxis()->SetTitleFont(42);
   histVonThCFD__10->GetZaxis()->SetLabelFont(42);
   histVonThCFD__10->GetZaxis()->SetTitleOffset(1);
   histVonThCFD__10->GetZaxis()->SetTitleFont(42);
   histVonThCFD__10->Draw("");
   histVonThCFD->Modified();
   histVonThCFD->cd();
   histVonThCFD->SetSelected(histVonThCFD);
}
