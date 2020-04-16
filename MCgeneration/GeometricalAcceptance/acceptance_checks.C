#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

TTree *t;
int mcNdiscarded;           
float mcDir[3];             
float mcCoo[3];             
float mcMom;                
float mcPhi;                
float mcCtheta;             
int mcStkintersections;     
float mcTracklengthcalox0;  
float mcTrackcaloentry[3];  
float mcTrackcaloexit[3];   
int mcTrackcaloentryplane;  
int mcTrackcaloexitplane;   

float calofidvolalpha;
bool calofidvolpass;

int calonhits;      
float calototedep;


  const float sphrad = 3; //m^2
  const float sphacc = TMath::Pi() * (4*TMath::Pi()*pow(sphrad,2)); //see Sullivan


int verify_calo_borders();
int verify_calo_fidvol();

Double_t* GenerateLogBinning(int nbins, float min, float max){

  Double_t *X = new Double_t[nbins+1];
  Double_t log_interval = ( TMath::Log10(max) - TMath::Log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) X[i] = TMath::Power( 10, TMath::Log10(min) + i * log_interval );
  return X;

}

Double_t* GenerateBinning(int nbins, float min, float max){

  Double_t *X = new Double_t[nbins+1];
  Double_t interval = ( max - min ) / nbins;
  for(int i=0; i<=nbins; i++) X[i] = min + i*interval;
  return X;

}

int acceptance_checks(){

  TFile *f = TFile::Open("tree.electrons.root");
  t = (TTree*)f->Get("tree");

  
  t->SetBranchAddress("mcNdiscarded",&mcNdiscarded);
  t->SetBranchAddress("mcDir",&mcDir[0]);
  t->SetBranchAddress("mcCoo",&mcCoo[0]);
  t->SetBranchAddress("mcMom",&mcMom);
  t->SetBranchAddress("mcPhi",&mcPhi);
  t->SetBranchAddress("mcCtheta",&mcCtheta);
  t->SetBranchAddress("mcStkintersections",&mcStkintersections);
  t->SetBranchAddress("mcTracklengthcalox0",&mcTracklengthcalox0);
  t->SetBranchAddress("mcTrackcaloentry",&mcTrackcaloentry[0]);
  t->SetBranchAddress("mcTrackcaloexit",&mcTrackcaloexit[0]);
  t->SetBranchAddress("mcTrackcaloentryplane",&mcTrackcaloentryplane);
  t->SetBranchAddress("mcTrackcaloexitplane",&mcTrackcaloexitplane);

  t->SetBranchAddress("calofidvolalpha",&calofidvolalpha);
  t->SetBranchAddress("calofidvolpass",&calofidvolpass);
  t->SetBranchAddress("calonhits",&calonhits);
  t->SetBranchAddress("calototedep",&calototedep);

  //  verify_calo_borders();
 verify_calo_fidvol();
  return 0;

}

