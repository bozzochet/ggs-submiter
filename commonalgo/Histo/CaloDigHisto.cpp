// Example headers
#include "CaloDigHisto.h"
#include "dataobjects/CaloHits.h"
#include "dataobjects/CaloClusters.h"
#include "dataobjects/CaloGeoParams.h"
#include "dataobjects/MCTruth.h"

// Root headers
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>
#include <vector>

RegisterAlgorithm(CaloDigHisto);

CaloDigHisto::CaloDigHisto(const std::string &name) : Algorithm{name} {
  
}

bool CaloDigHisto::Initialize() {
  const std::string routineName("CaloDigHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }

  int nhitenebins = 200;
  double *hitenebins = GenerateLogBinning(nhitenebins, 1e-3, 1e+3);  
  int nmcenebins = 100;
  double *mcenebins = GenerateLogBinning(nmcenebins, 1e+1, 1e+6);  
  int ncaloenebins = 120;
  double *caloenebins = GenerateLogBinning(ncaloenebins, 1, 1e+6);  
  hpdemchit   = std::make_shared<TH2F>("hpdemchit",  ";MC Hit Energy (GeV) [MC]; PD Hit Energy (GeV)", nhitenebins, hitenebins, nhitenebins, hitenebins);
  hlpdemchit  = std::make_shared<TH2F>("hlpdemchit", ";MC Hit Energy (GeV) [MC]; LPD Hit Energy (GeV)", nhitenebins, hitenebins, nhitenebins, hitenebins);
  hspdemchit  = std::make_shared<TH2F>("hspdemchit", ";MC Hit Energy (GeV) [MC]; SPD Hit Energy (GeV)", nhitenebins, hitenebins, nhitenebins, hitenebins);
  hpdelpdehit = std::make_shared<TH2F>("hpdelpdehit",";PD Hit Energy (GeV); LPD Hit Energy (GeV); ",    nhitenebins, hitenebins, nhitenebins, hitenebins);
  hpdespdehit = std::make_shared<TH2F>("hpdespdehit",";PD Hit Energy (GeV); SPD Hit Energy (GeV); ",    nhitenebins, hitenebins, nhitenebins, hitenebins);
  htotmcpde = std::make_shared<TH2F>("htotmcpde", ";MC Energy (GeV) [MC]; Total Edep (GeV) [PD]", nmcenebins, mcenebins, nmcenebins, mcenebins);
  htotmclpde = std::make_shared<TH2F>("htotmclpde", ";MC Energy (GeV) [MC]; Total Edep (GeV) [LPD]", nmcenebins, mcenebins, nmcenebins, mcenebins);
  htotmcspde = std::make_shared<TH2F>("htotmcspde", ";MC Energy (GeV) [MC]; Total Edep (GeV) [SPD]", nmcenebins, mcenebins, nmcenebins, mcenebins);
  hreldiffpde = std::make_shared<TH2F>("hreldiffpde", ";MC Energy (GeV) [MC]; Rel Diff (PD Edep - MC Edep)", ncaloenebins, caloenebins, 500, -2.5, +2.5);
  hmchitpdehit= std::make_shared<TH2F>("hmchitpdehit", "Hits above th.; PD Hit Energy (GeV);MC Hit Energy (GeV) [MC]", nhitenebins, hitenebins, nhitenebins, hitenebins);
  htotmc = std::make_shared<TH2F>("htotmc", "All MC hits; MC Energy (GeV) [MC]; Total Edep (GeV) [MC]", nmcenebins, mcenebins, nmcenebins, mcenebins);
  hreldiffpdeaboveth = std::make_shared<TH2F>("hreldiffpdeaboveth", "Hits above th.; Total Edep (GeV) [MC]; Rel Diff (PD Edep -  MC Edep)", ncaloenebins, caloenebins, 100, -0.05, +0.05);
  htotmcaboveth = std::make_shared<TH2F>("htotmcaboveth", "Hits above th.; MC Energy (GeV) [MC]; Total Edep (GeV) [MC]", nmcenebins, mcenebins, nmcenebins, mcenebins);
  hmchitpdehitreldiff = std::make_shared<TH2F>("hmchitpdehitreldiff", "Hits above th.; MC Hit Energy (GeV) [MC]; Rel Diff (PD Edep - MC Edep)", nhitenebins, hitenebins, 200, -1, 1);
  hmchitpdehitreldiffall = std::make_shared<TH2F>("hmchitpdehitreldiffall", "All hits; MC Hit Energy (GeV) [MC]; Rel Diff (PD Edep - MC Edep)", nhitenebins, hitenebins, 200, -1, 1);

  return true;
}

bool CaloDigHisto::Process() {
  const std::string routineName("CaloDigHisto::Process");

  Herd::CaloHits* caloHits = NULL;
  try { caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC"); }
  catch(Retrieval::Exception &exc) { caloHits=NULL; }
  static bool caloHits_f=true; if(caloHits_f){ COUT(INFO)<<Form("%s::caloHitsMC::%s",routineName.c_str(),caloHits?"FOUND":"NOT-FOUND")<<ENDL; } caloHits_f=false;

  Herd::CaloGeoParams* caloGeoParams = NULL;
  try { caloGeoParams = _globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams"); }
  catch(Retrieval::Exception &exc) { COUT(INFO) << "caloGeoParams not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  static bool caloGeoParams_f=true; if(caloGeoParams_f){ COUT(INFO)<<Form("%s::caloGeoParams::%s",routineName.c_str(),caloGeoParams?"FOUND":"NOT-FOUND")<<ENDL; } caloGeoParams_f=false;

  Herd::MCTruth* mcTruth = NULL;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=NULL; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruth::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  Herd::CaloHits* caloLPDHits = NULL;
  try { caloLPDHits = _evStore->GetObject<Herd::CaloHits>("caloLPDHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloLPDHits=NULL; }
  static bool caloLPDHits_f=true; if(caloLPDHits_f){ COUT(INFO)<<Form("%s::caloLPDHitsGeV::%s",routineName.c_str(),caloLPDHits?"FOUND":"NOT-FOUND")<<ENDL; } caloLPDHits_f=false;

  Herd::CaloHits* caloSPDHits = NULL;
  try { caloSPDHits = _evStore->GetObject<Herd::CaloHits>("caloSPDHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloSPDHits=NULL; }
  static bool caloSPDHits_f=true; if(caloSPDHits_f){ COUT(INFO)<<Form("%s::caloSPDHitsGeV::%s",routineName.c_str(),caloSPDHits?"FOUND":"NOT-FOUND")<<ENDL; } caloSPDHits_f=false;

  Herd::CaloHits* caloStitchedHits = NULL;
  try { caloStitchedHits = _evStore->GetObject<Herd::CaloHits>("caloHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloStitchedHits=NULL; }
  static bool caloStitchedHits_f=true; if(caloStitchedHits_f){ COUT(INFO)<<Form("%s::caloHitsGeV::%s",routineName.c_str(),caloStitchedHits?"FOUND":"NOT-FOUND")<<ENDL; } caloStitchedHits_f=false;

  float calototedepSPDE = accumulate(caloSPDHits->begin(), caloSPDHits->end(), 0.,
				     [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedepLPDE = accumulate(caloLPDHits->begin(), caloLPDHits->end(), 0.,
				     [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedepPDE = accumulate(caloStitchedHits->begin(), caloStitchedHits->end(), 0.,
				    [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  double mcmom = std::sqrt(mcTruth->primaries.at(0).InitialMomentum() * mcTruth->primaries.at(0).InitialMomentum());
  float calototedepaboveth = 0;

  //Find correlation between MC hits and PD digitized hits
  std::map<int,float>::iterator mapit;
  std::map<int,float> calomchitmap;   for( auto const &hit : *caloHits ){ calomchitmap.insert( std::pair<int,float>(hit.VolumeID(),hit.EDep() ) ); }
  std::map<int,float> calopdehitmap;  for( auto const &hit : *caloStitchedHits ){ calopdehitmap.insert( std::pair<int,float>(hit.VolumeID(),hit.EDep() ) ); }
  std::map<int,float> calolpdehitmap; for( auto const &hit : *caloLPDHits ){ calolpdehitmap.insert( std::pair<int,float>(hit.VolumeID(),hit.EDep() ) ); }
  std::map<int,float> calospdehitmap; for( auto const &hit : *caloSPDHits ){ calospdehitmap.insert( std::pair<int,float>(hit.VolumeID(),hit.EDep() ) ); }
  

  for( auto const &mchit : *caloHits ){
    mapit = calopdehitmap.find(mchit.VolumeID());
    if( mapit != calopdehitmap.end() ){ hpdemchit->Fill(mchit.EDep(),mapit->second); hmchitpdehitreldiffall->Fill(mchit.EDep(), (-mchit.EDep()+mapit->second)/mapit->second); }
    mapit = calolpdehitmap.find(mchit.VolumeID());
    if( mapit != calolpdehitmap.end() ){ hlpdemchit->Fill(mchit.EDep(),mapit->second); }
    mapit = calospdehitmap.find(mchit.VolumeID());
    if( mapit != calospdehitmap.end() ){ hspdemchit->Fill(mchit.EDep(),mapit->second); }
  }

  for( auto const &hit : *caloStitchedHits ){
    mapit = calolpdehitmap.find(hit.VolumeID());
    if( mapit != calolpdehitmap.end() ){ hpdelpdehit->Fill(hit.EDep(),mapit->second); /*printf("LPD: %f %f %f\n", hit.EDep(), mapit->second, hit.EDep()- mapit->second);*/}
    mapit = calospdehitmap.find(hit.VolumeID());
    if( mapit != calospdehitmap.end() ){ hpdespdehit->Fill(hit.EDep(),mapit->second); /*printf("SPD: %f %f %f\n", hit.EDep(), mapit->second, hit.EDep()- mapit->second);*/}
    mapit = calomchitmap.find(hit.VolumeID());
    if( mapit != calomchitmap.end() ){ 
      hmchitpdehit->Fill(hit.EDep(),mapit->second); //printf("PD: %f MC: %f\n", hit.EDep(), mapit->second);
      hmchitpdehitreldiff->Fill(mapit->second, (hit.EDep()-mapit->second)/mapit->second);
      calototedepaboveth+=mapit->second;
    }
  }

  htotmcpde->Fill( mcmom, calototedepPDE);
  htotmclpde->Fill( mcmom, calototedepLPDE);
  htotmcspde->Fill( mcmom, calototedepSPDE);
  htotmc->Fill( mcmom, calototedep);
  htotmcaboveth->Fill(mcmom, calototedepaboveth);
  
  hreldiffpde->Fill( calototedep, (calototedepPDE-calototedep)/calototedep);
  hreldiffpdeaboveth->Fill( calototedepaboveth, (calototedepPDE-calototedepaboveth)/calototedepaboveth);
  //printf("%f %f\n", calototedepaboveth, calototedepPDE);
  return true;
}

bool CaloDigHisto::Finalize() {
  const std::string routineName("CaloDigHisto::Finalize");
  
  _globStore->AddObject(hpdemchit->GetName(), hpdemchit);
  _globStore->AddObject(hlpdemchit->GetName(), hlpdemchit);
  _globStore->AddObject(hspdemchit->GetName(), hspdemchit);
  _globStore->AddObject(hpdelpdehit->GetName(), hpdelpdehit);
  _globStore->AddObject(hpdespdehit->GetName(), hpdespdehit);
  _globStore->AddObject(htotmcpde->GetName(),  htotmcpde);
  _globStore->AddObject(htotmclpde->GetName(), htotmclpde);
  _globStore->AddObject(htotmcspde->GetName(), htotmcspde);
  _globStore->AddObject(hreldiffpde->GetName(), hreldiffpde); 
  _globStore->AddObject(hmchitpdehit->GetName(), hmchitpdehit); 
  _globStore->AddObject(htotmc->GetName(), htotmc); 
  _globStore->AddObject(hreldiffpdeaboveth->GetName(), hreldiffpdeaboveth); 
  _globStore->AddObject(htotmcaboveth->GetName(), htotmcaboveth); 
  _globStore->AddObject(hmchitpdehitreldiff->GetName(), hmchitpdehitreldiff);
  _globStore->AddObject(hmchitpdehitreldiffall->GetName(), hmchitpdehitreldiffall);


  return true;
}

double* CaloDigHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloDigHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
