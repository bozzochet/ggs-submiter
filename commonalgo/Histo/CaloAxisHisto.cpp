// Example headers
#include "CaloAxisHisto.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloAxis.h"
#include "dataobjects/CaloAxes.h"
#include "dataobjects/TrackInfoForCalo.h"

// Root headers
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMath.h"
#include "TVector3.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>
#include <vector>

RegisterAlgorithm(CaloAxisHisto);

CaloAxisHisto::CaloAxisHisto(const std::string &name) : Algorithm{name} {

  const std::string routineName = GetName() + "::CaloAxisHisto";
  DeclareConsumedObject("caloAxes", ObjectCategory::EVENT, "evStore");
  DeclareConsumedObject("mcTruth", ObjectCategory::EVENT, "evStore");
  DeclareConsumedObject("trackInfoForCaloMC", ObjectCategory::EVENT, "evStore");

}

bool CaloAxisHisto::Initialize() {
  const std::string routineName("CaloAxisHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }

  int nenebins = 60; //9 bins/decade
  Double_t *enebins = GenerateLogBinning(nenebins,1e-1,1e+6);
  int nthetabins = 200;
  Double_t *thetabins = GenerateBinning(nthetabins,0,+TMath::Pi());
  int nphibins = 200;
  Double_t *phibins = GenerateBinning(nphibins,-TMath::Pi(),+TMath::Pi());
  int ndiffbins = 400;
  Double_t *diffbins = GenerateBinning(ndiffbins,0,20);
  int ndirbins = 100;
  Double_t *dirbins = GenerateBinning(ndirbins,-1,+1);
  int nfacebins = 10;
  Double_t *facebins = GenerateBinning(nfacebins,-0.5,9.5);
  int nxybins = 200;
  Double_t *xybins = GenerateBinning(nxybins,-45,+45);
  int nzbins = 170;
  Double_t *zbins = GenerateBinning(nzbins,-80,+5);

  hcaloaxistheta    = std::make_shared<TH3F>("hcaloaxistheta","hcaloaxistheta;Theta [MC];Theta [CALO]",nenebins, enebins, nthetabins, thetabins, nthetabins, thetabins);
  hcaloaxisphi      = std::make_shared<TH3F>("hcaloaxisphi",  "hphi;Phi [MC];Phi [CALO]",nenebins, enebins, nphibins, phibins, nphibins, phibins);
  for(int i=0; i<3; i++){
    hcaloaxisdir[i] = std::make_shared<TH3F>( Form("hcaloaxisdir_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nenebins, enebins, ndirbins, dirbins, ndirbins, dirbins); }

  hcaloaxisthetaface    = std::make_shared<TH3F>("hcaloaxisthetaface","hcaloaxisthetaface;Theta [MC];Theta [CALO]",nfacebins, facebins, nthetabins, thetabins, nthetabins, thetabins);
  hcaloaxisphiface      = std::make_shared<TH3F>("hcaloaxisphiface",  "hcaloaxisphiface;Phi [MC];Phi [CALO]",nfacebins, facebins, nphibins, phibins, nphibins, phibins);
  for(int i=0; i<3; i++) {
    hcaloaxisdirface[i] = std::make_shared<TH3F>( Form("hcaloaxisdirface_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nfacebins, facebins, ndirbins, dirbins, ndirbins, dirbins); }

  hcaloaxisthetadiff    = std::make_shared<TH3F>("hcaloaxisthetadiff","hcaloaxisthetadiff;Theta [MC];Theta [CALO]",nenebins, enebins, nthetabins, thetabins, ndiffbins, diffbins);
  hcaloaxisphidiff      = std::make_shared<TH3F>("hcaloaxisphidiff",  "hcaloaxisphidiff;Phi [MC];Phi [CALO]",nenebins, enebins, nphibins, phibins, ndiffbins, diffbins);
  for(int i=0; i<3; i++) {
    hcaloaxisdirdiff[i] = std::make_shared<TH3F>( Form("hcaloaxisdirdiff_%d",i), Form("Dir[%d];Theta [MC];Theta [CALO]",i), nenebins, enebins, ndirbins, dirbins, ndiffbins, diffbins);
  }
  hcaloaxisdiff     = std::make_shared<TH2F>("hcaloaxisdiff", "hcaloaxisdiff", nenebins, enebins, ndiffbins, diffbins);

  hcaloaxiscogxy    = std::make_shared<TH3F>("hcaloaxiscogxy","hcaloaxiscogxy;",nenebins, enebins, nxybins, xybins, nxybins, xybins);
  hcaloaxiscogxz    = std::make_shared<TH3F>("hcaloaxiscogxz","hcaloaxiscogxz;",nenebins, enebins, nxybins, xybins, nzbins, zbins);
  hcaloaxiscogyz    = std::make_shared<TH3F>("hcaloaxiscogyz","hcaloaxiscogyz;",nenebins, enebins, nxybins, xybins, nzbins, zbins);

  return true;
}

bool CaloAxisHisto::Process() {
  const std::string routineName("CaloAxisHisto::Process");

  observer_ptr<Herd::CaloAxes> caloAxes = nullptr;
  try { caloAxes = _evStore->GetObject<Herd::CaloAxes>("caloAxes"); }
  catch(Retrieval::Exception &exc) { caloAxes=nullptr; }
  static bool caloAxes_f=true; if(caloAxes_f){ COUT(INFO)<<Form("%s::caloAxesMC::%s",routineName.c_str(),caloAxes?"FOUND":"NOT-FOUND")<<ENDL; } caloAxes_f=false;

  observer_ptr<Herd::MCTruth> mcTruth = nullptr;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=NULL; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruth::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  observer_ptr<Herd::TrackInfoForCalo> mcCaloTrack = nullptr;
  try { mcCaloTrack = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC"); }
  catch(Retrieval::Exception &exc) { mcCaloTrack=NULL; }
  static bool mcCaloTrack_f=true; if(mcCaloTrack_f){ COUT(INFO)<<Form("%s::mcCaloTrack::%s",routineName.c_str(),mcCaloTrack?"FOUND":"NOT-FOUND")<<ENDL; } mcCaloTrack_f=false;

  const auto &primary = mcTruth->primaries.at(0);
  Herd::Point gencoo = primary.InitialPosition();
  TVector3 genmom (primary.InitialMomentum()[Herd::RefFrame::Coo::X],primary.InitialMomentum()[Herd::RefFrame::Coo::Y],primary.InitialMomentum()[Herd::RefFrame::Coo::Z]);
  Double_t mcctheta = genmom.CosTheta();
  Double_t mctheta = acos(genmom.CosTheta());
  Double_t mcphi = genmom.Phi();
  Double_t mcmom = genmom.Mag();
  Herd::Vec3D mcdir = primary.InitialTrajectory().Direction();

  Herd::RefFrame::Direction entrydir = mcCaloTrack->entrancePlane;
  Herd::RefFrame::Direction exitdir = mcCaloTrack->exitPlane;

  for( auto const &caloAxis : *caloAxes ){
    
    Herd::Point cog = caloAxis.GetCOG();
    Herd::Line caloTrack = caloAxis.GetAxis();
    hcaloaxiscogxy->Fill(mcmom, cog[Herd::RefFrame::Coo::X], cog[Herd::RefFrame::Coo::Y]);
    hcaloaxiscogxz->Fill(mcmom, cog[Herd::RefFrame::Coo::X], cog[Herd::RefFrame::Coo::Z]);
    hcaloaxiscogyz->Fill(mcmom, cog[Herd::RefFrame::Coo::Y], cog[Herd::RefFrame::Coo::Z]);

    Double_t theta = acos(caloTrack.Direction()[Herd::RefFrame::Coo::Z]);
    Double_t phi   = atan2(caloTrack.Direction()[Herd::RefFrame::Coo::Y],caloTrack.Direction()[Herd::RefFrame::Coo::X]);
    
    hcaloaxistheta->Fill(mcmom, mctheta, theta);
    hcaloaxisphi->Fill(mcmom, mcphi, phi);
    hcaloaxisdir[0]->Fill( mcmom, mcdir[Herd::RefFrame::Coo::X], caloTrack.Direction()[Herd::RefFrame::Coo::X]);
    hcaloaxisdir[1]->Fill( mcmom, mcdir[Herd::RefFrame::Coo::Y], caloTrack.Direction()[Herd::RefFrame::Coo::Y]);
    hcaloaxisdir[2]->Fill( mcmom, mcdir[Herd::RefFrame::Coo::Z], caloTrack.Direction()[Herd::RefFrame::Coo::Z]);
 
    hcaloaxisthetaface->Fill( static_cast<int>(entrydir), mctheta, theta );
    hcaloaxisphiface->Fill( static_cast<int>(entrydir), mcphi, phi );
    hcaloaxisdirface[0]->Fill( static_cast<int>(entrydir), mcdir[Herd::RefFrame::Coo::X], caloTrack.Direction()[Herd::RefFrame::Coo::X]);
    hcaloaxisdirface[1]->Fill( static_cast<int>(entrydir), mcdir[Herd::RefFrame::Coo::Y], caloTrack.Direction()[Herd::RefFrame::Coo::Y]);
    hcaloaxisdirface[2]->Fill( static_cast<int>(entrydir), mcdir[Herd::RefFrame::Coo::Z], caloTrack.Direction()[Herd::RefFrame::Coo::Z]);

    float angdiff = acos( mcdir[Herd::RefFrame::Coo::X]*caloTrack.Direction()[Herd::RefFrame::Coo::X] + mcdir[Herd::RefFrame::Coo::Y]*caloTrack.Direction()[Herd::RefFrame::Coo::Y] + mcdir[Herd::RefFrame::Coo::Z]*caloTrack.Direction()[Herd::RefFrame::Coo::Z]);
    if(angdiff>TMath::Pi()/2) angdiff-=TMath::Pi();
    angdiff = fabs(angdiff);
  
    hcaloaxisthetadiff->Fill(mcmom, mctheta, angdiff);
    hcaloaxisphidiff->Fill(mcmom, mcphi, angdiff);
    hcaloaxisdirdiff[0]->Fill(mcmom, caloTrack.Direction()[Herd::RefFrame::Coo::X], angdiff);
    hcaloaxisdirdiff[1]->Fill(mcmom, caloTrack.Direction()[Herd::RefFrame::Coo::Y], angdiff);
    hcaloaxisdirdiff[2]->Fill(mcmom, caloTrack.Direction()[Herd::RefFrame::Coo::Z], angdiff);
    hcaloaxisdiff->Fill(mcmom,angdiff);
  
  }
  return true;
}

bool CaloAxisHisto::Finalize() {
  const std::string routineName("CaloAxisHisto::Finalize");
  
  _globStore->AddObject(hcaloaxistheta->GetName(),hcaloaxistheta);
  _globStore->AddObject(hcaloaxisphi->GetName(),hcaloaxisphi);
  for(int i=0; i<3; i++){_globStore->AddObject(hcaloaxisdir[i]->GetName(),hcaloaxisdir[i]); }
  _globStore->AddObject(hcaloaxisthetaface->GetName(),hcaloaxisphiface);
  _globStore->AddObject(hcaloaxisphiface->GetName(),hcaloaxisphiface);
  for(int i=0; i<3; i++){_globStore->AddObject(hcaloaxisdirface[i]->GetName(),hcaloaxisdirface[i]);}
  _globStore->AddObject(hcaloaxisthetadiff->GetName(),hcaloaxisthetadiff);
  _globStore->AddObject(hcaloaxisphidiff->GetName(),hcaloaxisphidiff);
  for(int i=0; i<3; i++){_globStore->AddObject(hcaloaxisdirdiff[i]->GetName(),hcaloaxisdirdiff[i]);}
  _globStore->AddObject(hcaloaxisdiff->GetName(),hcaloaxisdiff);
  _globStore->AddObject(hcaloaxiscogxy->GetName(),hcaloaxiscogxy);
  _globStore->AddObject(hcaloaxiscogxz->GetName(),hcaloaxiscogxz);
  _globStore->AddObject(hcaloaxiscogyz->GetName(),hcaloaxiscogyz);

  return true;
}

double* CaloAxisHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloAxisHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