int verify_calo_borders(){

  //  const char* calotracklengthname[4] = {"Total_noBOT","TOPonly","LATonly","LAT_noCORNERS"};
  const char* dirname[11] = {"None", "Xpos", "Xneg", "Ypos",    "Yneg",    "Zpos", "Zneg", "XnegYneg", "XposYneg", "XnegYpos", "XposYpos"};
  const int dircolor[11]  = {kBlack,  kRed,  kBlue-8, kGreen+1, kOrange+2, kBlue+1, kCyan,   kSpring-8,  kMagenta-8, kYellow-8, kGray+1   };   
  TH1F *hcalotracklength_noBOT[11];
  TH1F *hcalotracklength_TOPonly[11];
  TH1F *hcalotracklength_LATonly[11];
  TH1F *hcalotracklength_LATnocorners[11];
  TGraph *gacccalotracklength_noBOT[11];
  TGraph *gacccalotracklength_TOPonly[11];
  TGraph *gacccalotracklength_LATonly[11];
  TGraph *gacccalotracklength_LATnocorners[11];
  for(int i=0; i<11; i++){
    hcalotracklength_noBOT[i] = new TH1F( Form("hcalotracklength_noBOT_%d",i),               Form("MC track in calo - Entry: not from BOTTOM - Exit: %s",i==0 ? "All" : dirname[i+1]), 500, 0, 100 );
    hcalotracklength_noBOT[i]->SetLineColor( dircolor[i] );
    hcalotracklength_noBOT[i]->GetXaxis()->SetTitle("MC track length (X_{0})");
    gacccalotracklength_noBOT[i] = new TGraph(); gacccalotracklength_noBOT[i]->SetNameTitle( Form("gacccalotracklength_noBOT_%d",i), hcalotracklength_noBOT[i]->GetTitle() );
    gacccalotracklength_noBOT[i]->SetMarkerColor( dircolor[i] );     gacccalotracklength_noBOT[i]->SetMarkerSize(0.5);
    hcalotracklength_TOPonly[i] = new TH1F( Form("hcalotracklength_TOPonly_%d",i),           Form("MC track in calo - Entry: TOP only - Exit: %s",i==0 ? "All" : dirname[i+1]), 500, 0, 100 );
    hcalotracklength_TOPonly[i]->SetLineColor( dircolor[i] );
    hcalotracklength_TOPonly[i]->GetXaxis()->SetTitle("MC track length (X_{0})");
    gacccalotracklength_TOPonly[i] = new TGraph(); gacccalotracklength_TOPonly[i]->SetNameTitle( Form("gacccalotracklength_TOPonly_%d",i), hcalotracklength_TOPonly[i]->GetTitle() );
    gacccalotracklength_TOPonly[i]->SetMarkerColor( dircolor[i] );     gacccalotracklength_TOPonly[i]->SetMarkerSize(0.5);
    hcalotracklength_LATonly[i] = new TH1F( Form("hcalotracklength_LATonly_%d",i),           Form("MC track in calo - Entry: LAT planes only - Exit: %s",i==0 ? "All" : dirname[i+1]), 500, 0, 100 );
    hcalotracklength_LATonly[i]->SetLineColor( dircolor[i] );
    hcalotracklength_LATonly[i]->GetXaxis()->SetTitle("MC track length (X_{0})");
    gacccalotracklength_LATonly[i] = new TGraph(); gacccalotracklength_LATonly[i]->SetNameTitle( Form("gacccalotracklength_LATonly_%d",i), hcalotracklength_LATonly[i]->GetTitle() );
    gacccalotracklength_LATonly[i]->SetMarkerColor( dircolor[i] );     gacccalotracklength_LATonly[i]->SetMarkerSize(0.5);
    hcalotracklength_LATnocorners[i] = new TH1F( Form("hcalotracklength_LATNocorners_%d",i), Form("MC track in calo - Entry: LAT planes (no corners) - Exit: %s",i==0 ? "All" : dirname[i+1]), 500, 0, 100 );
    hcalotracklength_LATnocorners[i]->SetLineColor( dircolor[i] );
    hcalotracklength_LATnocorners[i]->GetXaxis()->SetTitle("MC track length (X_{0})");
    gacccalotracklength_LATnocorners[i] = new TGraph(); gacccalotracklength_LATnocorners[i]->SetNameTitle( Form("gacccalotracklength_LATnocorners_%d",i), hcalotracklength_LATnocorners[i]->GetTitle() );
    gacccalotracklength_LATnocorners[i]->SetMarkerColor( dircolor[i] );     gacccalotracklength_LATnocorners[i]->SetMarkerSize(0.5);
  }
  TH2F *hcaloentryexit = new TH2F("hcaloentryexit","MC track CAL0 Exit vs Entry",11,-1.5,9.5,11,-1.5,9.5);
  for(int ibinx=1; ibinx<=hcaloentryexit->GetNbinsX(); ibinx++) hcaloentryexit->GetXaxis()->SetBinLabel( ibinx, dirname[ibinx-1]);
  for(int ibiny=1; ibiny<=hcaloentryexit->GetNbinsY(); ibiny++) hcaloentryexit->GetYaxis()->SetBinLabel( ibiny, dirname[ibiny-1]);
  
  int nentries=(int)t->GetEntries();
  int ngen=0;
  int nmctrackincalo=0;
  
  //---------LOOP on events
  for(int ientry=0; ientry<nentries; ientry++){
    if(ientry%100000==0) printf("%d/%d\n",ientry,nentries);
    
    t->GetEntry(ientry);

    ngen+=1+mcNdiscarded;
    hcaloentryexit->Fill(mcTrackcaloentryplane,mcTrackcaloexitplane);
    if(mcTrackcaloentryplane<0 && mcTrackcaloexitplane<0) continue;
    nmctrackincalo++;

    if(mcTrackcaloentryplane != 5) //Not entry from Zneg
      {
	hcalotracklength_noBOT[0]->Fill(mcTracklengthcalox0);
	hcalotracklength_noBOT[mcTrackcaloexitplane+1]->Fill(mcTracklengthcalox0);
      }
    if(mcTrackcaloentryplane == 4) //Entry from Zpos only
      {
	hcalotracklength_TOPonly[0]->Fill(mcTracklengthcalox0);
	hcalotracklength_TOPonly[mcTrackcaloexitplane+1]->Fill(mcTracklengthcalox0);
      }
    if(mcTrackcaloentryplane<4 || mcTrackcaloentryplane>5) //Entry from all lateral sides
      {
	hcalotracklength_LATonly[0]->Fill(mcTracklengthcalox0);
	hcalotracklength_LATonly[mcTrackcaloexitplane+1]->Fill(mcTracklengthcalox0);
      }
    if(mcTrackcaloentryplane<4) //Entry from all lateral sides, not from the corners
      {
	hcalotracklength_LATnocorners[0]->Fill(mcTracklengthcalox0);
	hcalotracklength_LATnocorners[mcTrackcaloexitplane+1]->Fill(mcTracklengthcalox0);
      }
  }

  //compute acceptance as function of shower length
  for(int idir=0; idir<11; idir++){
    for( int ibinx=1; ibinx<hcalotracklength_noBOT[idir]->GetNbinsX(); ibinx++){
      float npass=0;
      float x = hcalotracklength_noBOT[idir]->GetBinLowEdge(ibinx);

      npass = hcalotracklength_noBOT[idir]->Integral(ibinx, hcalotracklength_noBOT[idir]->GetNbinsX());
      gacccalotracklength_noBOT[idir]->SetPoint( gacccalotracklength_noBOT[idir]->GetN(), x, npass/(float)ngen * sphacc);

      npass = hcalotracklength_TOPonly[idir]->Integral(ibinx, hcalotracklength_TOPonly[idir]->GetNbinsX());
      gacccalotracklength_TOPonly[idir]->SetPoint( gacccalotracklength_TOPonly[idir]->GetN(), x, npass/(float)ngen * sphacc);

      npass = hcalotracklength_LATonly[idir]->Integral(ibinx, hcalotracklength_LATonly[idir]->GetNbinsX());
      gacccalotracklength_LATonly[idir]->SetPoint( gacccalotracklength_LATonly[idir]->GetN(), x, npass/(float)ngen * sphacc);

      npass = hcalotracklength_LATnocorners[idir]->Integral(ibinx, hcalotracklength_LATnocorners[idir]->GetNbinsX());
      gacccalotracklength_LATnocorners[idir]->SetPoint( gacccalotracklength_LATnocorners[idir]->GetN(), x, npass/(float)ngen * sphacc);

    }
  }
  
  printf("TotalGenerated:%d\n",ngen);
  float geomacc = (nentries/(float)ngen) * sphacc;
  printf("Sphere::Acceptance:%f m^2\n",sphacc);
  printf("Geom::Acceptance:%f m^2\n",geomacc);
  float trackincaloacc = (nmctrackincalo/(float)ngen) * sphacc;
  printf("TrackInCalo::Acceptance:%f m^2\n",trackincaloacc);
  
  TCanvas *ccaloentryexit = new TCanvas("caloentryexit","caloentryexit"); ccaloentryexit->SetTicks();
  hcaloentryexit->DrawNormalized("COLZ");

  TLegend *leg = new TLegend(0.86,0.2,0.95,0.85); leg->SetBorderSize(0);
  leg->AddEntry( (TObject*)NULL, "EXIT", "");
  for(int idir=0; idir<11; idir++) leg->AddEntry(hcalotracklength_noBOT[idir], Form("%s", idir==0 ? "ALL" : dirname[idir]), "L");

  TCanvas *ccalotracklength_noBOT = new TCanvas("ccalotracklength_noBOT","ccalotracklength_noBOT");
  ccalotracklength_noBOT->Divide(2,1);
  ccalotracklength_noBOT->cd(1)->SetLogy();
  ccalotracklength_noBOT->SetLogy();
  for(int idir=0; idir<11; idir++) hcalotracklength_noBOT[idir]->Draw( Form("%s",idir==0?"":"same") );
  leg->Draw("same");
  ccalotracklength_noBOT->cd(2)->SetLogy();   ccalotracklength_noBOT->cd(2)->SetTicks();
  for(int idir=0; idir<11; idir++) gacccalotracklength_noBOT[idir]->Draw( Form("%s",idir==0?"AP":"P") );
  gacccalotracklength_noBOT[0]->GetXaxis()->SetRangeUser(0,50);
  leg->Draw("same");

  TCanvas *ccalotracklength_TOPonly = new TCanvas("ccalotracklength_TOPonly","ccalotracklength_TOPonly");
  ccalotracklength_TOPonly->Divide(2,1);
  ccalotracklength_TOPonly->cd(1)->SetLogy();
  ccalotracklength_TOPonly->SetLogy();
  for(int idir=0; idir<11; idir++) hcalotracklength_TOPonly[idir]->Draw( Form("%s",idir==0?"":"same") );
  leg->Draw("same");
  ccalotracklength_TOPonly->cd(2)->SetLogy();   ccalotracklength_TOPonly->cd(2)->SetTicks();
  for(int idir=0; idir<11; idir++) gacccalotracklength_TOPonly[idir]->Draw( Form("%s",idir==0?"AP":"P") );
  gacccalotracklength_TOPonly[0]->GetXaxis()->SetRangeUser(0,50);
  leg->Draw("same");

  TCanvas *ccalotracklength_LATonly = new TCanvas("ccalotracklength_LATonly","ccalotracklength_LATonly");
  ccalotracklength_LATonly->Divide(2,1);
  ccalotracklength_LATonly->cd(1)->SetLogy();  ccalotracklength_LATonly->SetLogy();
  for(int idir=0; idir<11; idir++) hcalotracklength_LATonly[idir]->Draw( Form("%s",idir==0?"":"same") );
  leg->Draw("same");
  ccalotracklength_LATonly->cd(2)->SetLogy();   ccalotracklength_LATonly->cd(2)->SetTicks();
  for(int idir=0; idir<11; idir++) gacccalotracklength_LATonly[idir]->Draw( Form("%s",idir==0?"AP":"P") );
  gacccalotracklength_LATonly[0]->GetXaxis()->SetRangeUser(0,50);
  leg->Draw("same");

  TCanvas *ccalotracklength_LATnocorners = new TCanvas("ccalotracklength_LATnocorners","ccalotracklength_LATnocorners",1200,500);
  ccalotracklength_LATnocorners->Divide(2,1);
  ccalotracklength_LATnocorners->cd(1)->SetLogy();
  for(int idir=0; idir<11; idir++) hcalotracklength_LATnocorners[idir]->Draw( Form("%s",idir==0?"":"same") );
  leg->Draw("same");
  ccalotracklength_LATnocorners->cd(2)->SetLogy();   ccalotracklength_LATnocorners->cd(2)->SetTicks();
  for(int idir=0; idir<11; idir++) gacccalotracklength_LATnocorners[idir]->Draw( Form("%s",idir==0?"AP":"P") );
  gacccalotracklength_LATnocorners[0]->GetXaxis()->SetRangeUser(0,50);
  leg->Draw("same");

  return 0;
  
}


