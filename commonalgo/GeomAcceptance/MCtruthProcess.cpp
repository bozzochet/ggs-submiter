#include "MCtruthProcess.h"

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

RegisterAlgorithm(MCtruthProcess);
RegisterAlgorithm(MCtruthProcessStore);


MCtruthProcess::MCtruthProcess(const std::string &name) :
  Algorithm{name},
  printcalocubemap{false}
   {
     DefineParameter("printcalocubemap",    printcalocubemap);
   }

bool MCtruthProcess::Initialize() {
  const std::string routineName("MCtruthProcess::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
  if (!_evStore) {
    COUT(ERROR) << "Event data store not found." << ENDL;
    return false;
  }

  if(printcalocubemap) PrintCaloCubeMap();

  _processstore = std::make_shared<MCtruthProcessStore>("mcTruthProcessStore");
  COUT(INFO) << "InitializedProcessStore::" <<_processstore << ENDL;

  // Create the histogram
  // _gdiscarded       = std::make_shared<TGraph>();
  // _gdiscarded->SetNameTitle("gdiscarded","Discarded Events before simulated");
  // _hgencthetaphi = std::make_shared<TH2F>("hgencthetaphi","MCtruth Generation;cos(#theta);Phi (rad)",1000,-1,1,100,-TMath::Pi(),+TMath::Pi());
  // _hgencoo       = std::make_shared<TH3F>("hgencoo",      "MCtruth Generation;X(cm);Y(cm);Z(cm)",    100,-500,500,100,-500,500,100,-500,500);
  // _hstkintersections = std::make_shared<TH1F>("hstkintersections", "Inyersection of track with STK;Occurrence", 101,-1.5,99.5);
  // _ggencoo       = std::make_shared<TGraph2D>();
  // _ggencoo->SetNameTitle("ggencoo","Generation Coordinates;X(cm);Y(cm);Z(cm)");
  // _gcaloentry       = std::make_shared<TGraph2D>();
  // _gcaloentry->SetNameTitle("gcaloentry","CALO Entry point;X(cm);Y(cm);Z(cm)");
  // _gcaloexit       = std::make_shared<TGraph2D>();
  // _gcaloexit->SetNameTitle("gcaloexit","CALO Exit point;X(cm);Y(cm);Z(cm)");

  // _hcaloentryexitdir = std::make_shared<TH2F>("hcaloentryexitdir","CALO Entry (X) - Exit (Y)",Herd::RefFrame::NDirections+1, -0.5, Herd::RefFrame::NDirections+0.5, Herd::RefFrame::NDirections+1, -0.5, Herd::RefFrame::NDirections+0.5);
  // _hcaloentryexitdir->GetXaxis()->SetBinLabel(_hcaloentryexitdir->GetNbinsX(),"NONE");
  // for(int ibin=1; ibin<_hcaloentryexitdir->GetNbinsX(); ibin++) _hcaloentryexitdir->GetXaxis()->SetBinLabel(ibin, Herd::RefFrame::DirectionName[ibin-1].c_str());
  // _hcaloentryexitdir->GetYaxis()->SetBinLabel(_hcaloentryexitdir->GetNbinsY(),"NONE");
  // for(int ibin=1; ibin<_hcaloentryexitdir->GetNbinsY(); ibin++) _hcaloentryexitdir->GetYaxis()->SetBinLabel(ibin, Herd::RefFrame::DirectionName[ibin-1].c_str());
  
  // _hshowerlengthall  = std::make_shared<TH1F>(Form("hshowerlengthall"), Form("Shower Lenght (X0) All"),200,0,100);
  // for(int indir=0; indir < Herd::RefFrame::NDirections; indir++){
  //   for(int outdir=0; outdir < Herd::RefFrame::NDirections; outdir++){
  //     _hshowerlength[indir][outdir]  = std::make_shared<TH1F>(Form("hshowerlength_%d_%d",indir,outdir), Form("Shower Lenght (X0) [%s-%s]",Herd::RefFrame::DirectionName[indir].c_str(),Herd::RefFrame::DirectionName[outdir].c_str()),200,0,100);
  //     //COUT(DEBUG)<<Form("%s %s",_hshowerlength[indir][outdir]->GetName(),_hshowerlength[indir][outdir]->GetTitle())<<ENDL;
  //   }
  // }
  return true;
}

bool MCtruthProcess::Process() {
  const std::string routineName("MCtruthProcess::Process");

  //Add the ProcessStore object for this event to the event data store
  //_processstore->Reset();
  _evStore->AddObject("mcTruthProcessStore",_processstore);

  // auto mctruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
  // if (!mctruth) { COUT(DEBUG) << "MCTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }

  // auto calotrack = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC");
  // if (!calotrack) { COUT(DEBUG) << "TrackInfoForCalo  not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }

  observer_ptr<Herd::MCTruth> mcTruth = nullptr;
  try { mcTruth = _evStore->GetObject<Herd::MCTruth>("mcTruth"); }
  catch(Retrieval::Exception &exc) { mcTruth=NULL; }
  static bool mcTruth_f=true; if(mcTruth_f){ COUT(INFO)<<Form("%s::mcTruth::%s",routineName.c_str(),mcTruth?"FOUND":"NOT-FOUND")<<ENDL; } mcTruth_f=false;

  observer_ptr<Herd::TrackInfoForCalo> mcCaloTrack = nullptr;
  try { mcCaloTrack = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC"); }
  catch(Retrieval::Exception &exc) { mcCaloTrack=NULL; }
  static bool mcCaloTrack_f=true; if(mcCaloTrack_f){ COUT(INFO)<<Form("%s::mcCaloTrack::%s",routineName.c_str(),mcCaloTrack?"FOUND":"NOT-FOUND")<<ENDL; } mcCaloTrack_f=false;

  // _gdiscarded->SetPoint(_gdiscarded->GetN(), _gdiscarded->GetN(), mctruth->nDiscarded);

  const auto &primary = mcTruth->primaries.at(0);
  // Herd::Point gencoo = primary.InitialPosition();
  // TVector3 genmom (primary.InitialMomentum()[Herd::RefFrame::Coo::X],primary.InitialMomentum()[Herd::RefFrame::Coo::Y],primary.InitialMomentum()[Herd::RefFrame::Coo::Z]);
  // Double_t genctheta = genmom.CosTheta();
  // Double_t genphi = genmom.Phi();
  // _hgencoo->Fill(gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y],gencoo[Herd::RefFrame::Coo::Z]);
  // _ggencoo->SetPoint(_ggencoo->GetN(),gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y],gencoo[Herd::RefFrame::Coo::Z]);
  // _hgencthetaphi->Fill(genctheta,genphi);

  _processstore->mcDir[0] = primary.InitialMomentum()[Herd::RefFrame::Coo::X] / primary.InitialMomentum().Mag(); //genmom.Mag();
  _processstore->mcDir[1] = primary.InitialMomentum()[Herd::RefFrame::Coo::Y] / primary.InitialMomentum().Mag(); //genmom.Mag();genmom.Mag();
  _processstore->mcDir[2] = primary.InitialMomentum()[Herd::RefFrame::Coo::Z] / primary.InitialMomentum().Mag(); //genmom.Mag();genmom.Mag();
  _processstore->mcNdiscarded = mcTruth->nDiscarded;
  _processstore->mcCoo[0] = primary.InitialPosition()[Herd::RefFrame::Coo::X];
  _processstore->mcCoo[1] = primary.InitialPosition()[Herd::RefFrame::Coo::Y];
  _processstore->mcCoo[2] = primary.InitialPosition()[Herd::RefFrame::Coo::Z];
  _processstore->mcMom = primary.InitialMomentum().Mag();
  _processstore->mcPhi = primary.InitialMomentum().AzimuthAngle();
  _processstore->mcCtheta = cos(primary.InitialMomentum().PolarAngle());

  // COUT(INFO)<<"Phi"<<primary.InitialMomentum().AzimuthAngle()<<"\t"<<genmom.Phi()<<ENDL;
  // COUT(INFO)<<"Cth"<<cos(primary.InitialMomentum().PolarAngle())<<"\t"<<genmom.CosTheta()<<ENDL;
  // COUT(INFO)<<"Th"<<primary.InitialMomentum().PolarAngle()<<"\t"<<genmom.Theta()<<ENDL;
  // COUT(INFO)<<"Mom"<<primary.InitialMomentum().Mag()<<"\t"<<genmom.Mag()<<ENDL;
  
  Herd::RefFrame::Direction entrydir = mcCaloTrack->entrancePlane;
  Herd::RefFrame::Direction exitdir = mcCaloTrack->exitPlane;
  // _hcaloentryexitdir->Fill( entrydir==Herd::RefFrame::Direction::NONE ? _hcaloentryexitdir->GetNbinsX()-0.5 : static_cast<int>(entrydir), exitdir==Herd::RefFrame::Direction::NONE ? _hcaloentryexitdir->GetNbinsY()-0.5 : static_cast<int>(exitdir));
	
  //  float mcCaloTracklengthx0=-1;
  if( !(entrydir==Herd::RefFrame::Direction::NONE && exitdir==Herd::RefFrame::Direction::NONE) ){
    // 	  _gcaloentry->SetPoint(_gcaloentry->GetN(), mcCaloTrack->entrance[Herd::RefFrame::Coo::X],mcCaloTrack->entrance[Herd::RefFrame::Coo::Y],mcCaloTrack->entrance[Herd::RefFrame::Coo::Z]);
    // 	  _gcaloexit->SetPoint(_gcaloexit->GetN(), mcCaloTrack->exit[Herd::RefFrame::Coo::X],mcCaloTrack->exit[Herd::RefFrame::Coo::Y],mcCaloTrack->exit[Herd::RefFrame::Coo::Z]);
    // 	  _hshowerlength[static_cast<int>(entrydir)][static_cast<int>(exitdir)]->Fill(mcCaloTrack->trackLengthCaloX0);
    // _hshowerlengthall->Fill(mcCaloTrack->trackLengthCaloX0);
    // mcCaloTracklengthx0 = mcCaloTrack->trackLengthCaloX0;

    _processstore->mcTracklengthcalox0 = mcCaloTrack->trackLengthCaloX0;
    _processstore->mcTracklengthlysox0 = mcCaloTrack->trackLengthLYSOX0;
    _processstore->mcTracklenghtexactlysocm = mcCaloTrack->trackLengthExactLYSOCm;
    _processstore->mcTracklenghtlysoafii = mcCaloTrack->trackLengthLYSOAfterInteractionCm;

    _processstore->mcTrackcaloentry[0] = mcCaloTrack->entrance[Herd::RefFrame::Coo::X];
    _processstore->mcTrackcaloentry[1] = mcCaloTrack->entrance[Herd::RefFrame::Coo::Y];
    _processstore->mcTrackcaloentry[2] = mcCaloTrack->entrance[Herd::RefFrame::Coo::Z];
    _processstore->mcTrackcaloexit[0] = mcCaloTrack->exit[Herd::RefFrame::Coo::X];
    _processstore->mcTrackcaloexit[1] = mcCaloTrack->exit[Herd::RefFrame::Coo::Y];
    _processstore->mcTrackcaloexit[2] = mcCaloTrack->exit[Herd::RefFrame::Coo::Z];
    _processstore->mcTrackcaloentryplane = static_cast<int>(entrydir);
    _processstore->mcTrackcaloexitplane = static_cast<int>(exitdir);

	  }

  //Hadronic interaction
  _processstore->mcHashadint = mcTruth->primaries[0].HadronicInelasticInteractionIsPresent();
  if (_processstore->mcHashadint) {
    _processstore->mcFirsthadint[0] = mcTruth->primaries[0].HadronicInelasticInteraction().intPosition[Herd::RefFrame::Coo::X];
    _processstore->mcFirsthadint[1] = mcTruth->primaries[0].HadronicInelasticInteraction().intPosition[Herd::RefFrame::Coo::Y];
    _processstore->mcFirsthadint[2] = mcTruth->primaries[0].HadronicInelasticInteraction().intPosition[Herd::RefFrame::Coo::Z];
  }
  else{
    _processstore->mcFirsthadint[0] = -999;
    _processstore->mcFirsthadint[1] = -999;
    _processstore->mcFirsthadint[2] = -999;
  }

  return true;
}

bool MCtruthProcess::Finalize() {
  const std::string routineName("MCtruthProcess::Finalize");

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;return false;}

  //globStore->AddObject(_hgencoo->GetName(), _hgencoo);
  //globStore->AddObject(_hgencthetaphi->GetName(),_hgencthetaphi);
  //globStore->AddObject(_ggencoo->GetName(),_ggencoo);
  //globStore->AddObject(_gcaloentry->GetName(),_gcaloentry);
  //globStore->AddObject(_gcaloexit->GetName(),_gcaloexit);
  //globStore->AddObject(_gdiscarded->GetName(),_gdiscarded);
  //globStore->AddObject(_hstkintersections->GetName(),_hstkintersections);
  //globStore->AddObject(_hshowerlengthall->GetName(), _hshowerlengthall);
  //globStore->AddObject(_hcaloentryexitdir->GetName(),_hcaloentryexitdir);
  for(int indir=0; indir < Herd::RefFrame::NDirections; indir++){
    for(int outdir=0; outdir < Herd::RefFrame::NDirections; outdir++){
      //globStore->AddObject(_hshowerlength[indir][outdir]->GetName(), _hshowerlength[indir][outdir]);
    }
  } 


  return true;
}

void MCtruthProcess::PrintCaloCubeMap(){
  const std::string routineName("MCtruthProcess::PrintCaloCubeMap");
  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;}
  auto caloGeoParams = globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams");
  if (!caloGeoParams) {COUT(ERROR) << "Event data store not found." << ENDL;}
  else
  {
    for(unsigned int icube=0; icube<caloGeoParams->NCubes(); icube++){
    printf("[%04u]\t%.2f\t%.2f\t%.2f\t%.2f\n", icube, caloGeoParams->CubeSize(), caloGeoParams->Position(icube)[Herd::RefFrame::Coo::X],caloGeoParams->Position(icube)[Herd::RefFrame::Coo::Y],caloGeoParams->Position(icube)[Herd::RefFrame::Coo::Z]);
  }
  }

}


