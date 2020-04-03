/*
 * CaloEDepHisto.cpp
 *
 *  Created on: 11 Feb 2019
 *      Author: Nicola Mori
 */

// Example headers
#include "MCtruthHisto.h"

// HerdSoftware headers
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloHits.h"


// Root headers
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TVector3.h"

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
  _hgencoo       = std::make_shared<TH3F>("hgencoo",      "MCtruth Generation;X(cm);Y(cm);Z(cm)",    1000,-500,500,1000,-500,500,1000,-500,500);
  //_hgencoo       = std::make_shared<TH2F>("hgencoo",      "MCtruth Generation;X(cm);Y(cm)",    1000,-500,500,1000,-500,500);
  
  return true;
}

bool MCtruthHisto::Process() {
  const std::string routineName("MCtruthHisto::Process");

  // auto caloHits = _evStore->GetObject<Herd::CaloHits>("caloHitsMC");
  // if (!caloHits) {
  //   COUT(ERROR) << "CALO hits not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;
  //   return false;
  // }

  // float totEDep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,
  //                                 [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });

  // _histo->Fill(totEDep);

  auto mctruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
  const auto &primary = mctruth->primaries.at(0);
  Herd::Point gencoo = primary.initialPosition;
  TVector3 genmom (primary.initialMomentum[Herd::RefFrame::Coo::X],primary.initialMomentum[Herd::RefFrame::Coo::Y],primary.initialMomentum[Herd::RefFrame::Coo::Z]);
  //TVector3 genmom (primary.initialMomentum.at(0),primary.initialMomentum.at(1),primary.initialMomentum.at(2));
  Double_t genctheta = genmom.CosTheta();
  Double_t genphi = genmom.Phi();
  Double_t mom = genmom.Mag();
  _hgencoo->Fill(gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y],gencoo[Herd::RefFrame::Coo::Z]);
  //_hgencoo->Fill(gencoo[Herd::RefFrame::Coo::X],gencoo[Herd::RefFrame::Coo::Y]);
  _hgencthetaphi->Fill(genctheta,genphi);

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

  return true;
}
