// Example headers
#include "CaloDig.h"
#include "dataobjects/CaloHits.h"
#include "dataobjects/CaloClusters.h"
#include "dataobjects/CaloGeoParams.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloAxes.h"

// Root headers
#include "TMatrixD.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>
#include <vector>

namespace Herd{ 

RegisterAlgorithm(CaloDig);
RegisterAlgorithm(CaloDigStore);


CaloDig::CaloDig(const std::string &name) :
  Algorithm{name},
  filterenable{true}
   {
    DefineParameter("filterenable",  filterenable ); 
   }


bool CaloDig::Initialize() {
  const std::string routineName("CaloDig::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }

  _processstore = std::make_shared<CaloDigStore>("caloDigStore");

  hcalototedepSPDE = std::make_shared<TH2F>("hcalototedepSPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  hcalototedepLPDE = std::make_shared<TH2F>("hcalototedepLPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  hcalototedepPDE = std::make_shared<TH2F>("hcalototedepPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  hcaloclusteredepSPDE = std::make_shared<TH2F>("hcaloclusteredepSPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  hcaloclusteredepLPDE = std::make_shared<TH2F>("hcaloclusteredepLPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  hcaloclusteredepPDE = std::make_shared<TH2F>("hcaloclusteredepPDE",";Log_{10}(Energy (GeV));Log_{10}(Edep (GeV))",120,0,6,200,-4,6);
  
  // Setup the filter                                                                                                                                                                                                                       
  if (filterenable) SetFilterStatus(FilterStatus::ENABLED); else SetFilterStatus(FilterStatus::DISABLED);

  return true;
}

bool CaloDig::Process() {
  const std::string routineName("CaloDig::Process");

  //Add the ProcessStore object for this event to the event data store
  //_processstore->Reset();
  _processstore->Reset();
  _evStore->AddObject("caloDigStore",_processstore);

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;}


  Herd::CaloHits* caloHits = NULL;
  try { caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC"); }
  catch(Retrieval::Exception &exc) { caloHits=NULL; }
  static bool caloHits_f=true; if(caloHits_f){ COUT(INFO)<<Form("CaloDig::Process::caloHitsMC::%s",caloHits?"FOUND":"NOT-FOUND")<<ENDL; } caloHits_f=false;
  
  Herd::CaloGeoParams* caloGeoParams = NULL;
  try { caloGeoParams = globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams"); }
  catch(Retrieval::Exception &exc) { COUT(INFO) << "caloGeoParams not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  static bool caloGeoParams_f=true; if(caloGeoParams_f){ COUT(INFO)<<Form("CaloDig::Process::caloGeoParams::%s",caloGeoParams?"FOUND":"NOT-FOUND")<<ENDL; } caloGeoParams_f=false;

  Herd::CaloClusters* caloClusters = NULL;
  try { caloClusters = _evStore->GetObject<Herd::CaloClusters>("caloClusters"); }
  catch(Retrieval::Exception &exc) { caloClusters=NULL; }
  static bool caloClusters_f=true; if(caloClusters_f){ COUT(INFO)<<Form("CaloDig::Process::caloClusters::%s",caloClusters?"FOUND":"NOT-FOUND")<<ENDL; } caloClusters_f=false;
  
  Herd::MCTruth* mcTruth = NULL;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=NULL; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("CaloDig::Process::mcTruth::%s",mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  Herd::CaloAxes* caloAxes = NULL;
  try { caloAxes = _evStore->GetObject<Herd::CaloAxes>("caloAxes"); }
  catch(Retrieval::Exception &exc) { caloAxes=NULL; }
  static bool caloAxes_f=true; if(caloAxes_f){ COUT(INFO)<<Form("CaloDig::Process::caloAxes::%s",caloAxes?"FOUND":"NOT-FOUND")<<ENDL; } caloAxes_f=false;

  Herd::CaloHits* caloLPDHits = NULL;
  try { caloLPDHits = _evStore->GetObject<Herd::CaloHits>("caloLPDHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloLPDHits=NULL; }
  static bool caloLPDHits_f=true; if(caloLPDHits_f){ COUT(INFO)<<Form("CaloDig::Process::caloLPDHitsGeV::%s",caloLPDHits?"FOUND":"NOT-FOUND")<<ENDL; } caloLPDHits_f=false;

  Herd::CaloHits* caloSPDHits = NULL;
  try { caloSPDHits = _evStore->GetObject<Herd::CaloHits>("caloSPDHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloSPDHits=NULL; }
  static bool caloSPDHits_f=true; if(caloSPDHits_f){ COUT(INFO)<<Form("CaloDig::Process::caloSPDHitsGeV::%s",caloSPDHits?"FOUND":"NOT-FOUND")<<ENDL; } caloSPDHits_f=false;

  Herd::CaloHits* caloStitchedHits = NULL;
  try { caloStitchedHits = _evStore->GetObject<Herd::CaloHits>("caloHitsGeV"); }
  catch(Retrieval::Exception &exc) { caloStitchedHits=NULL; }
  static bool caloStitchedHits_f=true; if(caloStitchedHits_f){ COUT(INFO)<<Form("CaloDig::Process::caloHitsGeV::%s",caloStitchedHits?"FOUND":"NOT-FOUND")<<ENDL; } caloStitchedHits_f=false;

  float calototedepSPDE = accumulate(caloSPDHits->begin(), caloSPDHits->end(), 0.,
			     [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedepLPDE = accumulate(caloLPDHits->begin(), caloLPDHits->end(), 0.,
                             [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedepPDE = accumulate(caloStitchedHits->begin(), caloStitchedHits->end(), 0.,
                             [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  float calototedep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  hcalototedepSPDE->Fill( log10(calototedep), log10(calototedepSPDE) );
  hcalototedepLPDE->Fill( log10(calototedep), log10(calototedepLPDE) );
  hcalototedepPDE ->Fill( log10(calototedep), log10(calototedepPDE) );
  
  _processstore->calototedepSPDE = calototedepSPDE;
  _processstore->calototedepLPDE = calototedepLPDE;
  _processstore->calototedepPDE = calototedepPDE;

  int nSPD=0;
  for( auto const &hit : *caloSPDHits){
    _processstore->calopdhitsSPDE.push_back(hit.EDep());
    _processstore->calopdhitsSPDID.push_back(hit.VolumeID());   
    nSPD++;
  }

  int nLPD=0;
  for( auto const &hit : *caloLPDHits){
    _processstore->calopdhitsLPDE.push_back(hit.EDep());
    _processstore->calopdhitsLPDID.push_back(hit.VolumeID());   
    nLPD++;
  }

  int nPD=0;
  for( auto const &hit : *caloStitchedHits){
    _processstore->calopdhitsPDE.push_back(hit.EDep());
    _processstore->calopdhitsPDID.push_back(hit.VolumeID());   
    nPD++;
  }

  /*
  float calototedep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
  int calonhits =     std::accumulate(caloHits->begin(), caloHits->end(), 0.,[](int n, const Herd::Hit &hit) { if( hit.EDep()>0) return n+1; });
  _processstore->calonhits = calonhits;
  _processstore->calototedep = calototedep;
  
  _processstore->calonclusters=0;
  _processstore->caloicluster=-1;
  _processstore->caloclusteredep=0;
  _processstore->caloclusteredepall=0;
  _processstore->caloclusterhits=0;
  _processstore->caloclusterhitsall=0;

  //COUT(INFO)<<caloClusters->size()<<ENDL;
  if( caloClusters )
    if( (int)caloClusters->size()>0 )
      {
	int imaxcl=0; float maxedep=0; int maxclhits=0;
	_processstore->calonclusters = (int)caloClusters->size();
	for(int icl=0; icl<(int)caloClusters->size(); icl++){
	  auto const caloclHits = caloClusters->at(icl);
	  float _edep = std::accumulate(caloclHits.begin(), caloclHits.end(), 0.,[](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });
	  int _hits = std::accumulate(caloclHits.begin(), caloclHits.end(), 0.,[](int n, const Herd::Hit &hit) { if( hit.EDep()>0) return n+1; });
	  _processstore->caloclusteredepall += _edep;
	  _processstore->caloclusterhitsall += _hits;
	  
	  if( _edep>maxedep){ maxedep=_edep; maxclhits=_hits; imaxcl=icl; }
	}
	_processstore->caloclusteredep=maxedep;
	_processstore->caloclusterhits=maxclhits;
	_processstore->caloicluster=imaxcl;
      }
  
  //printf("%f %d\n", _processstore->calototedep, _processstore->calonhits);

  const Momentum &mcMom = mcTruth->primaries[0].InitialMomentum();
  const Point &mcPos    = mcTruth->primaries[0].InitialPosition();
  const Line mctrack(mcPos, mcMom);
  short nmiphitsontrack=0;
  double calomiptrack=0;

  std::vector<std::array<float,3>> hits;
  std::vector<std::array<float,3>> pcahits;

  auto caloAxisHits = caloHits;
  if( _processstore->caloicluster>=0 ){ *caloAxisHits = caloClusters->at(_processstore->caloicluster); }
  for( auto const &hit : *caloAxisHits){
    hcalohitsedep->Fill( log10(hit.EDep()) );
    double pathl = GetHitPathLenght(&mctrack, &hit);
    double distance = PointLineDistance(caloGeoParams->Position(hit.VolumeID()), mctrack);
    if( MipOnTrack(pathl, hit.EDep()) ) { nmiphitsontrack++; calomiptrack+=pathl; hcalohitontrackedep->Fill(log10(hit.EDep())); hcalohitontrackedeplin->Fill(hit.EDep());}
    hcalohitspathl->Fill( pathl );
    hcalopathldist->Fill(distance,pathl);
    if( distance>2.6 && pathl>0) COUT(INFO)<<distance<<" "<<pathl<<ENDL;
    if( pathl>0) hcalohitsedeppathl->Fill(hit.EDep()/pathl);
  }
  _processstore->calonmiphitsontrack = nmiphitsontrack;
  _processstore->calomiptrack = calomiptrack;


 if(caloaxes->size() != 0){
  TMatrixD R(3,3); //rotation matrix
  for(int i=0; i<3; i++){
      R[0][i] = caloaxes->at(0).GetEigenvectors()[i][RefFrame::Coo::X];
      R[1][i] = caloaxes->at(0).GetEigenvectors()[i][RefFrame::Coo::Y];
      R[2][i] = caloaxes->at(0).GetEigenvectors()[i][RefFrame::Coo::Z];
    }
  R.Invert();
  //printf("%f\t%f\t%f\n", R[0][0], R[0][1], R[0][2]);
  //printf("%f\t%f\t%f\n", R[1][0], R[1][1], R[1][2]);
  //printf("%f\t%f\t%f\n", R[2][0], R[2][1], R[2][2]);

  float cog[3];
  cog[0] = caloaxes->at(0).GetCOG()[RefFrame::Coo::X];
  cog[1] = caloaxes->at(0).GetCOG()[RefFrame::Coo::Y];
  cog[2] = caloaxes->at(0).GetCOG()[RefFrame::Coo::Z];
  //printf("%f\t%f\t%f\n", cog[0], cog[1], cog[2]);

  for( auto const &hit : *caloAxisHits){
  if(hit.EDep()>pcaedepth){
   double pathl = GetHitPathLenght(&mctrack, &hit);
   _processstore->calohitsID.push_back(hit.VolumeID());
   _processstore->calohitsX.push_back(caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::X]);
   _processstore->calohitsY.push_back(caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::Y]);
   _processstore->calohitsZ.push_back(caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::Z]);
   _processstore->calohitsE.push_back(hit.EDep());
   _processstore->calohitsPL.push_back((float)pathl);
   std::array<float,3> _hit= {caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::X], caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::Y], caloGeoParams->Position(hit.VolumeID())[RefFrame::Coo::Z]};
   hits.push_back(_hit);
  }
  }
 for (int i = 0; i < (int)hits.size(); i++) {   // loop on hits                                                                                                                               
    std::array<float,3> _pcahit = {0., 0., 0.};
    for (int j = 0; j < 3; j++) { // loop on coordinates                                                                                                                        
      for (int k = 0; k < 3; k++) { // loop on internal index xp[m] = sum_k R[m][k] * x[j]
        //if(i==0) COUT(INFO)<<i<<" "<<j<<" "<<k<<" "<<R[j][k]<<" "<<(hits.at(i)[k] - cog[k])<<" "<<_pcahit[k]<<ENDL;                                                                         
        _pcahit[j] += R[j][k] * (hits.at(i)[k] - cog[k]);
      }
    }
    pcahits.push_back(_pcahit);
    _processstore->calopcahits0.push_back(_pcahit[0]);
  _processstore->calopcahits1.push_back(_pcahit[1]);
  _processstore->calopcahits2.push_back(_pcahit[2]);
  }
 }


 hits.clear();
 pcahits.clear();

  if(calohitscutmc){
   
    float mcmom = std::sqrt(mcTruth->primaries.at(0).InitialMomentum() * mcTruth->primaries.at(0).InitialMomentum());
    if(calonhits < std::pow(10,(((1+log10(2))/3.))*std::log10(mcmom) + (2 - (1+std::log10(2))/3.)) ){ SetFilterResult(FilterResult::REJECT); }
  }
  */
return true;
}

bool CaloDig::Finalize() {
  const std::string routineName("CaloDig::Finalize");

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }

  globStore->AddObject(hcalototedepSPDE->GetName(),hcalototedepSPDE);
  globStore->AddObject(hcalototedepLPDE->GetName(),hcalototedepLPDE);
  globStore->AddObject(hcalototedepPDE->GetName(),hcalototedepPDE);
  globStore->AddObject(hcaloclusteredepSPDE->GetName(),hcaloclusteredepSPDE);
  globStore->AddObject(hcaloclusteredepLPDE->GetName(),hcaloclusteredepLPDE);
  globStore->AddObject(hcaloclusteredepPDE->GetName(),hcaloclusteredepPDE);

  return true;
}

  /*double CaloDig::PointLineDistance(const Point p, const Line l){
  //https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Vector_formulation
  Point distance = (l.Origin()- p) - (l.Direction() * (l.Origin()- p)*l.Direction());
  return sqrt( distance*distance );
}

double CaloDig::GetHitPathLenght(const Line* track, const Hit* hit){

  const std::string routineName("CaloDig::GetHitPathLenght");

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;}
  auto caloGeoParams = globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams");
  if (!caloGeoParams) { COUT(DEBUG) << "caloGeoParams not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }

  static double distmax = sqrt(3) * caloGeoParams->CubeSize()/2;
  double distance = PointLineDistance(caloGeoParams->Position(hit->VolumeID()), *track);
  if(distance>distmax) return 0; //if distance of the line from the cube center is larger than distmax - the diagonal of a cube with half-side - do not calculate the intersection: it will always be 0.  

  Plane Zpos( Point(0., 0., caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z]+caloGeoParams->CubeSize()/2), M_PI,      0. );
  Plane Zneg( Point(0., 0., caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z]-caloGeoParams->CubeSize()/2), 0.,        0. );
  Plane Xpos( Point(caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X]+caloGeoParams->CubeSize()/2, 0., 0.), M_PI / 2., 0. );
  Plane Xneg( Point(caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X]-caloGeoParams->CubeSize()/2, 0., 0.), M_PI / 2., M_PI);
  Plane Ypos( Point(0., caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y]+caloGeoParams->CubeSize()/2, 0.), M_PI / 2., M_PI * 3. / 2.);
  Plane Yneg( Point(0., caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y]-caloGeoParams->CubeSize()/2, 0.), M_PI / 2., M_PI / 2.);

  std::vector<Point> ints;
  Point intersection(0,0,0);

  intersection = Zpos.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::X]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::Y]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);
  //COUT(INFO)<<"Zpos::"<<intersection[RefFrame::Coo::X]<<" "<<intersection[RefFrame::Coo::Y]<<" "<<intersection[RefFrame::Coo::Z]<<ENDL;

  intersection = Zneg.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::X]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::Y]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);
  intersection = Xpos.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::Z]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::Y]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);
  intersection = Xneg.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::Z]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::Y]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Y])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);
  intersection = Ypos.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::Z]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::X]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);
  intersection = Yneg.Intersection(*track);
  if( fabs(intersection[RefFrame::Coo::Z]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::Z])<caloGeoParams->CubeSize()/2 &&
      fabs(intersection[RefFrame::Coo::X]-caloGeoParams->Position(hit->VolumeID())[RefFrame::Coo::X])<caloGeoParams->CubeSize()/2    ) ints.push_back(intersection);

  if( ints.size()==0) return 0;
  if( ints.size()==1) return -1;
  if( ints.size()==2) return sqrt( pow(ints.at(0)[RefFrame::Coo::X]-ints.at(1)[RefFrame::Coo::X],2) + pow(ints.at(0)[RefFrame::Coo::Y]-ints.at(1)[RefFrame::Coo::Y],2) + pow(ints.at(0)[RefFrame::Coo::Z]-ints.at(1)[RefFrame::Coo::Z],2)  );
  if( ints.size()==3) return -2;
  return -3;

}
  

int CaloDig::MipOnTrack( double pathl, double edep ){
  if( edep/pathl<0.025) return 1;
  return 0;
}
  */

//***************************

CaloDigStore::CaloDigStore(const std::string &name) :
  Algorithm{name}
   {
  }

  bool CaloDigStore::Initialize() {
  const std::string routineName("CaloDigStore::Initialize");
  Reset();
  return true;
}

  bool CaloDigStore::Process() {
  const std::string routineName("CaloDigStore::Process");
  return true;
}
  bool CaloDigStore::Finalize() {
  const std::string routineName("CaloDigStore::Finalize");
  return true;
}
bool CaloDigStore::Reset() {
  const std::string routineName("CaloDigStore::Finalize");

  calototedepSPDE = 0;
  calototedepLPDE = 0;
  calototedepPDE = 0;
  caloclusteredepSPDE = 0;
  caloclusteredepLPDE = 0;
  caloclusteredepPDE = 0;

  calopdhitsSPDE.clear();
  calopdhitsSPDID.clear();
  calopdhitsLPDE.clear();
  calopdhitsLPDID.clear();
  calopdhitsPDE.clear();
  calopdhitsPDID.clear();

  return true;
}

} //namespace Herd