//***************************

MCtruthProcessStore::MCtruthProcessStore(const std::string &name) :
  Algorithm{name}
   {
  }

  bool MCtruthProcessStore::Initialize() {
  const std::string routineName("MCtruthProcessStore::Initialize");
  Reset();
  return true;
}

  bool MCtruthProcessStore::Process() {
  const std::string routineName("MCtruthProcessStore::Process");
  return true;
}
  bool MCtruthProcessStore::Finalize() {
  const std::string routineName("MCtruthProcessStore::Finalize");
  return true;
}
bool MCtruthProcessStore::Reset() {
  const std::string routineName("MCtruthProcessStore::Finalize");

  mcNdiscarded = -1;
  for(int idir=0; idir<3; idir++) mcDir[idir] = -999.;
  for(int idir=0; idir<3; idir++) mcCoo[idir] = -999.;
  mcMom = -999.;
  mcPhi = -999.;
  mcCtheta = -999.;
  mcTracklengthcalox0 = -999.;
  mcTracklengthlysox0 = -999.;
  mcTracklenghtexactlysocm = -999.;
  mcTracklenghtlysoafii = -999.;

  for(int idir=0; idir<3; idir++) mcTrackcaloentry[idir] = -999.;
  for(int idir=0; idir<3; idir++) mcTrackcaloexit[idir] = -999.;
  mcTrackcaloentryplane = -1;
  mcTrackcaloexitplane = -1;

  return true;
}

