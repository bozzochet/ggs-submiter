#include "TFile.h"
#include "TCanvas.h"
#include "TH2.h"


int draw_caloaxiseigval(){

  TFile *fele = TFile::Open("fout.ele_10_1000.root");
  TFile *fpro = TFile::Open("fout.pro_10_1000.root");

  const char* eigvalaxisname[2] = {"#lambda_{0} (cm)","(#lambda_{1}+#lambda_{2})/2 (cm)"};
  const char* eigvalhname[2] = {"hcaloaxiseigval_0","hcaloaxiseigval_12"};
  TH1F *hele[3][9], *hpro[3][9];

  TCanvas *ccaloaxiseigval[3];
  for(int ieigv=0; ieigv<2; ieigv++){
    ccaloaxiseigval[ieigv] = new TCanvas( Form("ccaloaxiseigval_%d",ieigv), Form("ccaloaxiseigval_%d",ieigv) );
    ccaloaxiseigval[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxiseigval[ieigv]->cd(ii+1)->SetLogx();
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( eigvalhname[ieigv] );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxiseigval_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); //h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( eigvalaxisname[ieigv] );
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(1,30);
      if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(1,10);
      if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(1,10);
      hele[ieigv][ii] = (TH1F*)(h1ele->Clone( Form("hele_%d_%d", ieigv, ii) ) );
      
      h1ele->DrawNormalized("");
      TH2 *h2pro = (TH2*)fpro->Get( eigvalhname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxiseigval_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle( eigvalaxisname[ieigv] );
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
      hpro[ieigv][ii] = (TH1F*)(h1pro->Clone( Form("hpro_%d_%d", ieigv, ii) ) );
    }
  }
  
  //  const char* eigvalratioaxisname[3] = {"CALO axis eigenval[1]/eigenval[0]","CALO axis eigenval[2]/eigenval[1]","CALO axis eigenval[2]/eigenval[0]"};
  const char* eigvalratioaxisname[3] = {"#lambda_{0} / #lambda_{1}","#lambda_{2} / #lambda_{1}","#lambda_{2} / #lambda_{0}"};
  const char* eigvalratiohname[3] = {"hcaloaxiseigval_1_0","hcaloaxiseigval_2_1","hcaloaxiseigval_2_0"};
  TCanvas *ccaloaxiseigvalratio[3];
  for(int ieigv=0; ieigv<3; ieigv++){
    ccaloaxiseigvalratio[ieigv] = new TCanvas( Form("ccaloaxiseigvalratio_%d",ieigv), Form("ccaloaxiseigvalratio_%d",ieigv) );
    ccaloaxiseigvalratio[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxiseigvalratio[ieigv]->cd(ii+1);
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( eigvalratiohname[ieigv] );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxiseigvalratio_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( eigvalratioaxisname[ieigv]);
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(0,0.5);
      if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(0.4,1);
      if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(0,0.5);
      h1ele->DrawNormalized("");
      
      TH2 *h2pro = (TH2*)fpro->Get( eigvalratiohname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxiseigvalratio_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle(eigvalratioaxisname[ieigv]);
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
    }
  }

  TCanvas *clambda0ele = new TCanvas("clambda0ele","clambda0ele");
  clambda0ele->cd();
  hele[0][1]->SetFillStyle(0); hele[0][1]->SetLineColor(kRed-7);   hele[0][1]->DrawNormalized("");    hele[0][1]->GetXaxis()->SetRangeUser(0,20);
  hele[0][4]->SetFillStyle(0); hele[0][4]->SetLineColor(kRed-4);   hele[0][4]->DrawNormalized("same"); 
  hele[0][7]->SetFillStyle(0); hele[0][7]->SetLineColor(kRed+1);   hele[0][7]->DrawNormalized("same"); 

  TCanvas *clambda0pro = new TCanvas("clambda0pro","clambda0pro");
  clambda0pro->cd();
  hpro[0][1]->SetFillStyle(0); hpro[0][1]->SetLineColor(kBlue-7); hpro[0][1]->DrawNormalized("");     
  hpro[0][4]->SetFillStyle(0); hpro[0][4]->SetLineColor(kBlue-4); hpro[0][4]->DrawNormalized("same"); 
  hpro[0][7]->SetFillStyle(0); hpro[0][7]->SetLineColor(kBlue+1); hpro[0][7]->DrawNormalized("same"); 
  
  TCanvas *clambda1ele = new TCanvas("clambda1ele","clambda1ele");
  clambda1ele->cd();
  hele[1][1]->SetFillStyle(0); hele[1][1]->SetLineColor(kRed-7); hele[1][1]->DrawNormalized(); hele[1][1]->GetXaxis()->SetRangeUser(0,20);
  hele[1][4]->SetFillStyle(0); hele[1][4]->SetLineColor(kRed-4); hele[1][4]->DrawNormalized("same");
  hele[1][7]->SetFillStyle(0); hele[1][7]->SetLineColor(kRed+1); hele[1][7]->DrawNormalized("same");

  TCanvas *clambda1pro = new TCanvas("clambda1pro","clambda1pro");
  clambda1pro->cd();
  hpro[1][1]->SetFillStyle(0); hpro[1][1]->SetLineColor(kBlue-7); hpro[1][1]->DrawNormalized();
  hpro[1][4]->SetFillStyle(0); hpro[1][4]->SetLineColor(kBlue-4); hpro[1][4]->DrawNormalized("same");
  hpro[1][7]->SetFillStyle(0); hpro[1][7]->SetLineColor(kBlue+1); hpro[1][7]->DrawNormalized("same");

  return 0;
  
}

int draw_caloaxissigma(){

  TFile *fele = TFile::Open("fout.ele_10_1000.root");
  TFile *fpro = TFile::Open("fout.pro_10_1000.root");

  const char* sigmaaxisname[3] = {"#sigma_{0} (cm)","#sigma_{1} (cm)", "#sigma_{2} (cm)"};
  const char* sigmahname[3] = {"hcaloaxissigma_0","hcaloaxissigma_1","hcaloaxissigma_2"};
  TH1F *hele[3][9], *hpro[3][9];

  TCanvas *ccaloaxissigma[3];
  for(int ieigv=0; ieigv<3; ieigv++){
    ccaloaxissigma[ieigv] = new TCanvas( Form("ccaloaxissigma_%d",ieigv), Form("ccaloaxissigma_%d",ieigv) );
    ccaloaxissigma[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxissigma[ieigv]->cd(ii+1);
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( sigmahname[ieigv] );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxissigma_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); //h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( sigmaaxisname[ieigv] );
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(0,20);
      if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(0,20);
      if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(0,20);
      hele[ieigv][ii] = (TH1F*)(h1ele->Clone( Form("hele_%d_%d", ieigv, ii) ) );
      
      h1ele->DrawNormalized("");
      TH2 *h2pro = (TH2*)fpro->Get( sigmahname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxissigma_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle( sigmaaxisname[ieigv] );
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
      hpro[ieigv][ii] = (TH1F*)(h1pro->Clone( Form("hpro_%d_%d", ieigv, ii) ) );
    }
  }
  
  TCanvas *csigma0ele = new TCanvas("csigma0ele","csigma0ele");
  csigma0ele->cd();
  hele[0][1]->SetFillStyle(0); hele[0][1]->SetLineColor(kRed-7);   hele[0][1]->DrawNormalized("");    hele[0][1]->GetXaxis()->SetRangeUser(0,20);
  hele[0][4]->SetFillStyle(0); hele[0][4]->SetLineColor(kRed-4);   hele[0][4]->DrawNormalized("same"); 
  hele[0][7]->SetFillStyle(0); hele[0][7]->SetLineColor(kRed+1);   hele[0][7]->DrawNormalized("same"); 

  TCanvas *csigma0pro = new TCanvas("csigma0pro","csigma0pro");
  csigma0pro->cd();
  hpro[0][1]->SetFillStyle(0); hpro[0][1]->SetLineColor(kBlue-7); hpro[0][1]->DrawNormalized("");     
  hpro[0][4]->SetFillStyle(0); hpro[0][4]->SetLineColor(kBlue-4); hpro[0][4]->DrawNormalized("same"); 
  hpro[0][7]->SetFillStyle(0); hpro[0][7]->SetLineColor(kBlue+1); hpro[0][7]->DrawNormalized("same"); 
  
  TCanvas *csigma1ele = new TCanvas("csigma1ele","csigma1ele");
  csigma1ele->cd();
  hele[1][1]->SetFillStyle(0); hele[1][1]->SetLineColor(kRed-7); hele[1][1]->DrawNormalized(); hele[1][1]->GetXaxis()->SetRangeUser(0,20);
  hele[1][4]->SetFillStyle(0); hele[1][4]->SetLineColor(kRed-4); hele[1][4]->DrawNormalized("same");
  hele[1][7]->SetFillStyle(0); hele[1][7]->SetLineColor(kRed+1); hele[1][7]->DrawNormalized("same");

  TCanvas *csigma1pro = new TCanvas("csigma1pro","csigma1pro");
  csigma1pro->cd();
  hpro[1][1]->SetFillStyle(0); hpro[1][1]->SetLineColor(kBlue-7); hpro[1][1]->DrawNormalized();
  hpro[1][4]->SetFillStyle(0); hpro[1][4]->SetLineColor(kBlue-4); hpro[1][4]->DrawNormalized("same");
  hpro[1][7]->SetFillStyle(0); hpro[1][7]->SetLineColor(kBlue+1); hpro[1][7]->DrawNormalized("same");

    TCanvas *csigma2ele = new TCanvas("csigma2ele","csigma2ele");
  csigma2ele->cd();
  hele[2][1]->SetFillStyle(0); hele[2][1]->SetLineColor(kRed-7); hele[2][1]->DrawNormalized(); hele[2][1]->GetXaxis()->SetRangeUser(0,20);
  hele[2][4]->SetFillStyle(0); hele[2][4]->SetLineColor(kRed-4); hele[2][4]->DrawNormalized("same");
  hele[2][7]->SetFillStyle(0); hele[2][7]->SetLineColor(kRed+2); hele[2][7]->DrawNormalized("same");

  TCanvas *csigma2pro = new TCanvas("csigma2pro","csigma2pro");
  csigma2pro->cd();
  hpro[2][1]->SetFillStyle(0); hpro[2][1]->SetLineColor(kBlue-7); hpro[2][1]->DrawNormalized();
  hpro[2][4]->SetFillStyle(0); hpro[2][4]->SetLineColor(kBlue-4); hpro[2][4]->DrawNormalized("same");
  hpro[2][7]->SetFillStyle(0); hpro[2][7]->SetLineColor(kBlue+2); hpro[2][7]->DrawNormalized("same");

  return 0;
  
}


int draw_caloaxisskew(){

  TFile *fele = TFile::Open("fout.ele_10_1000.root");
  TFile *fpro = TFile::Open("fout.pro_10_1000.root");

  const char* skewaxisname[3] = {"s_{0}","s_{1}", "s_{2}"};
  const char* skewhname[3] = {"hcaloaxisskew_0","hcaloaxisskew_1","hcaloaxisskew_2"};
  TH1F *hele[3][9], *hpro[3][9];

  TCanvas *ccaloaxisskew[3];
  for(int ieigv=0; ieigv<3; ieigv++){
    ccaloaxisskew[ieigv] = new TCanvas( Form("ccaloaxisskew_%d",ieigv), Form("ccaloaxisskew_%d",ieigv) );
    ccaloaxisskew[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxisskew[ieigv]->cd(ii+1);
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( skewhname[ieigv] );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxisskew_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); //h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( skewaxisname[ieigv] );
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      // if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(1,30);
      // if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(1,10);
      // if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(1,10);
      hele[ieigv][ii] = (TH1F*)(h1ele->Clone( Form("hele_%d_%d", ieigv, ii) ) );
      
      h1ele->DrawNormalized("");
      TH2 *h2pro = (TH2*)fpro->Get( skewhname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxisskew_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle( skewaxisname[ieigv] );
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
      hpro[ieigv][ii] = (TH1F*)(h1pro->Clone( Form("hpro_%d_%d", ieigv, ii) ) );
    }
  }
  
  TCanvas *cskew0ele = new TCanvas("cskew0ele","cskew0ele");
  cskew0ele->cd();
  hele[0][1]->SetFillStyle(0); hele[0][1]->SetLineColor(kRed-7);   hele[0][1]->DrawNormalized("");    hele[0][1]->GetXaxis()->SetRangeUser(0,20);
  hele[0][4]->SetFillStyle(0); hele[0][4]->SetLineColor(kRed-4);   hele[0][4]->DrawNormalized("same"); 
  hele[0][7]->SetFillStyle(0); hele[0][7]->SetLineColor(kRed+1);   hele[0][7]->DrawNormalized("same"); 

  TCanvas *cskew0pro = new TCanvas("cskew0pro","cskew0pro");
  cskew0pro->cd();
  hpro[0][1]->SetFillStyle(0); hpro[0][1]->SetLineColor(kBlue-7); hpro[0][1]->DrawNormalized("");     
  hpro[0][4]->SetFillStyle(0); hpro[0][4]->SetLineColor(kBlue-4); hpro[0][4]->DrawNormalized("same"); 
  hpro[0][7]->SetFillStyle(0); hpro[0][7]->SetLineColor(kBlue+1); hpro[0][7]->DrawNormalized("same"); 
  
  TCanvas *cskew1ele = new TCanvas("cskew1ele","cskew1ele");
  cskew1ele->cd();
  hele[1][1]->SetFillStyle(0); hele[1][1]->SetLineColor(kRed-7); hele[1][1]->DrawNormalized(); hele[1][1]->GetXaxis()->SetRangeUser(0,20);
  hele[1][4]->SetFillStyle(0); hele[1][4]->SetLineColor(kRed-4); hele[1][4]->DrawNormalized("same");
  hele[1][7]->SetFillStyle(0); hele[1][7]->SetLineColor(kRed+1); hele[1][7]->DrawNormalized("same");

  TCanvas *cskew1pro = new TCanvas("cskew1pro","cskew1pro");
  cskew1pro->cd();
  hpro[1][1]->SetFillStyle(0); hpro[1][1]->SetLineColor(kBlue-7); hpro[1][1]->DrawNormalized();
  hpro[1][4]->SetFillStyle(0); hpro[1][4]->SetLineColor(kBlue-4); hpro[1][4]->DrawNormalized("same");
  hpro[1][7]->SetFillStyle(0); hpro[1][7]->SetLineColor(kBlue+1); hpro[1][7]->DrawNormalized("same");

    TCanvas *cskew2ele = new TCanvas("cskew2ele","cskew2ele");
  cskew2ele->cd();
  hele[2][1]->SetFillStyle(0); hele[2][1]->SetLineColor(kRed-7); hele[2][1]->DrawNormalized(); hele[2][1]->GetXaxis()->SetRangeUser(0,20);
  hele[2][4]->SetFillStyle(0); hele[2][4]->SetLineColor(kRed-4); hele[2][4]->DrawNormalized("same");
  hele[2][7]->SetFillStyle(0); hele[2][7]->SetLineColor(kRed+2); hele[2][7]->DrawNormalized("same");

  TCanvas *cskew2pro = new TCanvas("cskew2pro","cskew2pro");
  cskew2pro->cd();
  hpro[2][1]->SetFillStyle(0); hpro[2][1]->SetLineColor(kBlue-7); hpro[2][1]->DrawNormalized();
  hpro[2][4]->SetFillStyle(0); hpro[2][4]->SetLineColor(kBlue-4); hpro[2][4]->DrawNormalized("same");
  hpro[2][7]->SetFillStyle(0); hpro[2][7]->SetLineColor(kBlue+2); hpro[2][7]->DrawNormalized("same");

  return 0;
  
}
int draw_caloaxiskurt(){

  TFile *fele = TFile::Open("fout.ele_10_1000.root");
  TFile *fpro = TFile::Open("fout.pro_10_1000.root");

  const char* kurtaxisname[3] = {"k_{0}","k_{1}", "k_{2}"};
  const char* kurthname[3] = {"hcaloaxiskurt_0","hcaloaxiskurt_1","hcaloaxiskurt_2"};
  TH1F *hele[3][9], *hpro[3][9];

  TCanvas *ccaloaxiskurt[3];
  for(int ieigv=0; ieigv<3; ieigv++){
    ccaloaxiskurt[ieigv] = new TCanvas( Form("ccaloaxiskurt_%d",ieigv), Form("ccaloaxiskurt_%d",ieigv) );
    ccaloaxiskurt[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxiskurt[ieigv]->cd(ii+1);
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( kurthname[ieigv] );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxiskurt_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); //h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( kurtaxisname[ieigv] );
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      // if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(1,30);
      // if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(1,10);
      // if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(1,10);
      hele[ieigv][ii] = (TH1F*)(h1ele->Clone( Form("hele_%d_%d", ieigv, ii) ) );
      
      h1ele->DrawNormalized("");
      TH2 *h2pro = (TH2*)fpro->Get( kurthname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxiskurt_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle( kurtaxisname[ieigv] );
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
      hpro[ieigv][ii] = (TH1F*)(h1pro->Clone( Form("hpro_%d_%d", ieigv, ii) ) );
    }
  }
  
  TCanvas *ckurt0ele = new TCanvas("ckurt0ele","ckurt0ele");
  ckurt0ele->cd();
  hele[0][1]->SetFillStyle(0); hele[0][1]->SetLineColor(kRed-7);   hele[0][1]->DrawNormalized("");    hele[0][1]->GetXaxis()->SetRangeUser(0,20);
  hele[0][4]->SetFillStyle(0); hele[0][4]->SetLineColor(kRed-4);   hele[0][4]->DrawNormalized("same"); 
  hele[0][7]->SetFillStyle(0); hele[0][7]->SetLineColor(kRed+1);   hele[0][7]->DrawNormalized("same"); 

  TCanvas *ckurt0pro = new TCanvas("ckurt0pro","ckurt0pro");
  ckurt0pro->cd();
  hpro[0][1]->SetFillStyle(0); hpro[0][1]->SetLineColor(kBlue-7); hpro[0][1]->DrawNormalized("");     
  hpro[0][4]->SetFillStyle(0); hpro[0][4]->SetLineColor(kBlue-4); hpro[0][4]->DrawNormalized("same"); 
  hpro[0][7]->SetFillStyle(0); hpro[0][7]->SetLineColor(kBlue+1); hpro[0][7]->DrawNormalized("same"); 
  
  TCanvas *ckurt1ele = new TCanvas("ckurt1ele","ckurt1ele");
  ckurt1ele->cd();
  hele[1][1]->SetFillStyle(0); hele[1][1]->SetLineColor(kRed-7); hele[1][1]->DrawNormalized(); hele[1][1]->GetXaxis()->SetRangeUser(0,20);
  hele[1][4]->SetFillStyle(0); hele[1][4]->SetLineColor(kRed-4); hele[1][4]->DrawNormalized("same");
  hele[1][7]->SetFillStyle(0); hele[1][7]->SetLineColor(kRed+1); hele[1][7]->DrawNormalized("same");

  TCanvas *ckurt1pro = new TCanvas("ckurt1pro","ckurt1pro");
  ckurt1pro->cd();
  hpro[1][1]->SetFillStyle(0); hpro[1][1]->SetLineColor(kBlue-7); hpro[1][1]->DrawNormalized();
  hpro[1][4]->SetFillStyle(0); hpro[1][4]->SetLineColor(kBlue-4); hpro[1][4]->DrawNormalized("same");
  hpro[1][7]->SetFillStyle(0); hpro[1][7]->SetLineColor(kBlue+1); hpro[1][7]->DrawNormalized("same");

    TCanvas *ckurt2ele = new TCanvas("ckurt2ele","ckurt2ele");
  ckurt2ele->cd();
  hele[2][1]->SetFillStyle(0); hele[2][1]->SetLineColor(kRed-7); hele[2][1]->DrawNormalized(); hele[2][1]->GetXaxis()->SetRangeUser(0,20);
  hele[2][4]->SetFillStyle(0); hele[2][4]->SetLineColor(kRed-4); hele[2][4]->DrawNormalized("same");
  hele[2][7]->SetFillStyle(0); hele[2][7]->SetLineColor(kRed+2); hele[2][7]->DrawNormalized("same");

  TCanvas *ckurt2pro = new TCanvas("ckurt2pro","ckurt2pro");
  ckurt2pro->cd();
  hpro[2][1]->SetFillStyle(0); hpro[2][1]->SetLineColor(kBlue-7); hpro[2][1]->DrawNormalized();
  hpro[2][4]->SetFillStyle(0); hpro[2][4]->SetLineColor(kBlue-4); hpro[2][4]->DrawNormalized("same");
  hpro[2][7]->SetFillStyle(0); hpro[2][7]->SetLineColor(kBlue+2); hpro[2][7]->DrawNormalized("same");

  return 0;
  
}
