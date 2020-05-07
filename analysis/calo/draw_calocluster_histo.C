#include "TFile.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TH2.h"



int draw_calocluster_histo(){
  TFile *f = TFile::Open("outfile.caloclusters.temp.root");
  TH3F *honecaloclusterhitsedep = (TH3F*)f->Get("honecaloclusterhitsedep");
  TH3F *honecaloclusterhitsleak = (TH3F*)f->Get("honecaloclusterhitsleak");

  TCanvas *c1 = new TCanvas("c1","c1");
  c1->Divide(2,2);
  for(int ii=0; ii<4; ii++){
    c1->cd(ii+1)->SetLogx(); c1->cd(ii+1)->SetLogy(); c1->cd(ii+1)->SetTicks();
    int binstart = ii*5+1;
    int binstop  = ii*5+5;
    honecaloclusterhitsedep->GetXaxis()->SetRange(binstart,binstop);
    TH2 *hp = (TH2*)(honecaloclusterhitsedep->Project3D( "yz" ));
    hp->SetName( Form("h_zy_%d",ii) );
    hp->GetYaxis()->SetRangeUser(0.5,1);
    hp->Draw("COLZ"); hp->SetStats(0);
    hp->GetYaxis()->SetTitle("Cluster Edep / CALO Edep");
    hp->GetXaxis()->SetTitle("Cluster Hits / CALO Hits");
    hp->SetTitle( Form("[%.1f - %.1f] GV (MC)", honecaloclusterhitsedep->GetXaxis()->GetBinLowEdge(binstart),  honecaloclusterhitsedep->GetXaxis()->GetBinLowEdge(binstop+1) ) );
  }

  TCanvas *c2 = new TCanvas("c2","c2");
  c2->Divide(2,2);
  for(int ii=0; ii<4; ii++){
    c2->cd(ii+1)->SetLogx(); c2->cd(ii+1)->SetLogy(); c2->cd(ii+1)->SetTicks();
    int binstart = ii*5+1;
    int binstop  = ii*5+5;
    honecaloclusterhitsleak->GetXaxis()->SetRange(binstart,binstop);
    TH2 *hp = (TH2*)(honecaloclusterhitsleak->Project3D( "yz" ));
    hp->SetName( Form("h_zy_%d",ii) );
    hp->GetYaxis()->SetRangeUser(1e-3,1);
    hp->Draw("COLZ"); hp->SetStats(0);
    hp->GetYaxis()->SetTitle("1 - (Cluster Edep / CALO Edep)");
    hp->GetXaxis()->SetTitle("Cluster Hits / CALO Hits");
    hp->SetTitle( Form("[%.1f - %.1f] GV (MC)", honecaloclusterhitsleak->GetXaxis()->GetBinLowEdge(binstart),  honecaloclusterhitsleak->GetXaxis()->GetBinLowEdge(binstop+1) ) );
  }

  
  return 0;
  
}
