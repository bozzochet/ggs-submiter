// Example headers
#include "CaloMcHitsHisto.h"
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

RegisterAlgorithm(CaloMcHitsHisto);

CaloMcHitsHisto::CaloMcHitsHisto(const std::string &name) : Algorithm{name} {

  const std::string routineName = GetName() + "::CaloMcHitsHisto";

  DeclareConsumedObject("caloHitsMC", ObjectCategory::EVENT, "evStore");
  DeclareConsumedObject("mcTruth", ObjectCategory::EVENT, "evStore");

  DefineParameter("edepth", _edepth);
}

bool CaloMcHitsHisto::Initialize() {
  const std::string routineName("CaloMcHitsHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  
  int nenebins = 140; //9 bins/decade
  Double_t *enebins = GenerateLogBinning(nenebins,1e-1,1e+6);
  int nhitsbins = 2000;
  Double_t *hitsbins = GenerateBinning(nhitsbins,0,8000);

  hcalohitsemc    = std::make_shared<TH2F>("hcalohitsemc", "hcalohitsemc;Energy (GeV) [MC];CALO hits",nenebins,enebins,nhitsbins,hitsbins);
  hcalohitsedep   = std::make_shared<TH2F>("hcalohitsedep","hcalohitsedep;Energy (GeV) [CALO];CALO hits",nenebins,enebins,nhitsbins,hitsbins);
  hcalohitsemcall    = std::make_shared<TH2F>("hcalohitsemcall", "hcalohitsemcall;Energy (GeV) [MC];CALO hits",nenebins,enebins,nhitsbins,hitsbins);
  hcalohitsedepall   = std::make_shared<TH2F>("hcalohitsedepall","hcalohitsedepall;Energy (GeV) [CALO];CALO hits",nenebins,enebins,nhitsbins,hitsbins);

  return true;
}

bool CaloMcHitsHisto::Process() {
  const std::string routineName("CaloMcHitsHisto::Process");

  observer_ptr<Herd::CaloHits> caloHits = nullptr;
  try { caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC"); }
  catch(Retrieval::Exception &exc) { caloHits=nullptr; }
  static bool caloHits_f=true; if(caloHits_f){ COUT(INFO)<<Form("%s::caloHitsMC::%s",routineName.c_str(),caloHits?"FOUND":"NOT-FOUND")<<ENDL; } caloHits_f=false;

  observer_ptr<Herd::MCTruth> mcTruth = nullptr;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=NULL; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruth::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;
    
  const auto &primary = mcTruth->primaries.at(0);
  float mcmom (primary.InitialMomentum().Mag());

  float edep=0;
  int nhitsall=0;
  int nhits=0;
  for( auto const &caloHit : *caloHits ){
    edep += caloHit.EDep();
    if( caloHit.EDep()>_edepth ){ nhits++; }
    nhitsall++;
  }
  
  hcalohitsemc->Fill(mcmom, nhits);
  hcalohitsedep->Fill(edep, nhits);
  hcalohitsemcall->Fill(mcmom, nhitsall);
  hcalohitsedepall->Fill(edep, nhitsall);

  return true;
}

bool CaloMcHitsHisto::Finalize() {
  const std::string routineName("CaloMcHitsHisto::Finalize");

  _globStore->AddObject(hcalohitsemc->GetName(),hcalohitsemc);
  _globStore->AddObject(hcalohitsedep->GetName(),hcalohitsedep);
  _globStore->AddObject(hcalohitsemcall->GetName(),hcalohitsemcall);
  _globStore->AddObject(hcalohitsedepall->GetName(),hcalohitsedepall);
  
  return true;
}

double* CaloMcHitsHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloMcHitsHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
