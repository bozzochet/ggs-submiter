// Example headers
#include "CaloEnergyDepositHisto.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloHits.h"

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

RegisterAlgorithm(CaloEnergyDepositHisto);

CaloEnergyDepositHisto::CaloEnergyDepositHisto(const std::string &name) : Algorithm{name} {

  const std::string routineName = GetName() + "::CaloEnergyDepositHisto";

  DeclareConsumedObject("trackInfoForCaloMC", ObjectCategory::EVENT, "evStore");

}

bool CaloEnergyDepositHisto::Initialize() {
  const std::string routineName("CaloEnergyDepositHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  
  int nenebins = 140; //20 bins/decade
  Double_t *enebins = GenerateLogBinning(nenebins,1e-1,1e+6);
  int nfracbins = 200;
  Double_t *fracbins = GenerateBinning(nfracbins,0,1);
  
  hcaloedep = std::make_shared<TH2F>("hcaloedep","hcaloedep;Energy (GeV) [MC];CALO Edep (GeV)",nenebins,enebins,nenebins,enebins);
  hcaloedepfrac = std::make_shared<TH3F>("hcaloedepfrac","hcaloedepfrac;Energy (GeV) [MC];CALO Edep (GeV), Edep/MC",nenebins,enebins,nenebins,enebins,nfracbins,fracbins);
  hcaloedepleak = std::make_shared<TH3F>("hcaloedepleak","hcaloedepleak;Energy (GeV) [MC];CALO Edep (GeV), (MC-Edep)/MC",nenebins,enebins,nenebins,enebins,nfracbins,fracbins);

  return true;
}

bool CaloEnergyDepositHisto::Process() {
  const std::string routineName("CaloEnergyDepositHisto::Process");

  // observer_ptr<Herd::TrackInfoForCalo> trackInfoforCalo = nullptr;
  // try { trackInfoforCalo = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC"); }
  // catch(Retrieval::Exception &exc) { trackInfoforCalo=nullptr; }
  // static bool trackInfoforCalo_f=true; if(trackInfoforCalo_f){ COUT(INFO)<<Form("%s::trackInfoforCaloMC::%s",routineName.c_str(),trackInfoforCalo?"FOUND":"NOT-FOUND")<<ENDL; } trackInfoforCalo_f=false;

  observer_ptr<Herd::MCTruth> mcTruth = nullptr;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=nullptr; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruth::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  observer_ptr<Herd::CaloHits> caloHits = nullptr;
  try { caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC"); }
  catch(Retrieval::Exception &exc) { caloHits=nullptr; }
  static bool caloHits_f=true; if(caloHits_f){ COUT(INFO)<<Form("%s::caloHitsMC::%s",routineName.c_str(),caloHits?"FOUND":"NOT-FOUND")<<ENDL; } caloHits_f=false;

  const auto &primary = mcTruth->primaries.at(0);
  float mcmom (primary.InitialMomentum().Mag());

  float edep=0;
  for( auto const &caloHit : *caloHits ){
    edep += caloHit.EDep();
  }
  float edepfrac = edep/mcmom;
  float edepleak = (mcmom-edep)/mcmom;

  hcaloedep->Fill(mcmom,edep);
  hcaloedepfrac->Fill(mcmom,edep,edepfrac);
  hcaloedepleak->Fill(mcmom,edep,edepleak);

  return true;
}

bool CaloEnergyDepositHisto::Finalize() {
  const std::string routineName("CaloEnergyDepositHisto::Finalize");

  _globStore->AddObject(hcaloedep->GetName(),hcaloedep);
  _globStore->AddObject(hcaloedepfrac->GetName(),hcaloedepfrac);
  _globStore->AddObject(hcaloedepleak->GetName(),hcaloedepleak);

  return true;
}

double* CaloEnergyDepositHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloEnergyDepositHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