int verify_calo_fidvol(){

  int nentries=(int)t->GetEntries();
  int ngen=0;
  int nmctrackincalo=0;
  int nmctracknotfrombot=0;

  int nenebins = 36;
  Double_t *enebins = GenerateLogBinning(nenebins,10,10000);
  int nresbins = 100;
  Double_t *resbins = GenerateBinning(nresbins,0,1);
  int nleakbins = 100;
  Double_t *leakbins = GenerateLogBinning(nleakbins,1e-3,1);
  int ncalohitsbins = 100;
  Double_t *calohitsbins = GenerateLogBinning(ncalohitsbins,1,10000);
  
  
  TH1F *hcalotrack = new TH1F("hcalotrack","Track Length in CALO;X_0;Occurrence",500,0,100);
  TH1F *hcalotrack_fv = new TH1F("hcalotrack_fv","Track Length in CALO after Fid. Vol.;X_0;Occurrence",500,0,100);
  
  TH2F *heneleak = new TH2F("heneleak","Energy resolution;MC Momentum (GV); CALO Edep / MC Momentum",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_fv = new TH2F("heneleak_fv","Energy resolution;MC Momentum (GV); CALO Edep / MC Momentum",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_x0 = new TH2F("heneleak_x0","Energy resolution;MC Momentum (GV); CALO Edep / MC Momentum",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_x0_fv = new TH2F("heneleak_x0_fv","Energy resolution;MC Momentum (GV); CALO Edep / MC Momentum",nenebins, enebins, nleakbins, leakbins);

  TH3F *heneleakhits = new TH3F("heneleakhits","heneleakhits",nenebins, enebins, nleakbins, leakbins, ncalohitsbins, calohitsbins);

  TH2F *hcalohits = new TH2F("hcalohits",";MC Momentum (GV);CALO hits",nenebins, enebins,ncalohitsbins, calohitsbins);
  TH2F *hcalohits_eneleak = new TH2F("hcalohits_eneleak","1 -(CALO Edep / MC Momentum) > 0.9;MC Momentum (GV);CALO hits",nenebins, enebins,ncalohitsbins, calohitsbins);
  
  //---------LOOP on events
  for(int ientry=0; ientry<nentries; ientry++){
    if(ientry%100000==0) printf("%d/%d\n",ientry,nentries);
    
    t->GetEntry(ientry);

    ngen+=1+mcNdiscarded;

    if(mcTrackcaloentryplane<0 && mcTrackcaloexitplane<0) continue;
    nmctrackincalo++;

    if(mcTrackcaloentryplane == 5) continue; //Not entry from Zneg
    nmctracknotfrombot++;

    hcalotrack->Fill(mcTracklengthcalox0);
    if(calofidvolpass) hcalotrack_fv->Fill(mcTracklengthcalox0);

    heneleak->Fill(mcMom,1-calototedep/mcMom);
    if(calofidvolpass) heneleak_fv->Fill(mcMom,1-calototedep/mcMom);
    if(mcTracklengthcalox0>10) heneleak_x0->Fill(mcMom,1-calototedep/mcMom);
    if(mcTracklengthcalox0>10 && calofidvolpass) heneleak_x0_fv->Fill(mcMom,1-calototedep/mcMom);

    if( !(mcTracklengthcalox0>10 && calofidvolpass) ) continue;
    heneleakhits->Fill(mcMom,1-calototedep/mcMom,calonhits);

    hcalohits->Fill(mcMom,calonhits);
    if( (1-calototedep/mcMom)>0.9 ){
      hcalohits_eneleak->Fill(mcMom,calonhits);
    }
    
  }

  TCanvas *ccalotrackx0 = new TCanvas("ccalotrackx0","ccalotrackx0");
  ccalotrackx0->Divide(2,1);
  ccalotrackx0->cd(1)->SetLogy();
  hcalotrack->Draw("");
  hcalotrack_fv->Draw("same");
  ccalotrackx0->cd(2)->SetTicks();  ccalotrackx0->cd(2)->SetGrid();
  TGraphAsymmErrors *gcalotrackratio = new TGraphAsymmErrors(); gcalotrackratio->Divide(hcalotrack_fv,hcalotrack);
  gcalotrackratio->Draw("AP");
  gcalotrackratio->GetXaxis()->SetTitle(hcalotrack->GetXaxis()->GetTitle());
  gcalotrackratio->GetYaxis()->SetTitle("Fraction");

  TCanvas *ceneres = new TCanvas("ceneres","ceneres");
  ceneres->Divide(2,1);
  ceneres->cd(1)->SetTicks();   ceneres->cd(1)->SetLogx();
  heneleak->Draw("COLZ");
  ceneres->cd(2);
  for(int ii=0; ii<9; ii++){
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    TH1 *h = heneleak->ProjectionY( Form("heneleak_%d",ii), binstart, binstop );
    h->DrawNormalized( Form("%s", ii==1 ? "" : "same" ) );
  }

  TCanvas *ceneres2 = new TCanvas("ceneres2","ceneres2");
  ceneres2->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ceneres2->cd(ii+1)->SetLogx();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    TH1 *h = heneleak->ProjectionY( Form("heneleak_%d",ii), binstart,binstop);
    h->SetLineColor(kBlack); h->SetFillColor(kGray);
    h->DrawNormalized("");
    TH1 *h_x0 = heneleak_x0->ProjectionY( Form("heneleak_x0_%d",ii), binstart,binstop);
    h_x0->SetLineColor(kBlue+1); h_x0->SetFillColor(kBlue-10); 
    h_x0->DrawNormalized("same");
    TH1 *h_x0_fv = heneleak_x0_fv->ProjectionY( Form("heneleak_x0_fv_%d",ii), binstart,binstop);
    h_x0_fv->SetLineColor(kRed+1);     h_x0_fv->SetFillColor(kRed-10); 
    h_x0_fv->DrawNormalized("same");
  }

  TCanvas *ceneres3 = new TCanvas("ceneres3","ceneres3");
  ceneres3->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ceneres3->cd(ii+1)->SetLogx();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    TH1 *h = heneleak->ProjectionY( Form("heneleak_%d",ii), binstart,binstop);
    h->SetLineColor(kBlack); h->SetFillColor(kGray);
    h->DrawNormalized("");
    TH1 *h_fv = heneleak_x0->ProjectionY( Form("heneleak_x0_%d",ii), binstart,binstop);
    h_fv->SetLineColor(kGreen+2); h_fv->SetFillColor(kGreen-10); 
    h_fv->DrawNormalized("same");
    TH1 *h_x0_fv = heneleak_x0_fv->ProjectionY( Form("heneleak_x0_fv_%d",ii), binstart,binstop);
    h_x0_fv->SetLineColor(kRed+1);     h_x0_fv->SetFillColor(kRed-10); 
    h_x0_fv->DrawNormalized("same");
  }

  TCanvas *cenerescalohits = new TCanvas("cenerescalohits","cenerescalohits");
  cenerescalohits->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    cenerescalohits->cd(ii+1)->SetLogx();     cenerescalohits->cd(ii+1)->SetLogy();     cenerescalohits->cd(ii+1)->SetLogz();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    heneleakhits->GetXaxis()->SetRange(binstart,binstop);
    TH2 *h = (TH2*)heneleakhits->Project3D( "yz" );
    h->SetName( Form("heneleakhits_yz_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("CALO hits");
    h->GetYaxis()->SetTitle("CALO energy leak (fraction)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  TCanvas *ccalohits = new TCanvas("ccalohits","ccalohits");
  ccalohits->Divide(2,1);
  ccalohits->cd(1)->SetTicks(); ccalohits->cd(1)->SetLogx(); ccalohits->cd(1)->SetLogy(); ccalohits->cd(1)->SetLogz();
  hcalohits->Draw("COLZ");   hcalohits->SetStats(0);
  ccalohits->cd(2)->SetTicks(); ccalohits->cd(2)->SetLogx(); ccalohits->cd(2)->SetLogy(); ccalohits->cd(2)->SetLogz();
  hcalohits_eneleak->Draw("COLZ"); hcalohits_eneleak->SetStats(0);
  TF1 *fcut = new TF1("ff","pow(10,([0]*log10(x)+[1]))",10,10000); ff->SetParameter(0, (1+log10(2))/3. ); ff->SetParameter(1, 2 - (1+log10(2))/3. );
  fcut->SetLineColor(kBlue+1);
  fcut->Draw("same");
  
  return 0;

}
