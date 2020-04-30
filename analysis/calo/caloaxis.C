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
#include "TVector3.h"
#include "TLine.h"
#include "TObjArray.h"

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

short calofidvolxpos;
short calofidvolxneg;
short calofidvolypos;
short calofidvolyneg;
short calofidvolzpos;
short calofidvolzneg;
short calofidvolxnegyneg;
short calofidvolxposyneg;
short calofidvolxnegypos;
short calofidvolxposypos;

int calonhits;      
float calototedep;

float caloaxiscog[3];
float caloaxisdir[3];
float caloaxiseigval[3];
float caloaxiseigvec[3][3];

const float sphrad = 3;// m^2
const float sphacc = TMath::Pi() * (4*TMath::Pi()*pow(sphrad,2)); //see Sullivan

TObjArray *arrCanvas;
TObjArray *arrGraphs;

const char* facename[10] = {"Xpos","Xneg","Ypos","Yneg","Zpos","Zneg","XnegYneg","XposYneg","XnegYpos","XposYpos"};

TF1 *fcalohitscut;

int verify_calo_borders();
int verify_calo_fidvol();
int verify_calo_axis( double x0);

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

TH2F* NormalizeX(TH2F* hh){    // Get pointer to TH2F normalized in X slices

  std::string hname; hname.assign( Form("%s_normX", hh->GetName() ) );
  if( gDirectory->Get( hname.c_str() ) )
    {
      std::cout<<Form("NormalizeX::Deleting old %s",hname.c_str() )<<std::endl;
      gDirectory->Get( hname.c_str() ) ->Delete();
    }

  TH2F *hNorm = (TH2F*)hh->Clone( Form("%s_normX", hh->GetName() ) );
  hNorm->Sumw2();  

  Double_t nentries;
  Double_t value;
  for(int ibinx=1; ibinx<=hNorm->GetNbinsX(); ibinx++)
    {
      nentries=0;
      for(int ibiny=1; ibiny<=hNorm->GetNbinsY(); ibiny++)
	{
	  nentries+=hNorm->GetBinContent(ibinx,ibiny);
	}
      for(int ibiny=1; ibiny<=hNorm->GetNbinsY(); ibiny++)
	{
	  if( nentries<=0 ) continue;
	  value = hNorm->GetBinContent(ibinx,ibiny);
	  value /= nentries;
	  hNorm->SetBinContent(ibinx,ibiny,value);
	}
    }
  return hNorm;
}



int caloaxis( const char* filename, int save=1, const double x0=30 ){

  //TFile *f = TFile::Open("tree.electrons.root");
  //TFile *f = TFile::Open("electrons_sphere_10GeV_10000GeV_E-1.dig.tree.root");
  TFile *f = TFile::Open( filename );
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

  t->SetBranchAddress("calofidvolxpos",&calofidvolxpos);
  t->SetBranchAddress("calofidvolxneg",&calofidvolxneg);
  t->SetBranchAddress("calofidvolypos",&calofidvolypos);
  t->SetBranchAddress("calofidvolyneg",&calofidvolyneg);
  t->SetBranchAddress("calofidvolzpos",&calofidvolzpos);
  t->SetBranchAddress("calofidvolzneg",&calofidvolzneg);
  t->SetBranchAddress("calofidvolxnegyneg",&calofidvolxnegyneg);
  t->SetBranchAddress("calofidvolxposyneg",&calofidvolxposyneg);
  t->SetBranchAddress("calofidvolxnegypos",&calofidvolxnegypos);
  t->SetBranchAddress("calofidvolxposypos",&calofidvolxposypos);

  t->SetBranchAddress("calonhits",&calonhits);
  t->SetBranchAddress("calototedep",&calototedep);

  t->SetBranchAddress("caloaxiscog",&caloaxiscog);
  t->SetBranchAddress("caloaxisdir",&caloaxisdir);
  t->SetBranchAddress("caloaxisdir",&caloaxisdir);;
  t->SetBranchAddress("caloaxiseigval", &caloaxiseigval);
  t->SetBranchAddress("caloaxiseigvec", &caloaxiseigvec);

  fcalohitscut = new TF1("ff","pow(10,([0]*log10(x)+[1]))",10,10000);
  double fcalohitscut_m = (log10(1500)-log10(100))/(log10(10000)-log10(10));
  double fcalohitscut_q = log10(100) - fcalohitscut_m*log10(10);
  fcalohitscut->SetParameter(0, fcalohitscut_m ); fcalohitscut->SetParameter(1, fcalohitscut_q );

  arrCanvas = new TObjArray();
  arrGraphs = new TObjArray();

  TFile *fout = 0;
  if( save>0 ) { fout = new TFile("fout.root","recreate"); fout->cd(); }
  
  verify_calo_axis(x0);
  // verify_calo_borders();
  //verify_calo_fidvol();

  for(int ic=0; ic<arrCanvas->GetEntries(); ic++){
    TCanvas *c = (TCanvas*)arrCanvas->At(ic);
    c->SaveAs( Form("Plots/%s.root",c->GetName() ) );
    c->SaveAs( Form("Plots/%s.C",c->GetName() ) );
    if( ic==0 ) c->SaveAs( Form("Plots/Plots.pdf(" ) );
    else if( ic==arrCanvas->GetEntries()-1 ) c->SaveAs( Form("Plots/Plots.pdf)") );
    else c->SaveAs( Form("Plots/Plots.pdf") );
  }

  if( save>0 ){
  fout->cd();
  fout->Write();
  for(int ig=0; ig<arrGraphs->GetEntries(); ig++){ TGraph *g = (TGraph*)arrGraphs->At(ig); g->Write(); }
  fout->Close();
  }
  
  return 0;

}

