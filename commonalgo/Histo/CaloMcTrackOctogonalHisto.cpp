// Example headers
#include "CaloMcTrackOctogonalHisto.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloHits.h"
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

RegisterAlgorithm(CaloMcTrackOctogonalHisto);

CaloMcTrackOctogonalHisto::CaloMcTrackOctogonalHisto(const std::string &name) : Algorithm{name} {

  const std::string routineName = GetName() + "::CaloMcTrackOctogonalHisto";

  DeclareConsumedObject("trackInfoForCaloMC", ObjectCategory::EVENT, "evStore");

}

bool CaloMcTrackOctogonalHisto::Initialize() {
  const std::string routineName("CaloMcTrackOctogonalHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  
  int nenebins = 140; //9 bins/decade
  Double_t *enebins = GenerateLogBinning(nenebins,1e-1,1e+6);
  int ncmlengthbins = 100;
  Double_t *cmlengthbins = GenerateBinning(ncmlengthbins,0,100);
  int nx0lengthbins = 50;
  Double_t *x0lengthbins = GenerateBinning(nx0lengthbins,0,50);
  int ncoocmbins = 100;
  Double_t *coocmbins = GenerateBinning(ncoocmbins,-100,+100);
  int nfacesbins = static_cast<int>(Herd::RefFrame::NDirections);
  Double_t *facesbins = GenerateBinning(nfacesbins,-0.5,nfacesbins-0.5);

  hcalomctrackentryxy = std::make_shared<TH3F>("hcalomctrackentryxy", "hcalomctrackentryxy;Face;X (cm);Y (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);
  hcalomctrackentryyz = std::make_shared<TH3F>("hcalomctrackentryyz", "hcalomctrackentryyz;Face;Y (cm);Z (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);
  hcalomctrackentryxz = std::make_shared<TH3F>("hcalomctrackentryxz", "hcalomctrackentryxz;Face;X (cm);Z (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);
  hcalomctrackexitxy = std::make_shared<TH3F>("hcalomctrackexitxy", "hcalomctrackexitxy;Face;X (cm);Y (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);
  hcalomctrackexityz = std::make_shared<TH3F>("hcalomctrackexityz", "hcalomctrackexityz;Face;Y (cm);Z (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);
  hcalomctrackexitxz = std::make_shared<TH3F>("hcalomctrackexitxz", "hcalomctrackexitxz;Face;X (cm);Z (cm)",nfacesbins,facesbins,ncoocmbins,coocmbins,ncoocmbins,coocmbins);

  hcalomctracklenx0 = std::make_shared<TH2F>("hcalomctracklenx0", "hcalomctracklenx0;Entry Face;MC track in CALO (X0)",nfacesbins,facesbins,nx0lengthbins,x0lengthbins);
  hcalomctracklencm = std::make_shared<TH2F>("hcalomctracklencm", "hcalomctracklencm;Entry Face;MC track in CALO (CM)",nfacesbins,facesbins,ncmlengthbins,cmlengthbins);

  hcalomctrackaftintcm = std::make_shared<TH3F>("hcalomctrackaftintcm", "hcalomctrackaftintcm;Entry Face;MC track in CALO (CM)",nenebins,enebins,nfacesbins,facesbins,ncmlengthbins,cmlengthbins);

  return true;
}

bool CaloMcTrackOctogonalHisto::Process() {
  const std::string routineName("CaloMcTrackOctogonalHisto::Process");

  observer_ptr<Herd::TrackInfoForCalo> trackInfoforCalo = nullptr;
  try { trackInfoforCalo = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC"); }
  catch(Retrieval::Exception &exc) { trackInfoforCalo=nullptr; }
  static bool trackInfoforCalo_f=true; if(trackInfoforCalo_f){ COUT(INFO)<<Form("%s::trackInfoforCaloMC::%s",routineName.c_str(),trackInfoforCalo?"FOUND":"NOT-FOUND")<<ENDL; } trackInfoforCalo_f=false;

  observer_ptr<Herd::MCTruth> mcTruth = nullptr;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=nullptr; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruthMC::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  const auto &primary = mcTruth->primaries.at(0);
  float mcmom (primary.InitialMomentum().Mag());

  hcalomctrackentryxy->Fill(static_cast<int>(trackInfoforCalo->entrancePlane),trackInfoforCalo->entrance[Herd::RefFrame::Coo::X],trackInfoforCalo->entrance[Herd::RefFrame::Coo::Y]);
  hcalomctrackentryyz->Fill(static_cast<int>(trackInfoforCalo->entrancePlane),trackInfoforCalo->entrance[Herd::RefFrame::Coo::Y],trackInfoforCalo->entrance[Herd::RefFrame::Coo::Z]);
  hcalomctrackentryxz->Fill(static_cast<int>(trackInfoforCalo->entrancePlane),trackInfoforCalo->entrance[Herd::RefFrame::Coo::X],trackInfoforCalo->entrance[Herd::RefFrame::Coo::Z]);
  hcalomctrackexitxy ->Fill(static_cast<int>(trackInfoforCalo->exitPlane),    trackInfoforCalo->exit[Herd::RefFrame::Coo::X],    trackInfoforCalo->exit[Herd::RefFrame::Coo::Y]);
  hcalomctrackexityz ->Fill(static_cast<int>(trackInfoforCalo->exitPlane),    trackInfoforCalo->exit[Herd::RefFrame::Coo::Y],    trackInfoforCalo->exit[Herd::RefFrame::Coo::Z]);
  hcalomctrackexitxz ->Fill(static_cast<int>(trackInfoforCalo->exitPlane),    trackInfoforCalo->exit[Herd::RefFrame::Coo::X],    trackInfoforCalo->exit[Herd::RefFrame::Coo::Z]);

  hcalomctracklenx0->Fill(static_cast<int>(trackInfoforCalo->entrancePlane),trackInfoforCalo->trackLengthCaloX0);
  hcalomctracklencm->Fill(static_cast<int>(trackInfoforCalo->exitPlane),    trackInfoforCalo->trackLengthCaloX0);

  hcalomctrackaftintcm->Fill(mcmom,static_cast<int>(trackInfoforCalo->entrancePlane),trackInfoforCalo->trackLengthLYSOAfterInteractionCm);

  return true;
}

bool CaloMcTrackOctogonalHisto::Finalize() {
  const std::string routineName("CaloMcTrackOctogonalHisto::Finalize");

  _globStore->AddObject(hcalomctrackentryxy->GetName(),hcalomctrackentryxy);
  _globStore->AddObject(hcalomctrackentryyz->GetName(),hcalomctrackentryyz);
  _globStore->AddObject(hcalomctrackentryxz->GetName(),hcalomctrackentryxz);
  _globStore->AddObject(hcalomctrackexitxy->GetName(),hcalomctrackexitxy);
  _globStore->AddObject(hcalomctrackexityz->GetName(),hcalomctrackexityz);
  _globStore->AddObject(hcalomctrackexitxz->GetName(),hcalomctrackexitxz);
  _globStore->AddObject(hcalomctracklenx0->GetName(),hcalomctracklenx0);
  _globStore->AddObject(hcalomctracklencm->GetName(),hcalomctracklencm);
  _globStore->AddObject(hcalomctrackaftintcm->GetName(),hcalomctrackaftintcm);

  return true;
}

double* CaloMcTrackOctogonalHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloMcTrackOctogonalHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
