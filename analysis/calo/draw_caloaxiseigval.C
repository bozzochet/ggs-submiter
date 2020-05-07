#include "TFile.h"
#include "TCanvas.h"
#include "TH2.h"



int draw_caloaxiseigval(){
  TFile *fele = TFile::Open("fout.ele_10_10000.root");
  TFile *fpro = TFile::Open("fout.pro_10_10000.root");

  TCanvas *ccaloaxiseigval[3];
  for(int ieigv=0; ieigv<3; ieigv++){
    ccaloaxiseigval[ieigv] = new TCanvas( Form("ccaloaxiseigval_%d",ieigv), Form("ccaloaxiseigval_%d",ieigv) );
    ccaloaxiseigval[ieigv]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      ccaloaxiseigval[ieigv]->cd(ii+1)->SetLogx();
      int binstart = ii*4+1;
      int binstop  = ii*4+4;
      TH2 *h2ele = (TH2*)fele->Get( Form("hcaloaxiseigval_%d", ieigv) );
      TH1 *h1ele = (TH1*)(h2ele->ProjectionY( Form("hcaloaxiseigval_%d_ele_%d",ieigv, ii), binstart,binstop)->Clone()); h1ele->SetStats(0);
      TArrayD *enebins = (TArrayD*)h2ele->GetXaxis()->GetXbins();
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1ele->GetXaxis()->SetTitle( Form("CALO axis eigenval[%d]",ieigv) );
      h1ele->SetLineColor(kRed+1); h1ele->SetFillColor(kRed-10);
      if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(10,1000);
      if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(1,100);
      if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(1,100);
      
      h1ele->DrawNormalized("");
      TH2 *h2pro = (TH2*)fpro->Get( Form("hcaloaxiseigval_%d", ieigv) );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxiseigval_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle(Form("CALO axis eigenval[%d]",ieigv) );
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
    }
  }
  
  const char* eigvalratioaxisname[3] = {"CALO axis eigenval[1]/eigenval[0]","CALO axis eigenval[2]/eigenval[1]","CALO axis eigenval[2]/eigenval[0]"};
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
      if(ieigv==0) h1ele->GetXaxis()->SetRangeUser(0,0.25);
      if(ieigv==1) h1ele->GetXaxis()->SetRangeUser(0,1);
      if(ieigv==2) h1ele->GetXaxis()->SetRangeUser(0,0.25);
      h1ele->DrawNormalized("");
      
      TH2 *h2pro = (TH2*)fpro->Get( eigvalratiohname[ieigv] );
      TH1 *h1pro = (TH1*)(h2pro->ProjectionY( Form("hcaloaxiseigvalratio_%d_pro_%d",ieigv, ii), binstart,binstop)->Clone());
      h1ele->SetTitle( Form("[%.1f - %.1f] GeV [CALO Edep]", enebins->At(binstart-1), enebins->At(binstart+3)) );
      h1pro->GetXaxis()->SetTitle(eigvalratioaxisname[ieigv]);
      h1pro->SetLineColor(kBlue+1);
      h1pro->DrawNormalized("same");
    }
  }
  
  return 0;
  
}
