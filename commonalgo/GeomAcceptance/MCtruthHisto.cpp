/*
 * CaloEDepHisto.cpp
 *
 *  Created on: 11 Feb 2019
 *      Author: Nicola Mori
 */

// Example headers
#include "MCtruthHisto.h"





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

RegisterAlgorithm(MCtruthHisto);

MCtruthHisto::MCtruthHisto(const std::string &name) :
  Algorithm{name}//,//quando esegui il xostruttore, esegui prima il costruttore di algorithm
  // _axis{100., 0., 100.} { 
  // DefineParameter("axis", _axis);
  {
}

bool MCtruthHisto::Initialize() {
  const std::string routineName("MCtruthHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
  if (!_evStore) {
    COUT(ERROR) << "Event data store not found." << ENDL;
    return false;
  }

  // Create the histogram
  _hgencthetaphi = std::make_shared<TH2F>("hgencthetaphi","MCtruth Generation;cos(#theta);Phi (rad)",1000,-1,1,100,-TMath::Pi(),+TMath::Pi());
  _hgencoo       = std::make_shared<TH3F>("hgencoo",      "MCtruth Generation;X(cm);Y(cm);Z(cm)",    100,-500,500,100,-500,500,100,-500,500);
  _ggencoo       = std::make_shared<TGraph2D>();
  _ggencoo->SetNameTitle("ggencoo","Generation Coordinates;X(cm);Y(cm);Z(cm)");
  _gcaloentry       = std::make_shared<TGraph2D>();
  _gcaloentry->SetNameTitle("gcaloentry","CALO Entry point;X(cm);Y(cm);Z(cm)");
  _gcaloexit       = std::make_shared<TGraph2D>();
  _gcaloexit->SetNameTitle("gcaloexit","CALO Exit point;X(cm);Y(cm);Z(cm)");

  for(int indir=0; indir < Herd::RefFrame::NDirections; indir++){
    for(int outdir=0; outdir < Herd::RefFrame::NDirections; outdir++){
      _hshowerlength[indir][outdir]  = std::make_shared<TH1F>(Form("hshowerlength_%d_%d",indir,outdir), Form("Shower Lenght [%s-%s]",Herd::RefFrame::DirectionName[indir].c_str(),Herd::RefFrame::DirectionName[outdir].c_str()),200,0,200);
      COUT(DEBUG)<<Form("%s %s",_hshowerlength[indir][outdir]->GetName(),_hshowerlength[indir][outdir]->GetTitle())<<ENDL;
    }
  }
  return true;
}

bool MCtruthHisto::Process() {
  const std::string routineName("MCtruthHisto::Process");

  // auto caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC");
  // if (!caloHits) {
  //   COUT() << "CALO hits not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;
  //   return false;
  // }

  // float totEDep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,
  //                                 [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });

  // _histo->Fill(totEDep);

  auto mctruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
  auto calotrack = _evStore->GetObject<Herd::TrackInfoForCalo>("trackInfoForCaloMC");

  if (!calotrack) { COUT(DEBUG) << "CALO track not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;
     return false; }

  const auto &primary = mctruth->primaries.at(0);
  Herd::Point gencoo = primary.initialPosition;
	TVector3 genmom (primary.initialMomentum[Herd::RefFrame::Coo::X],primary.initialMomentum[Herd::RefFrame::Coo::Y],primary.initialMomentum[Herd::RefFrame::Coo::Z]);
  Double_t genctheta = genmom.CosTheta();
  Double_t genphi = genmom.Phi();
  Double_t mom = genmom.Mag();
  _hgencoo->Fill(gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y],gencoo[Herd::RefFrame::Coo::Z]);
  _ggencoo->SetPoint(_ggencoo->GetN(),gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y],gencoo[Herd::RefFrame::Coo::Z]);
  _hgencthetaphi->Fill(genctheta,genphi);

	Herd::RefFrame::Direction entrydir = calotrack->entrancePlane;
	Herd::RefFrame::Direction exitdir = calotrack->exitPlane;
	if( !(entrydir==Herd::RefFrame::Direction::NONE && exitdir==Herd::RefFrame::Direction::NONE) ){
		_gcaloentry->SetPoint(_gcaloentry->GetN(), calotrack->entrance[Herd::RefFrame::Coo::X],calotrack->entrance[Herd::RefFrame::Coo::Y],calotrack->entrance[Herd::RefFrame::Coo::Z]);
		_gcaloexit->SetPoint(_gcaloexit->GetN(), calotrack->exit[Herd::RefFrame::Coo::X],calotrack->exit[Herd::RefFrame::Coo::Y],calotrack->exit[Herd::RefFrame::Coo::Z]);
		COUT(DEBUG)<<static_cast<int>(entrydir)<<" "<<static_cast<int>(exitdir)<<ENDL;
		COUT(DEBUG)<<Herd::RefFrame::DirectionName[static_cast<int>(entrydir)]<<" "<<Herd::RefFrame::DirectionName[static_cast<int>(exitdir)]<<ENDL;
		_hshowerlength[static_cast<int>(entrydir)][static_cast<int>(exitdir)]->Fill(calotrack->trackLengthCaloCm);
	}
	
  return true;
}

bool MCtruthHisto::Finalize() {
  const std::string routineName("MCtruthHisto::Finalize");

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {
    COUT(ERROR) << "Global data store not found." << ENDL;
    return false;
  }

  globStore->AddObject(_hgencoo->GetName(), _hgencoo);
  globStore->AddObject(_hgencthetaphi->GetName(),_hgencthetaphi);
  globStore->AddObject(_ggencoo->GetName(),_ggencoo);
  globStore->AddObject(_gcaloentry->GetName(),_gcaloentry);
  globStore->AddObject(_gcaloexit->GetName(),_gcaloexit);

  for(int indir=0; indir < Herd::RefFrame::NDirections; indir++){
    for(int outdir=0; outdir < Herd::RefFrame::NDirections; outdir++){
      globStore->AddObject(_hshowerlength[indir][outdir]->GetName(), _hshowerlength[indir][outdir]);
    }
  } 


  return true;
}


