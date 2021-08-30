void rms_noise()
{
//=========Macro generated from canvas: rms_noise/rms_noise
//=========  (Mon Aug 30 17:12:15 2021) by ROOT version 6.24/02
   TCanvas *rms_noise = new TCanvas("rms_noise", "rms_noise",0,0,800,800);
   rms_noise->SetHighLightColor(2);
   rms_noise->Range(0,0,1,1);
   rms_noise->SetFillColor(0);
   rms_noise->SetBorderMode(0);
   rms_noise->SetBorderSize(2);
   rms_noise->SetFrameBorderMode(0);
   
   TH1D *hist_rms_noise;RMSoBVcB;Counts__13 = new TH1D("hist_rms_noise;RMSoBVcB;Counts__13","hist_rms_noise",100,0,-1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   hist_rms_noise;RMSoBVcB;Counts__13->SetLineColor(ci);
   hist_rms_noise;RMSoBVcB;Counts__13->SetLineWidth(2);
   hist_rms_noise;RMSoBVcB;Counts__13->GetXaxis()->SetTitle("RMS [V]");
   hist_rms_noise;RMSoBVcB;Counts__13->GetXaxis()->SetLabelFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->GetXaxis()->SetTitleOffset(1);
   hist_rms_noise;RMSoBVcB;Counts__13->GetXaxis()->SetTitleFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->GetYaxis()->SetTitle("Counts");
   hist_rms_noise;RMSoBVcB;Counts__13->GetYaxis()->SetLabelFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->GetYaxis()->SetLabelSize(0.03);
   hist_rms_noise;RMSoBVcB;Counts__13->GetYaxis()->SetTitleOffset(1.5);
   hist_rms_noise;RMSoBVcB;Counts__13->GetYaxis()->SetTitleFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->GetZaxis()->SetLabelFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->GetZaxis()->SetTitleOffset(1);
   hist_rms_noise;RMSoBVcB;Counts__13->GetZaxis()->SetTitleFont(42);
   hist_rms_noise;RMSoBVcB;Counts__13->Draw("");
   rms_noise->Modified();
   rms_noise->cd();
   rms_noise->SetSelected(rms_noise);
}
