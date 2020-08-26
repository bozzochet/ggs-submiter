// Example headers
#include "CaloClusteringHisto.h"
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

RegisterAlgorithm(CaloClusteringHisto);

CaloClusteringHisto::CaloClusteringHisto(const std::string &name) : Algorithm{name} {
  
}

bool CaloClusteringHisto::Initialize() {
  const std::string routineName("CaloClusteringHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }


  // Create the histogram
  int nenebins = 30;
  double *enebins = GenerateLogBinning(nenebins, 10, 10000);
  int ncaloclustersbins = 21;
  double *caloclustersbins = GenerateBinning(ncaloclustersbins, -1.5, 19.5);
  int nenefracbins = 100;
  double *enefracbins = GenerateLogBinning(nenefracbins, 1e-1, 1);
  int nleakfracbins = 200;
  double *leakfracbins = GenerateLogBinning(nleakfracbins, 1e-5, 1);
  int nhitsfracbins = 100;
  double *hitsfracbins = GenerateLogBinning(nhitsfracbins, 1e-2, 1);

  
  hncaloclusters = std::make_shared<TH2F>("hncaloclusters", ";Energy (GeV) [MC]; CaloClusters", nenebins, enebins, ncaloclustersbins, caloclustersbins);
  honecaloclusterhitsedep= std::make_shared<TH3F>("honecaloclusterhitsedep", "Single CALOcluster events;Edep(cluster)/Edep(CALO);Hits(cluster)/hits(CALO)", nenebins, enebins, nenefracbins, enefracbins, nhitsfracbins, hitsfracbins);
  honecaloclusterhitsleak= std::make_shared<TH3F>("honecaloclusterhitsleak", "Single CALOcluster events;1 - Edep(cluster)/Edep(CALO);Hits(cluster)/hits(CALO)", nenebins, enebins, nleakfracbins, leakfracbins, nhitsfracbins, hitsfracbins);

  return true;
}

bool CaloClusteringHisto::Process() {
  const std::string routineName("CaloClusteringHisto::Process");

 //Add the ProcessStore object for this event to the event data store
  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;}
  auto caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC");
  if (!caloHits) { COUT(DEBUG) << "CaloHitsMC not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  auto caloGeoParams = globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams");
  if (!caloGeoParams) { COUT(DEBUG) << "caloGeoParams not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  auto caloClusters = _evStore->GetObject<Herd::CaloClusters>("caloClusters");
  if (!caloClusters) { COUT(DEBUG) << "caloClusters not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  auto mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
  if (!mcTruth) {COUT(ERROR) << "mcTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;return false;}
  
  double mcmom = std::sqrt(mcTruth->primaries.at(0).GetInitialMomentum() * mcTruth->primaries.at(0).GetInitialMomentum());
  double calototedep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  int calotothits = (int)caloHits->size();
  int ncaloclusters = (int)caloClusters->size();

  hncaloclusters->Fill( mcmom, ncaloclusters);


  std::vector<double> caloclusteredep;
  std::vector<int>    ncaloclusterhits;
  for( auto const& clustercalohits : *caloClusters){
    double edep = std::accumulate(clustercalohits.begin(), clustercalohits.end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
    caloclusteredep.push_back(edep);
    int nhits = (int)clustercalohits.size();
    ncaloclusterhits.push_back(nhits);
  }

  if( (int)caloclusteredep.size()==1){
    double caloclusterenefrac = caloclusteredep.at(0)/(double)calototedep;
    double caloclusterhitsfrac = ncaloclusterhits.at(0)/(double)calotothits;
    double caloclusterleakfrac = 1 - caloclusteredep.at(0)/(double)calototedep;
    honecaloclusterhitsedep->Fill(mcmom, caloclusterenefrac, caloclusterhitsfrac);
    honecaloclusterhitsleak->Fill(mcmom, caloclusterleakfrac, caloclusterhitsfrac);

  }

  return true;
}

bool CaloClusteringHisto::Finalize() {
  const std::string routineName("CaloEDepCut::Finalize");
  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  
  globStore->AddObject(hncaloclusters->GetName(),          hncaloclusters);
  globStore->AddObject(honecaloclusterhitsedep->GetName(), honecaloclusterhitsedep);
  globStore->AddObject(honecaloclusterhitsleak->GetName(), honecaloclusterhitsleak);

  return true;
}

double* CaloClusteringHisto::GenerateLogBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double log_interval = ( log10(max) - log10(min) ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = pow( 10, log10(min) + i * log_interval );
  return axis;
 }
double* CaloClusteringHisto::GenerateBinning(int nbins, double min, double max){
  double *axis = new double[nbins+1];
  double interval = ( max -min ) / nbins;
  for(int i=0; i<=nbins; i++) axis[i] = min + i*interval;
  return axis;
}
