// Example headers
#include "TreePersistence.h"
#include "../GeomAcceptance/MCtruthProcess.h"
#include "../GeomAcceptance/CaloGeomFidVolume.h"
#include "../Calo/CaloGlob.h"
#include "../Calo/CaloAxis.h"



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
RegisterAlgorithm(TreePersistenceHelper);


TreePersistence::TreePersistence(const std::string &name, const std::string &output)
    : PersistenceService{name, output},
    bookMCtruthProcess{false},
    bookCaloGeomFidVolume{false}, 
    bookCaloGlob{false},
    bookCaloAxis{false}
    {
  //DefineParameter("psdHitThreshold", _psdHitThreshold);
  DefineParameter("BookMCtruthProcess",bookMCtruthProcess);
  DefineParameter("BookCaloGeomFidVolume",bookCaloGeomFidVolume);
  DefineParameter("BookCaloGlob",bookCaloGlob);
  DefineParameter("BookCaloAxis",bookCaloAxis);
  

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
  
  if(bookMCtruthProcess){
  _outputTree->Branch("mcNdiscarded",          &(mcNdiscarded),          "mcNdiscarded/I");
  _outputTree->Branch("mcDir",                 &(mcDir[0]),              "mcDir[3]/F");
  _outputTree->Branch("mcCoo",                 &(mcCoo[0]),              "mcCoo[3]/F");
  _outputTree->Branch("mcMom",                 &(mcMom),                 "mcMom/F");
  _outputTree->Branch("mcPhi",                 &(mcPhi),                 "mcPhi/F");
  _outputTree->Branch("mcCtheta",              &(mcCtheta),              "mcCtheta/F");
  _outputTree->Branch("mcStkintersections",    &(mcStkintersections),    "mcStkintersections/I");
  _outputTree->Branch("mcTracklengthcalox0",   &(mcTracklengthcalox0),   "mcTracklengthcalox0/F");
  _outputTree->Branch("mcTracklengthlysox0",   &(mcTracklengthlysox0),   "mcTracklengthlysox0/F");
  _outputTree->Branch("mcTrackcaloentry",      &(mcTrackcaloentry[0]),   "mcTrackcaloentry[3]/F");
  _outputTree->Branch("mcTrackcaloexit",       &(mcTrackcaloexit[0]),    "mcTrackcaloexit[3]/F");
  _outputTree->Branch("mcTrackcaloentryplane", &(mcTrackcaloentryplane), "mcTrackcaloentryplane/I");
  _outputTree->Branch("mcTrackcaloexitplane",  &(mcTrackcaloexitplane),  "mcTrackcaloexitplane/I");
  }

  if(bookCaloGeomFidVolume){
  _outputTree->Branch("calofidvolalpha",     &(calofidvolalpha),    "calofidvolalpha/F");
  _outputTree->Branch("calofidvolpass",      &(calofidvolpass),     "calofidvolpass/O");
  _outputTree->Branch("calofidvolxpos",      &(calofidvolxpos),     "calofidvolxpos/S");
  _outputTree->Branch("calofidvolxneg",      &(calofidvolxneg),     "calofidvolxneg/S");
  _outputTree->Branch("calofidvolypos",      &(calofidvolypos),     "calofidvolypos/S");
  _outputTree->Branch("calofidvolyneg",      &(calofidvolyneg),     "calofidvolyneg/S");
  _outputTree->Branch("calofidvolzpos",      &(calofidvolzpos),     "calofidvolzpos/S");
  _outputTree->Branch("calofidvolzneg",      &(calofidvolzneg),     "calofidvolzneg/S");
  _outputTree->Branch("calofidvolxnegyneg",  &(calofidvolxnegyneg), "calofidvolxnegyneg/S");
  _outputTree->Branch("calofidvolxposyneg",  &(calofidvolxposyneg), "calofidvolxposyneg/S");
  _outputTree->Branch("calofidvolxnegypos",  &(calofidvolxnegypos), "calofidvolxnegypos/S");
  _outputTree->Branch("calofidvolxposypos",  &(calofidvolxposypos), "calofidvolxposypos/S");
  _outputTree->Branch("calofidvolxposEntry",      &(calofidvolxposEntry[0]),     "calofidvolxposEntry[2][3]/F");
  _outputTree->Branch("calofidvolxnegEntry",      &(calofidvolxnegEntry[0]),     "calofidvolxnegEntry[2][3]/F");
  _outputTree->Branch("calofidvolyposEntry",      &(calofidvolyposEntry[0]),     "calofidvolyposEntry[2][3]/F");
  _outputTree->Branch("calofidvolynegEntry",      &(calofidvolynegEntry[0]),     "calofidvolynegEntry[2][3]/F");
  _outputTree->Branch("calofidvolzposEntry",      &(calofidvolzposEntry[0]),     "calofidvolzposEntry[2][3]/F");
  _outputTree->Branch("calofidvolznegEntry",      &(calofidvolznegEntry[0]),     "calofidvolznegEntry[2][3]/F");
  _outputTree->Branch("calofidvolxnegynegEntry",  &(calofidvolxnegynegEntry[0]), "calofidvolxnegynegEntry[2][3]/F");
  _outputTree->Branch("calofidvolxposynegEntry",  &(calofidvolxposynegEntry[0]), "calofidvolxposynegEntry[2][3]/F");
  _outputTree->Branch("calofidvolxnegyposEntry",  &(calofidvolxnegyposEntry[0]), "calofidvolxnegyposEntry[2][3]/F");
  _outputTree->Branch("calofidvolxposyposEntry",  &(calofidvolxposyposEntry[0]), "calofidvolxposyposEntry[2][3]/F");
  }

if(bookCaloGlob){
  _outputTree->Branch("calonhits",     &(calonhits),    "calonhits/I");
  _outputTree->Branch("calototedep",   &(calototedep),  "calototedep/F");
}

if(bookCaloAxis){
  _outputTree->Branch("caloaxiscog",     &(caloaxiscog[0]),    "caloaxiscog[3]/F");
  _outputTree->Branch("caloaxisdir",     &(caloaxisdir[0]),    "caloaxisdir[3]/F");
  _outputTree->Branch("caloaxiseigval",  &(caloaxiseigval[0]), "caloaxiseigval[3]/F");
  _outputTree->Branch("caloaxiseigvec",  &(caloaxiseigvec[0]), "caloaxiseigvec[3][3]/F");
}
 
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

bool TreePersistence::BeginningOfEvent() {
  static const std::string routineName("TreePersistence::EndOfEvent");

  // Handle default objects
  if (!_evStore) { _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
  if (!_evStore) {COUT(ERROR) << "EventStore::\"evStore\"::NotFound." << ENDL; return false; } }
  
  return true;
}
      
bool TreePersistence::EndOfEvent() {
  static const std::string routineName("TreePersistence::EndOfEvent");

  // Handle default objects
  if (!_evStore) { _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
    if (!_evStore) {COUT(ERROR) << "EventStore::\"evStore\"::NotFound." << ENDL; return false; } }

  if( bookMCtruthProcess )
  {
    auto mcTruthProcessStore = _evStore->GetObject<MCtruthProcessStore>("MCtruthProcessStore");
    if(mcTruthProcessStore){
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
      mcTracklengthlysox0 = mcTruthProcessStore->mcTracklengthlysox0;
      mcTrackcaloentry[0] = mcTruthProcessStore->mcTrackcaloentry[0];
      mcTrackcaloentry[1] = mcTruthProcessStore->mcTrackcaloentry[1];
      mcTrackcaloentry[2] = mcTruthProcessStore->mcTrackcaloentry[2];
      mcTrackcaloexit[0] = mcTruthProcessStore->mcTrackcaloexit[0];
      mcTrackcaloexit[1] = mcTruthProcessStore->mcTrackcaloexit[1];
      mcTrackcaloexit[2] = mcTruthProcessStore->mcTrackcaloexit[2];
      mcTrackcaloentryplane = mcTruthProcessStore->mcTrackcaloentryplane;
      mcTrackcaloexitplane = mcTruthProcessStore->mcTrackcaloexitplane;
      //
      mcTruthProcessStore->Reset();
    }
  }

if( bookCaloGeomFidVolume )
  {
    auto caloGeomFidVolumeStore = _evStore->GetObject<CaloGeomFidVolumeStore>("caloGeomFidVolumeStore");
    if(caloGeomFidVolumeStore){
      calofidvolalpha = caloGeomFidVolumeStore->calofidvolalpha;
      calofidvolpass = caloGeomFidVolumeStore->calofidvolpass;
      calofidvolxpos = caloGeomFidVolumeStore->calofidvolxpos;
      calofidvolxneg = caloGeomFidVolumeStore->calofidvolxneg;
      calofidvolypos = caloGeomFidVolumeStore->calofidvolypos;
      calofidvolyneg = caloGeomFidVolumeStore->calofidvolyneg;
      calofidvolzpos = caloGeomFidVolumeStore->calofidvolzpos;
      calofidvolzneg = caloGeomFidVolumeStore->calofidvolzneg;
      calofidvolxnegyneg = caloGeomFidVolumeStore->calofidvolxnegyneg;
      calofidvolxposyneg = caloGeomFidVolumeStore->calofidvolxposyneg;
      calofidvolxnegypos = caloGeomFidVolumeStore->calofidvolxnegypos;
      calofidvolxposypos = caloGeomFidVolumeStore->calofidvolxposypos;
      for(int ii=0; ii<2; ii++){
        for(int jj=0; jj<3; jj++){
        calofidvolxposEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxposEntry[ii][jj];
        calofidvolxnegEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxnegEntry[ii][jj];
        calofidvolyposEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolyposEntry[ii][jj];
        calofidvolynegEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolynegEntry[ii][jj];
        calofidvolzposEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolzposEntry[ii][jj];
        calofidvolznegEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolznegEntry[ii][jj];
        calofidvolxnegynegEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxnegynegEntry[ii][jj];
        calofidvolxposynegEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxposynegEntry[ii][jj];
        calofidvolxnegyposEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxnegyposEntry[ii][jj];
        calofidvolxposyposEntry[ii][jj] = caloGeomFidVolumeStore->calofidvolxposyposEntry[ii][jj];
        }
      }
      //
      caloGeomFidVolumeStore->Reset();
   }
  }
  if(bookCaloGlob){
    auto caloGlobStore = _evStore->GetObject<CaloGlobStore>("caloGlobStore");
    if(caloGlobStore)
    {
      calonhits = caloGlobStore->calonhits;
      calototedep = caloGlobStore->calototedep;
      //
      caloGlobStore->Reset();
     }
  }

  if(bookCaloAxis){
    auto caloAxisStore = _evStore->GetObject<Herd::CaloAxisStore>("CaloAxisStore");
    if(caloAxisStore)
    {
      for(int i=0; i<3; i++){
        caloaxiscog[i] = caloAxisStore->caloaxiscog[i];
        caloaxisdir[i] = caloAxisStore->caloaxisdir[i];
        caloaxiseigval[i] = caloAxisStore->caloaxiseigval[i];

          for(int j=0; j<3; j++)
           {
          caloaxiseigvec[i][j] = caloAxisStore->caloaxiseigvec[i][j];
       }
     }
      //
      caloAxisStore->Reset();
     }
  }

auto fillthisevent = _evStore->GetObject<bool>("FillTreeThisEvent");
if(fillthisevent){
  _outputTree->Fill();
}

return true;
}

//***************************

TreePersistenceHelper::TreePersistenceHelper(const std::string &name) :
  Algorithm{name}
  {
    DefineParameter("FillTreeThisEvent", filltreethisevent);
  }

  bool TreePersistenceHelper::Initialize() {
  const std::string routineName("TreePersistenceHelper::Initialize");
  //fillflag = std::make_shared<bool>(false);
  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL;  return false; }
  
  return true;
}

  bool TreePersistenceHelper::Process() {
  const std::string routineName("TreePersistenceHelper::Process");
  if(filltreethisevent) _evStore->AddObject("FillTreeThisEvent",std::make_shared<bool>(true));//"FillTreeThisEvent");
  return true;
}
  bool TreePersistenceHelper::Finalize() {

  const std::string routineName("TreePersistenceHelper::Finalize");
  return true;
}
