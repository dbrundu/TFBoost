
{

  gROOT->SetStyle("Plain"); 
  
  TStyle *tfboost_style= new TStyle("tfboost_style","TFBoost plots style");
  
  int colors[8] = {0,5,7,3,6,2,4,1};
  
  tfboost_style->SetFillColor(1);
  tfboost_style->SetFillStyle(1001);
  tfboost_style->SetFrameFillColor(0);
  tfboost_style->SetFrameBorderMode(0);
  tfboost_style->SetPadBorderMode(0);
  tfboost_style->SetPadColor(0);
  tfboost_style->SetCanvasBorderMode(0);
  tfboost_style->SetCanvasColor(0);
  tfboost_style->SetStatColor(0);
  tfboost_style->SetLegendBorderSize(0);
  tfboost_style->SetLegendFont(132);
  tfboost_style->SetPalette(1);
  tfboost_style->SetPalette(8,colors);
  tfboost_style->SetPaperSize(20,26);
  tfboost_style->SetPadTopMargin(0.05);
  tfboost_style->SetPadRightMargin(0.05);
  tfboost_style->SetPadBottomMargin(0.16);
  tfboost_style->SetPadLeftMargin(0.14);
  tfboost_style->SetTextFont(132);
  tfboost_style->SetTextSize(0.054);
  tfboost_style->SetLabelFont(132,"x");
  tfboost_style->SetLabelFont(132,"y");
  tfboost_style->SetLabelFont(132,"z");
  tfboost_style->SetLabelSize(0.054,"x");
  tfboost_style->SetLabelSize(0.054,"y");
  tfboost_style->SetLabelSize(0.054,"z");
  tfboost_style->SetTitleFont(132);
  tfboost_style->SetTitleFont(132,"x");
  tfboost_style->SetTitleFont(132,"y");
  tfboost_style->SetTitleFont(132,"z");
  tfboost_style->SetTitleSize(1.2*0.054,"x");
  tfboost_style->SetTitleSize(1.2*0.054,"y");
  tfboost_style->SetTitleSize(1.2*0.054,"z");
  tfboost_style->SetLineWidth(1.8);
  tfboost_style->SetFrameLineWidth(1.8);
  tfboost_style->SetHistLineWidth(1.8);
  tfboost_style->SetFuncWidth(1.8);
  tfboost_style->SetGridWidth(1.8);
  tfboost_style->SetLineStyleString(2,"[12 12]");
  tfboost_style->SetMarkerStyle(20);
  tfboost_style->SetMarkerSize(1.0);
  tfboost_style->SetLabelOffset(0.010,"X");
  tfboost_style->SetLabelOffset(0.010,"Y");
  tfboost_style->SetOptStat("emr");  
  tfboost_style->SetStatFormat("6.3g");
  tfboost_style->SetOptTitle(0);
  tfboost_style->SetOptFit(0011);
  tfboost_style->SetTitleOffset(0.95,"X");
  tfboost_style->SetTitleOffset(0.95,"Y");
  tfboost_style->SetTitleOffset(1.2,"Z");
  tfboost_style->SetTitleFillColor(0);
  tfboost_style->SetTitleStyle(0);
  tfboost_style->SetTitleBorderSize(0);
  tfboost_style->SetTitleFont(132,"title");
  tfboost_style->SetTitleX(0.0);
  tfboost_style->SetTitleY(1.0); 
  tfboost_style->SetTitleW(1.0);
  tfboost_style->SetTitleH(0.05);
  tfboost_style->SetStatBorderSize(0);
  tfboost_style->SetStatFont(132);
  tfboost_style->SetStatFontSize(0.05);
  tfboost_style->SetStatX(0.9);
  tfboost_style->SetStatY(0.9);
  tfboost_style->SetStatW(0.25);
  tfboost_style->SetStatH(0.15);
  tfboost_style->SetPadTickX(1);
  tfboost_style->SetPadTickY(1);
  tfboost_style->SetNdivisions(505,"x");
  tfboost_style->SetNdivisions(510,"y");

  gROOT->SetStyle("tfboost_style");

  gROOT->ForceStyle();

  cout << "--------------------------" << endl;  
  cout << " Using TFBoost Plot Style " << endl;
  cout << "--------------------------" << endl;  
  
}