int verify_calo_axis(const double x0){

  int nentries=(int)t->GetEntries();
  int ngen=0;
  int nmctrackincalo=0;

  int nenebins = 36;
  Double_t *enebins = GenerateLogBinning(nenebins,10,10000);
  int nthetabins = 200;
  Double_t *thetabins = GenerateBinning(nthetabins,0,+TMath::Pi());
  int nphibins = 200;
  Double_t *phibins = GenerateBinning(nphibins,-TMath::Pi(),+TMath::Pi());
  int ndiffbins = 200;
  Double_t *diffbins = GenerateBinning(ndiffbins,0,+0.25 * TMath::RadToDeg());
  int ndirbins = 100;
  Double_t *dirbins = GenerateBinning(ndirbins,-1,+1);
  int nfacebins = 10;
  Double_t *facebins = GenerateBinning(nfacebins,-0.5,9.5);
  int nxybins = 200;
  Double_t *xybins = GenerateBinning(nxybins,-45,+45);
  int nzbins = 170;
  Double_t *zbins = GenerateBinning(nzbins,-80,+5);
  int ncalotothitsbins=100;
  Double_t *calotothitsbins = GenerateLogBinning(ncalotothitsbins,20,8000);
  
  TH2F *hthetaall = new TH2F("hthetaall","hthetaall;Theta [MC];Theta [CALO]", nthetabins, thetabins, nphibins, phibins);
  TH2F *hphiall   = new TH2F("hphiall",  "hphiall;Phi [MC];Phi [CALO]", nthetabins, thetabins, nphibins, phibins);
  TH1F *hdiffall  = new TH1F("hdiffall", "hdiffall;Angle([MC]-[CALO])", ndiffbins, diffbins);

  TH3F *htheta    = new TH3F("htheta","htheta;Theta [MC];Theta [CALO]",nenebins, enebins, nthetabins, thetabins, nthetabins, thetabins);
  TH3F *hphi      = new TH3F("hphi",  "hphi;Phi [MC];Phi [CALO]",nenebins, enebins, nphibins, phibins, nphibins, phibins);
  TH3F *hdir[3]; for(int i=0; i<3; i++) hdir[i] = new TH3F( Form("hdir_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nenebins, enebins, ndirbins, dirbins, ndirbins, dirbins);

  TH3F *hthetaface    = new TH3F("hthetaface","hthetaface;Theta [MC];Theta [CALO]",nfacebins, facebins, nthetabins, thetabins, nthetabins, thetabins);
  TH3F *hphiface      = new TH3F("hphiface",  "hphiface;Phi [MC];Phi [CALO]",nfacebins, facebins, nphibins, phibins, nphibins, phibins);
  TH3F *hdirface[3]; for(int i=0; i<3; i++) hdirface[i] = new TH3F( Form("hdirface_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nfacebins, facebins, ndirbins, dirbins, ndirbins, dirbins);

  TH3F *hthetadiff    = new TH3F("hthetadiff","hthetadiff;Theta [MC];Theta [CALO]",nenebins, enebins, nthetabins, thetabins, ndiffbins, diffbins);
  TH3F *hphidiff      = new TH3F("hphidiff",  "hphidiff;Phi [MC];Phi [CALO]",nenebins, enebins, nphibins, phibins, ndiffbins, diffbins);
  TH3F *hdirdiff[3]; for(int i=0; i<3; i++) hdirdiff[i] = new TH3F( Form("hdirdiff_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nenebins, enebins, ndirbins, dirbins, ndiffbins, diffbins);
  TH3F *hcalotothitsdiff    = new TH3F("hcalotothitsdiff","hcalotothitsdiff;Calotothits [MC];Calotothits [CALO]",nenebins, enebins, ncalotothitsbins, calotothitsbins, ndiffbins, diffbins);
    
  TH3F *hcogxy    = new TH3F("hcogxy","hcogxy;",nenebins, enebins, nxybins, xybins, nxybins, xybins);
  TH3F *hcogxz    = new TH3F("hcogxz","hcogxz;",nenebins, enebins, nxybins, xybins, nzbins, zbins);
  TH3F *hcogyz    = new TH3F("hcogyz","hcogyz;",nenebins, enebins, nxybins, xybins, nzbins, zbins);
  
  TH2F *hdiff     = new TH2F("hdiff", "hdiff", nenebins, enebins, ndiffbins, diffbins);
  TH2F *hdiffface     = new TH2F("hdiffface", "hdiffface", nfacebins, facebins, ndiffbins, diffbins);


  
  // ---------LOOP on events
  // nentries=10000;
  for(int ientry=0; ientry<nentries; ientry++){
    if(ientry%100000==0) printf("%d/%d\n",ientry,nentries);
    
    t->GetEntry(ientry);

    ngen+=1+mcNdiscarded;

    if(mcTrackcaloentryplane<0 && mcTrackcaloexitplane<0) continue;
    if(mcTrackcaloentryplane == 5) continue; //Not entry from Zneg
    //if(mcTrackcaloentryplane != 0) continue; //Not entry from Zneg
    if(!calofidvolpass) continue;
    if(mcTracklengthcalox0<0) continue;
    if(x0>=100){
      if (mcTracklengthcalox0<50) continue; }
    else{
      if(mcTracklengthcalox0<x0-10 || mcTracklengthcalox0>=x0) continue; }
    //if(mcTracklengthcalox0<x0) continue;
    double calohitscut = fcalohitscut->Eval(mcMom);
    if( calonhits<calohitscut ) continue;

    //if( 1-(calototedep/mcMom)>0.02 ) continue;
    
    // if( caloaxisdir[2]*mcDir[2] < 0 ){
    //   caloaxisdir[0] *=-1;
    //   caloaxisdir[1] *=-1;
    //   caloaxisdir[2] *=-1;
    // }
    
    // if( caloaxisdir[0]*mcDir[0] < 0 ){
    // caloaxisdir[0] *=-1;
    // }

    // if( caloaxisdir[1]*mcDir[1] < 0 ){
    // caloaxisdir[1] *=-1;
    // }

    
    TVector3 mcvec(mcDir[0],mcDir[1],mcDir[2]);
    TVector3 calovec(caloaxisdir[0],caloaxisdir[1],caloaxisdir[2]);

    //float angdiff = mcvec.Angle(calovec); 
    //angdiff = atan(tan(acos( mcDir[0]*caloaxisdir[0] + mcDir[1]*caloaxisdir[1] + mcDir[2]*caloaxisdir[2] )));
    float angdiff = acos( mcDir[0]*caloaxisdir[0] + mcDir[1]*caloaxisdir[1] + mcDir[2]*caloaxisdir[2]);
    //if(angdiff>TMath::Pi()/2) angdiff-=TMath::Pi();
    //if(angdiff<TMath::Pi()/2) continue;
    if(angdiff>TMath::Pi()/2) angdiff-=TMath::Pi();
    angdiff = fabs(angdiff);

    float theta = acos(caloaxisdir[2]);
    float phi   = atan2(caloaxisdir[1],caloaxisdir[0]);
    float mcTheta = acos(mcCtheta);
    
    angdiff *= TMath::RadToDeg();
    hthetaall->Fill( mcTheta, theta );
    hphiall->Fill( mcPhi, phi );
    hdiffall->Fill( angdiff );

    hcogxy->Fill( mcMom, caloaxiscog[0], caloaxiscog[1] );
    hcogxz->Fill( mcMom, caloaxiscog[0], caloaxiscog[2] );
    hcogyz->Fill( mcMom, caloaxiscog[1], caloaxiscog[2] );

    hcalotothitsdiff->Fill( mcMom, calonhits, angdiff );
    
    for(int idir=0; idir<3; idir++) hdir[idir]->Fill(mcMom, mcDir[idir], caloaxisdir[idir]);
    htheta->Fill( mcMom, mcTheta, theta );
    hphi->Fill( mcMom, mcPhi, phi);
    hdiff->Fill( mcMom, angdiff);

    for(int idir=0; idir<3; idir++) hdirdiff[idir]->Fill(mcMom, mcDir[idir], angdiff );
    hthetadiff->Fill( mcMom, mcTheta, angdiff );
    hphidiff->Fill( mcMom, mcPhi, angdiff);

    
    
    if( mcMom>50 && mcMom<100){
    for(int idir=0; idir<3; idir++) hdirface[idir]->Fill(mcTrackcaloentryplane, mcDir[idir], caloaxisdir[idir]);
    hthetaface->Fill( mcTrackcaloentryplane, mcTheta, theta );
    hphiface->Fill( mcTrackcaloentryplane, mcPhi, phi);
    hdiffface->Fill( mcTrackcaloentryplane, angdiff);
    }
  }
  
  TCanvas *cthetaall = new TCanvas("cthetaall","cthetaall"); cthetaall->cd(1)->SetTicks(); cthetaall->cd(1)->SetLogz(); arrCanvas->Add(cthetaall);
  hthetaall->Draw("COLZ");
  TCanvas *cphiall = new TCanvas("cphiall","cphiall"); cphiall->cd(1)->SetTicks(); cphiall->cd(1)->SetLogz(); arrCanvas->Add(cphiall);
  hphiall->Draw("COLZ");
  TCanvas *cdiffall = new TCanvas("cdiffall","cdiffall"); cdiffall->cd(1)->SetTicks(); cdiffall->cd(1)->SetLogz(); arrCanvas->Add(cdiffall);
  hdiffall->Draw("");
  
  TCanvas *cdir[3]; for(int idir=0; idir<3; idir++){
    cdir[idir]= new TCanvas(Form("cdir[%d]",idir), Form("cdir[%d]",idir) ); arrCanvas->Add(cdir[idir]);
    cdir[idir]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      cdir[idir]->cd(ii+1)->SetLogz(); cdir[idir]->cd(ii+1)->SetTicks();
      int binstart = ii*4+1; int binstop  = ii*4+4;
      hdir[idir]->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hdir[idir]->Project3D( "zy" ); h->SetName( Form("hdir_%d_zy_%d",idir,ii) );
      h->Draw("COLZ"); h->SetStats(0);
      h->GetXaxis()->SetTitle( Form("Dir[%d] [MC]", idir) );
      h->GetYaxis()->SetTitle( Form("Dir[%d] [CALO]", idir) );
      h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
    }
  }

  TCanvas *cdirface[3]; for(int idir=0; idir<3; idir++){
    cdirface[idir]= new TCanvas(Form("cdirface[%d]",idir), Form("cdirface[%d]",idir), 1000, 500 ); arrCanvas->Add(cdirface[idir]);
    cdirface[idir]->Divide(5,2);
    for(int ii=0; ii<10; ii++){
      cdirface[idir]->cd(ii+1)->SetLogz(); cdirface[idir]->cd(ii+1)->SetTicks();
      int binstart = ii+1; int binstop  = ii+1;
      hdirface[idir]->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hdirface[idir]->Project3D( "zy" ); h->SetName( Form("hdirface_%d_zy_%d",idir,ii) );
      h->Draw("COLZ"); h->SetStats(0);
      h->GetXaxis()->SetTitle( Form("Dir[%d] [MC]", idir) );
      h->GetYaxis()->SetTitle( Form("Dir[%d] [CALO]", idir) );
      h->SetTitle( Form("%s [50 - 100] GV (MC)", facename[ii]) );
    }
  }

    TCanvas *cdirdiff[3]; for(int idir=0; idir<3; idir++){
    cdirdiff[idir]= new TCanvas(Form("cdirdiff[%d]",idir), Form("cdirdiff[%d]",idir) ); arrCanvas->Add(cdirdiff[idir]);
    cdirdiff[idir]->Divide(3,3);
    for(int ii=0; ii<9; ii++){
      cdirdiff[idir]->cd(ii+1)->SetLogz(); cdirdiff[idir]->cd(ii+1)->SetTicks();
      int binstart = ii*4+1; int binstop  = ii*4+4;
      hdirdiff[idir]->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hdirdiff[idir]->Project3D( "zy" ); h->SetName( Form("hdirdiff_%d_zy_%d",idir,ii) );
      h->Draw("COLZ"); h->SetStats(0);
      h->GetXaxis()->SetTitle( Form("Dir[%d] [MC]", idir) );
      h->GetYaxis()->SetTitle( Form("Angle [MC - CALO]") );
      h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
    }
  }

  
  TCanvas *ctheta = new TCanvas("ctheta","ctheta"); arrCanvas->Add(ctheta);
  ctheta->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ctheta->cd(ii+1)->SetLogz(); ctheta->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    htheta->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)htheta->Project3D( "zy" ); h->SetName( Form("htheta_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Theta [MC]");
    h->GetYaxis()->SetTitle("Theta [CALO]");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  
  
  TCanvas *ccogxy = new TCanvas("ccogxy","ccogxy"); arrCanvas->Add(ccogxy);
  ccogxy->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ccogxy->cd(ii+1)->SetLogz(); ccogxy->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    hcogxy->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hcogxy->Project3D( "zy" ); h->SetName( Form("hcogxy_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("COG [X] (cm)");
    h->GetYaxis()->SetTitle("COG [Y] (cm)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  TCanvas *ccogxz = new TCanvas("ccogxz","ccogxz"); arrCanvas->Add(ccogxz);
  ccogxz->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ccogxz->cd(ii+1)->SetLogz(); ccogxz->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    hcogxz->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hcogxz->Project3D( "zy" ); h->SetName( Form("hcogxz_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("COG [X] (cm)");
    h->GetYaxis()->SetTitle("COG [Z] (cm)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  TCanvas *ccogyz = new TCanvas("ccogyz","ccogyz"); arrCanvas->Add(ccogyz);
  ccogyz->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ccogyz->cd(ii+1)->SetLogz(); ccogyz->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    hcogyz->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hcogyz->Project3D( "zy" ); h->SetName( Form("hcogyz_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("COG [Y] (cm)");
    h->GetYaxis()->SetTitle("COG [Z] (cm)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  
  TCanvas *cthetaface = new TCanvas("cthetaface","cthetaface",1000,400); arrCanvas->Add(cthetaface);
  cthetaface->Divide(5,2);
  for(int ii=0; ii<10; ii++){
    cthetaface->cd(ii+1)->SetLogz(); cthetaface->cd(ii+1)->SetTicks();
    int binstart = ii+1; int binstop  = ii+1;
    hthetaface->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hthetaface->Project3D( "zy" ); h->SetName( Form("hthetaface_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Theta [MC]");
    h->GetYaxis()->SetTitle("Theta [CALO]");
    h->SetTitle( Form("%s [50 - 100] GV (MC)", facename[ii]) );
  }

    TCanvas *cthetadiff = new TCanvas("cthetadiff","cthetadiff",1000,400); arrCanvas->Add(cthetadiff);
  cthetadiff->Divide(5,2);
  for(int ii=0; ii<10; ii++){
    cthetadiff->cd(ii+1)->SetLogz(); cthetadiff->cd(ii+1)->SetTicks();
    int binstart = ii+1; int binstop  = ii+1;
    hthetadiff->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hthetadiff->Project3D( "zy" ); h->SetName( Form("hthetadiff_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Theta [MC]");
    h->GetYaxis()->SetTitle("Angle [MC - CALO]");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  
  TCanvas *cphi = new TCanvas("cphi","cphi"); arrCanvas->Add(cphi);
  cphi->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    cphi->cd(ii+1)->SetLogz(); cphi->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    hphi->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hphi->Project3D( "zy" ); h->SetName( Form("hphi_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Phi [MC]");
    h->GetYaxis()->SetTitle("Phi [CALO]");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  TCanvas *cphiface = new TCanvas("cphiface","cphiface");  cphiface->Divide(5,2);  arrCanvas->Add(cphiface);
  for(int ii=0; ii<10; ii++){
    cphiface->cd(ii+1)->SetLogz(); cphiface->cd(ii+1)->SetTicks();
    int binstart = ii+1; int binstop  = ii+1;
    hphiface->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hphiface->Project3D( "zy" ); h->SetName( Form("hphiface_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Phi [MC]");
    h->GetYaxis()->SetTitle("Phi [CALO]");
    h->SetTitle( Form("%s [50 - 100] GV (MC)", facename[ii]) );
  }

    TCanvas *cphidiff = new TCanvas("cphidiff","cphidiff");  cphidiff->Divide(5,2);  arrCanvas->Add(cphidiff);
    for(int ii=0; ii<10; ii++){
    cphidiff->cd(ii+1)->SetLogz(); cphidiff->cd(ii+1)->SetTicks();
    int binstart = ii+1; int binstop  = ii+1;
    hphidiff->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hphidiff->Project3D( "zy" ); h->SetName( Form("hphidiff_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("Phi [MC]");
    h->GetYaxis()->SetTitle("Angle [MC - CALO]");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  
  TGraphErrors *gdiff68 = new TGraphErrors(); gdiff68->SetNameTitle("gdiff68","gdiff68"); arrGraphs->Add(gdiff68);
  TGraphErrors *gdiff95 = new TGraphErrors(); gdiff95->SetNameTitle("gdiff95","gdiff95"); arrGraphs->Add(gdiff95);
  TCanvas *cdiff = new TCanvas("cdiff","cdiff"); arrCanvas->Add(cdiff);
  cdiff->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    cdiff->cd(ii+1)->SetLogy(); cdiff->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    TH1 *h = (TH1*)hdiff->ProjectionY( Form("hdiff_%d", ii), binstart, binstop );
    h->Draw(""); h->SetStats(0);
    h->GetXaxis()->SetTitle("Angle [MC - CALO] (deg)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
    // TF1 *f = new TF1("f","[0] * ( (1/sqrt([1])) * exp( -(x*x)/(2*[1]*[1])) * x) + [2] * ( (1/sqrt([3])) * exp( -(x*x)/(2*[3]*[3])) * x)",0,10);
    // f->SetParameter(0,h->GetEntries());
    // f->SetParameter(1,h->GetRMS());
    // f->SetParameter(2,h->GetEntries()*0.01);
    // f->SetParameter(3,h->GetRMS()*2);
    // for(int i=0; i<10; i++) h->Fit(f,"","",0,10);
    

    double tot = h->Integral();
    double perc68=-1, perc95=-1;
    for(int ibinx=1; ibinx<h->GetNbinsX(); ibinx++){
      double frac = h->Integral(1, ibinx) / tot;
      //printf("%f %f\n", frac, tot);
      if( frac>0.68 && perc68<0 ){ perc68 = h->GetBinLowEdge(ibinx+1); }
      if( frac>0.95 && perc95<0 ){ perc95 = h->GetBinLowEdge(ibinx+1); }
    }
    gdiff68->SetPoint( gdiff68->GetN(), sqrt(enebins[binstart-1]*enebins[binstart+3]), perc68);
    gdiff68->SetPointError( gdiff68->GetN()-1, 0, h->GetBinWidth(1));
    gdiff95->SetPoint( gdiff95->GetN(), sqrt(enebins[binstart-1]*enebins[binstart+3]), perc95);
    gdiff95->SetPointError( gdiff95->GetN()-1, 0, h->GetBinWidth(1));
    TLine *l68 = new TLine(perc68,1,perc68,h->GetMaximum()); l68->SetLineColor(kRed+1);  l68->SetLineStyle(2); l68->Draw("same");
    TLine *l95 = new TLine(perc95,1,perc95,h->GetMaximum()); l95->SetLineColor(kBlue+1); l95->SetLineStyle(2); l95->Draw("same");
  }
  TCanvas *cdiffgraph = new TCanvas("cdiffgraph","cdiffgraph"); cdiffgraph->cd(1)->SetTicks(); cdiffgraph->cd(1)->SetLogx(); arrCanvas->Add(cdiffgraph);
  TH2F *h2 = new TH2F("h2",";Energy (GeV) [MC];Angular Resolution (deg)",100,10,10000,100,0,0.1*TMath::RadToDeg()); h2->SetStats(0); h2->Draw("");
  gdiff68->Draw("P"); gdiff68->SetLineColor(kRed+1); gdiff68->SetMarkerColor(kRed+1);
  gdiff95->Draw("P"); gdiff95->SetLineColor(kBlue+1); gdiff95->SetMarkerColor(kBlue+1);
  
  
  TCanvas *cdiffface = new TCanvas("cdiffface","cdiffface",1000,400); arrCanvas->Add(cdiffface);
  cdiffface->Divide(5,2);
   for(int ii=0; ii<10; ii++){
     cdiffface->cd(ii+1)->SetLogy(); cdiffface->cd(ii+1)->SetTicks();
     int binstart = ii+1; int binstop  = ii+1;
     TH1 *h = (TH1*)hdiffface->ProjectionY( Form("hdiffface_%d", ii), binstart, binstop );
     h->Draw("");
     h->GetXaxis()->SetTitle("Angle [MC - CALO]");
     h->SetTitle( Form("%s [50 - 100] GV (MC)", facename[ii]) );
   }

   TCanvas *ccalotothitsdiff = new TCanvas("ccalotothitsdiff","ccalotothitsdiff"); arrCanvas->Add(ccalotothitsdiff);
   ccalotothitsdiff->Divide(3,3);
   for(int ii=0; ii<9; ii++){
    ccalotothitsdiff->cd(ii+1)->SetLogz(); ccalotothitsdiff->cd(ii+1)->SetLogx();  ccalotothitsdiff->cd(ii+1)->SetTicks();
    int binstart = ii*4+1; int binstop  = ii*4+4;
    hcalotothitsdiff->GetXaxis()->SetRange(binstart,binstop); TH2 *h = (TH2*)hcalotothitsdiff->Project3D( "zy" ); h->SetName( Form("hcalotothitsdiff_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("CALO hits");
    h->GetYaxis()->SetTitle("Angle [MC - CALO]");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

   
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
  int nmctrackcalox0=0;
  int ncalofidvolpass=0;
  int npasscalohits=0;
  
  int nenebins = 36;
  Double_t *enebins = GenerateLogBinning(nenebins,10,10000);
  int nresbins = 100;
  Double_t *resbins = GenerateBinning(nresbins,0,1);
  int nleakbins = 600;
  Double_t *leakbins = GenerateLogBinning(nleakbins,0.9,1);
  int ncalohitsbins = 1785;
  Double_t *calohitsbins = GenerateBinning(ncalohitsbins,10.5,7140.5);
  
  
  TH1F *hcalotrack = new TH1F("hcalotrack","Track Length in CALO;X_0;Occurrence",500,0,100);
  TH1F *hcalotrack_fv = new TH1F("hcalotrack_fv","Track Length in CALO after Fid. Vol.;X_0;Occurrence",500,0,100);
  TH1F *hcalotrack_fv_entryxy = new TH1F("hcalotrack_fv_entryxy","Track Length in CALO after Fid. Vol.;X_0;Occurrence",500,0,100);
  TH1F *hcalotrack_fv_entrzy = new TH1F("hcalotrack_fv_entrzy","Track Length in CALO after Fid. Vol.;X_0;Occurrence",500,0,100);
  TH1F *hcalotrack_fv_entrylat = new TH1F("hcalotrack_fv_entrylat","Track Length in CALO after Fid. Vol.;X_0;Occurrence",500,0,100);


  TH2F *heneleak = new TH2F("heneleak","Energy resolution;MC Momentum (GV);1 - (CALO Edep / MC Momentum)",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_fv = new TH2F("heneleak_fv","Energy resolution;MC Momentum (GV);1 - (CALO Edep / MC Momentum)",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_x0 = new TH2F("heneleak_x0","Energy resolution;MC Momentum (GV);1 - (CALO Edep / MC Momentum)",nenebins, enebins, nleakbins, leakbins);
  TH2F *heneleak_x0_fv = new TH2F("heneleak_x0_fv","Energy resolution;MC Momentum (GV);1 - (CALO Edep / MC Momentum)",nenebins, enebins, nleakbins, leakbins);

  TH3F *heneleakhits = new TH3F("heneleakhits","heneleakhits",nenebins, enebins, nleakbins, leakbins, ncalohitsbins, calohitsbins);

  TH2F *hcalohits = new TH2F("hcalohits",";MC Momentum (GV);CALO hits",nenebins, enebins,ncalohitsbins, calohitsbins);
  TH2F *hcalohits_eneleak = new TH2F("hcalohits_eneleak","1 -(CALO Edep / MC Momentum) > 0.9;MC Momentum (GV);CALO hits",nenebins, enebins,ncalohitsbins, calohitsbins);
  TH2F *hcalohits_pass = new TH2F("hcalohits_pass","1 -(CALO Edep / MC Momentum) > 0.9;MC Momentum (GV);CALO hits",nenebins, enebins,ncalohitsbins, calohitsbins);


  
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
    if( calofidvolxneg==1 || calofidvolxpos==1 || calofidvolyneg==1 || calofidvolypos==1 ) hcalotrack_fv_entryxy->Fill(mcTracklengthcalox0);
    if( calofidvolzneg==1 || calofidvolzpos==1 ) hcalotrack_fv_entrzy->Fill(mcTracklengthcalox0);
    if( calofidvolxnegyneg==1 || calofidvolxposyneg==1 || calofidvolxnegypos==1 || calofidvolxposypos==1 ) hcalotrack_fv_entrylat->Fill(mcTracklengthcalox0);
    if(calofidvolpass) {
      hcalotrack_fv->Fill(mcTracklengthcalox0);
    }
      
    heneleak->Fill(mcMom,1-calototedep/mcMom);
    if(calofidvolpass) heneleak_fv->Fill(mcMom,1-calototedep/mcMom);
    if(mcTracklengthcalox0>10) heneleak_x0->Fill(mcMom,1-calototedep/mcMom);
    if(mcTracklengthcalox0>10 && calofidvolpass) heneleak_x0_fv->Fill(mcMom,1-calototedep/mcMom);

    if( mcTracklengthcalox0<10 ) continue;
    nmctrackcalox0++;

    if( !( calofidvolpass) ) continue;
    heneleakhits->Fill(mcMom,1-calototedep/mcMom,calonhits);
    ncalofidvolpass++;

    hcalohits->Fill(mcMom,calonhits);
    if( (1-calototedep/mcMom)>0.9 ){
      hcalohits_eneleak->Fill(mcMom,calonhits);
    }

    
    double calohitscut = fcalohitscut->Eval(mcMom);
    if( calonhits<calohitscut ) continue;
    hcalohits_pass->Fill(mcMom,calonhits);
    npasscalohits++;
    
  }

  printf("ngen:                %d\n",ngen);
  printf("nentries:            %d\n",nentries);
  printf("nmctrackincalo:      %d\n",nmctrackincalo);
  printf("nmctracknotfrombot:  %d\n",nmctracknotfrombot);
  printf("nmctrackcalox0:      %d\n",nmctrackcalox0);
  printf("ncalofidvolpass:     %d\n",ncalofidvolpass);
  printf("npasscalohits:       %d\n",npasscalohits);
  

  TCanvas *ccalotrackx0 = new TCanvas("ccalotrackx0","ccalotrackx0");
  ccalotrackx0->Divide(2,1);
  ccalotrackx0->cd(1)->SetLogy();
  hcalotrack->Draw("");
  hcalotrack_fv->Draw("same");
  hcalotrack_fv_entryxy->Draw("same");
  hcalotrack_fv_entrzy->Draw("same");
  hcalotrack_fv_entrylat->Draw("same");
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
    TH1 *h = (TH1*)(heneleak->ProjectionY( Form("heneleak_%d",ii), binstart, binstop )->Clone());
    h->DrawNormalized( Form("%s", ii==1 ? "" : "same" ) );
  }

  TCanvas *ceneres2 = new TCanvas("ceneres2","ceneres2");
  ceneres2->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ceneres2->cd(ii+1)->SetLogx();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    TH1 *h = (TH1*)(heneleak->ProjectionY( Form("heneleak_%d",ii), binstart,binstop)->Clone());
    h->SetLineColor(kBlack); h->SetFillColor(kGray);
    h->Draw("");
    TH1 *h_x0 = (TH1*)(heneleak_x0->ProjectionY( Form("heneleak_x0_%d",ii), binstart,binstop)->Clone());
    h_x0->SetLineColor(kBlue+1); h_x0->SetFillColor(kBlue-10); 
    h_x0->Draw("same");
    TH1 *h_x0_fv = (TH1*)(heneleak_x0_fv->ProjectionY( Form("heneleak_x0_fv_%d",ii), binstart,binstop)->Clone());
    h_x0_fv->SetLineColor(kRed+1);     h_x0_fv->SetFillColor(kRed-10); 
    h_x0_fv->Draw("same");
  }

  TCanvas *ceneres3 = new TCanvas("ceneres3","ceneres3");
  ceneres3->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    ceneres3->cd(ii+1)->SetLogx();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    TH1 *h = (TH1*)(heneleak->ProjectionY( Form("heneleak_%d",ii), binstart,binstop)->Clone());
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
    h->GetYaxis()->SetTitle("Occurrence");
    h->GetXaxis()->SetTitle("CALO energy leak (fraction)");
    h->SetLineColor(kBlack); h->SetFillColor(kGray);
    h->Draw("");
    TH1 *h_fv = (TH1*)(heneleak_x0->ProjectionY( Form("heneleak_x0_%d",ii), binstart,binstop)->Clone());
    h_fv->SetLineColor(kGreen+2); h_fv->SetFillColor(kGreen-10); 
    h_fv->Draw("same");
    TH1 *h_x0_fv = (TH1*)(heneleak_x0_fv->ProjectionY( Form("heneleak_x0_fv_%d",ii), binstart,binstop)->Clone());
    h_x0_fv->SetLineColor(kRed+1);     h_x0_fv->SetFillColor(kRed-10); 
    h_x0_fv->Draw("same");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
    h->GetYaxis()->SetTitle("Occurrence");
    h->GetXaxis()->SetTitle("CALO energy leak (fraction)");
  }

  
  TCanvas *cenerescalohits = new TCanvas("cenerescalohits","cenerescalohits");
  cenerescalohits->Divide(3,3);
  for(int ii=0; ii<9; ii++){
    cenerescalohits->cd(ii+1)->SetLogx();     cenerescalohits->cd(ii+1)->SetLogy();     cenerescalohits->cd(ii+1)->SetLogz();
    int binstart = ii*4+1;
    int binstop  = ii*4+4;
    heneleakhits->GetXaxis()->SetRange(binstart,binstop);
    TH2 *h = (TH2*)heneleakhits->Project3D( "zy" );
    h->SetName( Form("heneleakhits_zy_%d",ii) );
    h->Draw("COLZ"); h->SetStats(0);
    h->GetXaxis()->SetTitle("CALO hits");
    h->GetYaxis()->SetTitle("CALO energy leak (fraction)");
    h->SetTitle( Form("[%.1f - %.1f] GV (MC)", enebins[binstart-1], enebins[binstart+3]) );
  }

  TCanvas *ccalohits = new TCanvas("ccalohits","ccalohits");
  ccalohits->Divide(2,1);
  ccalohits->cd(1)->SetTicks(); ccalohits->cd(1)->SetLogx(); ccalohits->cd(1)->SetLogy(); ccalohits->cd(1)->SetLogz();
  TH2F *hcalohitsnormx = NormalizeX(hcalohits);
  hcalohitsnormx->Draw("COLZ");   hcalohitsnormx->SetStats(0);
  ccalohits->cd(2)->SetTicks(); ccalohits->cd(2)->SetLogx(); ccalohits->cd(2)->SetLogy(); ccalohits->cd(2)->SetLogz();
  TH2F *hcalohits_eneleaknormx = NormalizeX(hcalohits_eneleak);
  hcalohits_eneleaknormx->Draw("COLZ"); hcalohits_eneleaknormx->SetStats(0);
  fcalohitscut->SetLineColor(kBlue+1);
  fcalohitscut->Draw("same");

  TCanvas *cenereseff = new TCanvas("cenereseff","cenereseff");
  cenereseff->cd(1)->SetTicks();  cenereseff->cd(1)->SetLogx();
  TH1* hall =    (TH1*)(heneleak->   ProjectionX( "hall",    1,heneleak->GetNbinsY())->Clone());
  TH1* hsel_fv = (TH1*)(heneleak_fv->ProjectionX( "hsel_fv", 1,heneleak->GetNbinsY())->Clone());
  TH1* hsel_x0 = (TH1*)(heneleak_x0->ProjectionX( "hsel_x0", 1,heneleak->GetNbinsY())->Clone());
  TH1* hsel_x0_fv = (TH1*)(heneleak_x0_fv->ProjectionX( "hsel_x0_fv", 1,heneleak->GetNbinsY())->Clone());
  TH1* hsel_x0_fv_hits = (TH1*)(hcalohits_pass->ProjectionX( "hsel_x0_fv_hits", 1,hcalohits_pass->GetNbinsY())->Clone());
  TGraphAsymmErrors *genereseff_fv = new TGraphAsymmErrors(); genereseff_fv->Divide(hsel_fv,hall);
  genereseff_fv->GetXaxis()->SetTitle(heneleak->GetXaxis()->GetTitle());
  genereseff_fv->GetYaxis()->SetTitle("Fraction");
  TGraphAsymmErrors *genereseff_x0 = new TGraphAsymmErrors(); genereseff_x0->Divide(hsel_x0,hall);
  genereseff_x0->GetXaxis()->SetTitle(heneleak->GetXaxis()->GetTitle());
  genereseff_x0->GetYaxis()->SetTitle("Fraction");
  TGraphAsymmErrors *genereseff_x0_fv = new TGraphAsymmErrors(); genereseff_x0_fv->Divide(hsel_x0_fv,hall);
  genereseff_x0_fv->GetXaxis()->SetTitle(heneleak->GetXaxis()->GetTitle());
  genereseff_x0_fv->GetYaxis()->SetTitle("Fraction");
  TGraphAsymmErrors *genereseff_x0_fv_hits = new TGraphAsymmErrors(); genereseff_x0_fv_hits->Divide(hsel_x0_fv_hits,hall);
  genereseff_x0_fv_hits->GetXaxis()->SetTitle(heneleak->GetXaxis()->GetTitle());
  genereseff_x0_fv_hits->GetYaxis()->SetTitle("Fraction");
  TH2F *henereseff = new TH2F("henereseff",";MC Momentum (MC);Fraction",100,10,10000,100,0.75,1); henereseff->SetStats(0); henereseff->Draw("");
  genereseff_fv->Draw("P"); genereseff_fv->SetMarkerColor(kBlue+1);   genereseff_fv->SetLineColor(kBlue+1);
  genereseff_x0->Draw("P"); genereseff_x0->SetMarkerColor(kViolet+1); genereseff_x0->SetLineColor(kViolet+1);
  genereseff_x0_fv->Draw("P"); genereseff_x0_fv->SetMarkerColor(kMagenta); genereseff_x0_fv->SetLineColor(kMagenta);
  genereseff_x0_fv_hits->Draw("P"); genereseff_x0_fv_hits->SetMarkerColor(kBlack); genereseff_x0_fv_hits->SetLineColor(kBlack);



  
  return 0;

}
