//HERD headers
#include "CaloMcHitsNumberSelection.h"
#include "dataobjects/CaloGeoParams.h"
#include "dataobjects/CaloHits.h"

// Root headers
#include "TMath.h"
#include "TString.h"

// C/C++ standard headers
#include <numeric>

using namespace std;

RegisterAlgorithm(CaloMcHitsNumberSelection);

CaloMcHitsNumberSelection::CaloMcHitsNumberSelection(const std::string &name) :
  Algorithm{name},
  _filterenable{true},
  _minhits{0},
  _edepth(0.020) //MeV
   {
     DeclareConsumedObject("caloHitsMC", ObjectCategory::EVENT, "evStore");
     DeclareConsumedObject("caloGeParams", ObjectCategory::EVENT, "evStore");

     DefineParameter("filterenable",        _filterenable);
     DefineParameter("minhits",             _minhits);
     DefineParameter("edepth",              _edepth);
   }

bool CaloMcHitsNumberSelection::Initialize() {
  const std::string routineName("CaloMcHitsNumberSelection::Initialize");

  if (_filterenable) SetFilterStatus(FilterStatus::ENABLED); else SetFilterStatus(FilterStatus::DISABLED);

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) {COUT(ERROR) << "Event data store not found." << ENDL;  return false;  }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) {COUT(ERROR) << "Global data store not found." << ENDL; return false;}

  return true;
}

bool CaloMcHitsNumberSelection::Process() {
  const std::string routineName("CaloMcHitsNumberSelection::Process");

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);
  if( !_filterenable ){ return true; }
  
  observer_ptr<Herd::CaloHits> caloHits = nullptr;
  try { caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC"); }
  catch(Retrieval::Exception &exc) { caloHits=NULL; }
  static bool caloHits_f=true; if(caloHits_f){ COUT(INFO)<<Form("%s::caloHitsMC::%s",routineName.c_str(),caloHits?"FOUND":"NOT-FOUND")<<ENDL; } caloHits_f=false;

  observer_ptr<Herd::CaloGeoParams> caloGeoParams = nullptr;
  try { caloGeoParams = _globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams"); }
  catch(Retrieval::Exception &exc) { caloGeoParams=NULL; }
  static bool caloGeoParams_f=true; if(caloGeoParams_f){ COUT(INFO)<<Form("%s::caloGeoParams::%s",routineName.c_str(),caloGeoParams?"FOUND":"NOT-FOUND")<<ENDL; } caloGeoParams_f=false;

  //Check 
  //int ncalohits = std::accumulate(caloHits->begin(), caloHits->end(), 0.,[this](int n, const Herd::Hit &hit) { return hit.EDep()<_edepth ? n : n+1; });
  int ncalohits=0;
  for( auto const &caloHit : *caloHits ){
    if( caloHit.EDep()>_edepth ){ ncalohits++; }
  }

  if( ncalohits < _minhits ) SetFilterResult(FilterResult::REJECT);

  return true;
}

bool CaloMcHitsNumberSelection::Finalize() {
  const std::string routineName("CaloMcHitsNumberSelection::Finalize");
  return true;
}

