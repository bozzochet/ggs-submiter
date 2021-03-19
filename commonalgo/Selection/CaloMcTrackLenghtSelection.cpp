//HERD headers
#include "CaloMcTrackLenghtSelection.h"
#include "dataobjects/CaloGeoParams.h"
#include "dataobjects/TrackInfoForCalo.h"

// Root headers
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TGraph2D.h"

// C/C++ standard headers
#include <numeric>

using namespace std;

RegisterAlgorithm(CaloMcTrackLengthSelection);

CaloMcTrackLengthSelection::CaloMcTrackLengthSelection(const std::string &name) :
  Algorithm{name},
  _filterenable{true},
  _mincalotrackx0{-999},
  _notfrombottom{true}
   {
     DeclareConsumedObject("trackInfoForCaloMC", ObjectCategory::EVENT, "evStore");
     DeclareConsumedObject("caloGeParams", ObjectCategory::EVENT, "evStore");

     DefineParameter("filterenable",        _filterenable);
     DefineParameter("mincalotrackx0",      _mincalotrackx0);
     DefineParameter("notfrombottom",       _notfrombottom);
   }

bool CaloMcTrackLengthSelection::Initialize() {
  const std::string routineName("CaloMcTrackLengthSelection::Initialize");

  if (_filterenable) SetFilterStatus(FilterStatus::ENABLED); else SetFilterStatus(FilterStatus::DISABLED);

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) {COUT(ERROR) << "Event data store not found." << ENDL;  return false;  }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) {COUT(ERROR) << "Global data store not found." << ENDL; return false;}

  return true;
}

bool CaloMcTrackLengthSelection::Process() {
  const std::string routineName("CaloMcTrackLengthSelection::Process");

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);

  observer_ptr<Herd::TrackInfoForCalo> mcCaloTrack = nullptr;
  try { mcCaloTrack = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC"); }
  catch(Retrieval::Exception &exc) { mcCaloTrack=NULL; }
  static bool mcCaloTrack_f=true; if(mcCaloTrack_f){ COUT(INFO)<<Form("%s::mcCaloTrack::%s",routineName.c_str(),mcCaloTrack?"FOUND":"NOT-FOUND")<<ENDL; } mcCaloTrack_f=false;

  observer_ptr<Herd::CaloGeoParams> caloGeoParams = nullptr;
  try { caloGeoParams = _globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams"); }
  catch(Retrieval::Exception &exc) { caloGeoParams=NULL; }
  static bool caloGeoParams_f=true; if(caloGeoParams_f){ COUT(INFO)<<Form("%s::caloGeoParams::%s",routineName.c_str(),caloGeoParams?"FOUND":"NOT-FOUND")<<ENDL; } caloGeoParams_f=false;

  //Check if teh track enter the calorimeter volume at all
  if(mcCaloTrack->entrancePlane == Herd::RefFrame::Direction::NONE) SetFilterResult(FilterResult::REJECT);
  
  //Check MC track entrance plane
  if(_notfrombottom) {
    if(mcCaloTrack->entrancePlane == Herd::RefFrame::Direction::Zneg) SetFilterResult(FilterResult::REJECT);
    } 
  //Check MC track length
  if(mcCaloTrack->trackLengthCaloX0<_mincalotrackx0) SetFilterResult(FilterResult::REJECT);
  
  

  return true;
}

bool CaloMcTrackLengthSelection::Finalize() {
  const std::string routineName("CaloMcTrackLengthSelection::Finalize");
  return true;
}

void CaloMcTrackLengthSelection::PrintCaloCubeMap(){
  const std::string routineName("CaloMcTrackLengthSelection::PrintCaloCubeMap");
  
  observer_ptr<Herd::CaloGeoParams> caloGeoParams = nullptr;
  try { caloGeoParams = _globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams"); }
  catch(Retrieval::Exception &exc) { caloGeoParams=NULL; }
  static bool caloGeoParams_f=true; if(caloGeoParams_f){ COUT(INFO)<<Form("%s::caloGeoParams::%s",routineName.c_str(),caloGeoParams?"FOUND":"NOT-FOUND")<<ENDL; } caloGeoParams_f=false;

  for(unsigned int icube=0; icube<caloGeoParams->NCubes(); icube++){
    printf("[%04u]\t%.2f\t%.2f\t%.2f\t%.2f\n", icube, caloGeoParams->CubeSize(), caloGeoParams->Position(icube)[Herd::RefFrame::Coo::X],caloGeoParams->Position(icube)[Herd::RefFrame::Coo::Y],caloGeoParams->Position(icube)[Herd::RefFrame::Coo::Z]);
  }
}

