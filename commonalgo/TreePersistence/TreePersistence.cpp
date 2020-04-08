/*
 * TreePersistence.cpp
 *
 *  Created on: 4 Feb 2020
 *      Author: Nicola Mori
 */

// Example headers
#include "TreePersistence.h"
#include "../GeomAcceptance/MCtruthProcess.h"


// HerdSoftware headers
#include "dataobjects/CaloHits.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/PsdHitsColl.h"

// Root headers
#include "TFile.h"
#include "TTree.h"

// C/C++ headers
#include <numeric>

RegisterPersistence(TreePersistence);

TreePersistence::TreePersistence(const std::string &name, const std::string &output)
    : PersistenceService{name, output}, bookMCtruthProcess{false} {
  //DefineParameter("psdHitThreshold", _psdHitThreshold);
  DefineParameter("BookMCtruthProcess",bookMCtruthProcess);
}

bool TreePersistence::Connect() {
  const std::string routineName("TreePersistence::Connect");

  // Create the output file
  _outputFile = std::unique_ptr<TFile>{TFile::Open(GetOutput().data(), "RECREATE")};
  if (_outputFile->IsZombie()) { COUT(ERROR) << "Can't open output file " << GetOutput() << ENDL; return false; }
  COUT(INFO) << "CreatingOutoutFile::" << _outputFile->GetName() << ENDL;

  // Create the output tree
  _outputTree = new TTree("tree", "TTreePersistence tree");
  COUT(INFO) << "CreatingOutputTree" << ENDL;

  // Create branches for single variables
  /*_outputTree->Branch("mcPx", &(_mcMomentum[Herd::RefFrame::Coo::X]));
  _outputTree->Branch("mcPy", &(_mcMomentum[Herd::RefFrame::Coo::Y]));
  _outputTree->Branch("mcPz", &(_mcMomentum[Herd::RefFrame::Coo::Z]));
  _outputTree->Branch("mcX", &(_mcPosition[Herd::RefFrame::Coo::X]));
  _outputTree->Branch("mcY", &(_mcPosition[Herd::RefFrame::Coo::Y]));
  _outputTree->Branch("mcZ", &(_mcPosition[Herd::RefFrame::Coo::Z]));*/
  
  if(bookMCtruthProcess){
  _outputTree->Branch("mcNdiscarded", &(mcNdiscarded),"mcNdiscarded/I");
  _outputTree->Branch("mcDir", &(mcDir[0]),"mcDir[3]/F");
  _outputTree->Branch("mcCoo", &(mcCoo[0]),"mcCoo[3]/F");
  _outputTree->Branch("mcMom", &(mcMom),"mcMom/F");
  _outputTree->Branch("mcPhi", &(mcPhi),"mcPhi/F");
  _outputTree->Branch("mcCtheta", &(mcCtheta),"mcCtheta/F");
  _outputTree->Branch("mcStkintersections", &(mcStkintersections),"mcStkintersections/I");
  _outputTree->Branch("mcTracklengthcalox0", &(mcTracklengthcalox0),"mcTracklengthcalox0/F");
  _outputTree->Branch("mcTrackcaloentry", &(mcTrackcaloentry[0]),"mcTrackcaloentry[3]/F");
  _outputTree->Branch("mcTrackcaloexit", &(mcTrackcaloexit[0]),"mcTrackcaloexit[3]/F");
  _outputTree->Branch("mcTrackcaloentryplane", &(mcTrackcaloentryplane),"mcTrackcaloentryplane/I");
  _outputTree->Branch("mcTrackcaloexitplane", &(mcTrackcaloexitplane),"mcTrackcaloexitplane/I");  }

  return true;
}

bool TreePersistence::Disconnect() {
  const std::string routineName("TreePersistence::Disconnect");
  COUT(INFO) << "WritingOutputFile::" << _outputTree->GetName() << ENDL;
  _outputFile->cd();
  _outputTree->Write();
  _outputFile->Close();
  return true;
}

bool TreePersistence::BookEventObject(const std::string &objName, const std::string &objStore) {
  const std::string routineName("TreePersistence::BookEventObject");
  
  // Don't create the branch here. The tree is created in Connect, which is executed later, so at this point the tree
  // is still not available.
 
  /*
  if (objName == "psdHitsCollMC") {
    _psdStoreName = objStore;
    _isPSDBooked = true;
  
     } else {
    // The only manageable optional object is psdHitsCollMC, so raise an error if a different object is given.
    COUT(ERROR) << "Unknown object: " << objName << ENDL;
    return false;
  }
  */



  return true;
}

bool TreePersistence::EndOfEvent() {
  static const std::string routineName("TreePersistence::EndOfEvent");

  // Handle default objects
  if (!_evStore) { _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
    if (!_evStore) {  COUT(ERROR) << "Event store \"evStore\" not found." << ENDL; return false; } }

  if( bookMCtruthProcess )
  {
    auto mcTruthProcessStore = _evStore->GetObject<MCtruthProcessStore>("MCtruthProcessStore");
    if (!mcTruthProcessStore) {
      COUT(ERROR) << "MCtruthProcessStore object not found." << ENDL;
      return false;
    }

    mcNdiscarded  = mcTruthProcessStore->mcNdiscarded;
    mcDir[0] = mcTruthProcessStore->mcDir[0];
    mcDir[1] = mcTruthProcessStore->mcDir[1];
    mcDir[2] = mcTruthProcessStore->mcDir[2];
    mcCoo[0] = mcTruthProcessStore->mcCoo[0];
    mcCoo[1] = mcTruthProcessStore->mcCoo[1];
    mcCoo[2] = mcTruthProcessStore->mcCoo[2];
    mcMom = mcTruthProcessStore->mcMom;
    mcPhi = mcTruthProcessStore->mcPhi;
    mcCtheta = mcTruthProcessStore->mcCtheta;
    mcStkintersections = mcTruthProcessStore->mcStkintersections;
    mcTracklengthcalox0 = mcTruthProcessStore->mcTracklengthcalox0;
    mcTrackcaloentry[0] = mcTruthProcessStore->mcTrackcaloentry[0];
    mcTrackcaloentry[1] = mcTruthProcessStore->mcTrackcaloentry[1];
    mcTrackcaloentry[2] = mcTruthProcessStore->mcTrackcaloentry[2];
    mcTrackcaloexit[0] = mcTruthProcessStore->mcTrackcaloexit[0];
    mcTrackcaloexit[1] = mcTruthProcessStore->mcTrackcaloexit[1];
    mcTrackcaloexit[2] = mcTruthProcessStore->mcTrackcaloexit[2];
    mcTrackcaloentryplane = mcTruthProcessStore->mcTrackcaloentryplane;
    mcTrackcaloexitplane = mcTruthProcessStore->mcTrackcaloexitplane;
    }

  _outputTree->Fill();
  return true;
}
