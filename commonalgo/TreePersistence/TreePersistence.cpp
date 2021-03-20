// Example headers
#include "TreePersistence.h"
#include "../GeomAcceptance/MCtruthProcess.h"
#include "../GeomAcceptance/CaloGeomFidVolume.h"
#include "../Calo/CaloGlob.h"
#include "../Calo/CaloDig.h"
#include "../Calo/CaloAxisProcess.h"



// HerdSoftware headers
#include "dataobjects/CaloHits.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/PsdHitsColl.h"

// Root headers
#include "TFile.h"
#include "TTree.h"
#include <Compression.h>

// C/C++ headers
#include <numeric>



RegisterPersistence(TreePersistence);
RegisterAlgorithm(TreePersistenceHelper);


TreePersistence::TreePersistence(const std::string &name, const std::string &output)
    : PersistenceService{name, output},
  bookMCtruthProcess{false},
  bookCaloGeomFidVolume{false}, 
  bookCaloGlob{false},
  bookCaloDig{false},
  bookCaloAxis{false}
    {
      DefineParameter("BookMCtruthProcess",bookMCtruthProcess);
      DefineParameter("BookCaloGeomFidVolume",bookCaloGeomFidVolume);
      DefineParameter("BookCaloGlob",bookCaloGlob);
      DefineParameter("BookCaloDig",bookCaloDig);
      DefineParameter("BookCaloAxis",bookCaloAxis);
      
      
    }

bool TreePersistence::Connect() {
  const std::string routineName("TreePersistence::Connect");

  // Create the output file
  int complevel = ROOT::CompressionSettings(ROOT::kLZMA, 2);
  _outputFile = std::unique_ptr<TFile>{TFile::Open(GetOutput().data(), "RECREATE", "outputfile", complevel)};
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
  _outputTree->Branch("mcTracklengthcalox0",   &(mcTracklengthcalox0),   "mcTracklengthcalox0/F");
  _outputTree->Branch("mcTracklengthlysox0",   &(mcTracklengthlysox0),   "mcTracklengthlysox0/F");
  _outputTree->Branch("mcTrackcaloentry",      &(mcTrackcaloentry[0]),   "mcTrackcaloentry[3]/F");
  _outputTree->Branch("mcTrackcaloexit",       &(mcTrackcaloexit[0]),    "mcTrackcaloexit[3]/F");
  _outputTree->Branch("mcTrackcaloentryplane", &(mcTrackcaloentryplane), "mcTrackcaloentryplane/I");
  _outputTree->Branch("mcTrackcaloexitplane",  &(mcTrackcaloexitplane),  "mcTrackcaloexitplane/I");
  _outputTree->Branch("mcTracklenghtexactlysocm",  &(mcTracklenghtexactlysocm),  "mcTracklenghtexactlysocm/F");
  _outputTree->Branch("mcTracklenghtlysoafii",  &(mcTracklenghtlysoafii),  "mcTracklenghtlysoafii/F");
  _outputTree->Branch("mcFirsthadint",         &(mcFirsthadint[0]),          "mcFirsthadint[3]/F");
  _outputTree->Branch("mcHashadint",         &(mcHashadint),          "mcHashadint/O");

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
  _outputTree->Branch("calonhits",           &(calonhits),          "calonhits/I");
  _outputTree->Branch("calototedep",         &(calototedep),         "calototedep/F");
  _outputTree->Branch("calonclusters",       &(calonclusters),       "calonclusters/I");
  _outputTree->Branch("caloicluster",        &(caloicluster),        "caloicluster/I");
  _outputTree->Branch("caloclusteredep",     &(caloclusteredep),     "caloclusteredep/F");
  _outputTree->Branch("caloclusteredepall",  &(caloclusteredepall),  "caloclusteredepall/F");
  _outputTree->Branch("caloclusterhits",     &(caloclusterhits),     "caloclusterhits/I");
  _outputTree->Branch("caloclusterhitsall",  &(caloclusterhitsall),  "caloclusterhitsall/I");
  _outputTree->Branch("calonmiphitsontrack", &(calonmiphitsontrack), "calonmiphitsontrack/S");
  _outputTree->Branch("calomiptrack",        &(calomiptrack),        "calomiptrack/F");
}

 if(bookCaloDig){
   _outputTree->Branch("calototedepSPDE",        &(calototedepSPDE),        "calototedepSPDE/F");
   _outputTree->Branch("calototedepLPDE",        &(calototedepLPDE),        "calototedepLPDE/F");
   _outputTree->Branch("calototedepPDE",         &(calototedepPDE),         "calototedepPDE/F");
   _outputTree->Branch("caloclusteredepSPDE",    &(caloclusteredepSPDE),    "caloclusteredepSPDE/F");
   _outputTree->Branch("caloclusteredepLPDE",    &(caloclusteredepLPDE),    "caloclusteredepLPDE/F");
   _outputTree->Branch("caloclusteredepPDE",     &(caloclusteredepPDE),     "caloclusteredepPDE/F");
   _outputTree->Branch("calopdhitsSPDE",  &calopdhitsSPDE);
   _outputTree->Branch("calopdhitsSPDID",  &calopdhitsSPDID);
   _outputTree->Branch("calopdhitsLPDE",  &calopdhitsLPDE);
   _outputTree->Branch("calopdhitsLPDID",  &calopdhitsLPDID);
   _outputTree->Branch("calopdhitsPDE",  &calopdhitsPDE);
   _outputTree->Branch("calopdhitsPDID",  &calopdhitsPDID);
 }

if(bookCaloAxis){
  _outputTree->Branch("caloaxishits",    &(caloaxishits),      "caloaxishits/s");
  _outputTree->Branch("caloaxiscog",     &(caloaxiscog[0]),    "caloaxiscog[3]/F");
  _outputTree->Branch("caloaxissigma",   &(caloaxissigma[0]),  "caloaxissigma[3]/F");
  _outputTree->Branch("caloaxisskew",    &(caloaxisskew[0]),   "caloaxisskew[3]/F");
  _outputTree->Branch("caloaxiskurt",    &(caloaxiskurt[0]),   "caloaxiskurt[3]/F");
  _outputTree->Branch("caloaxisdir",     &(caloaxisdir[0]),    "caloaxisdir[3]/F");
  _outputTree->Branch("caloaxiseigval",  &(caloaxiseigval[0]), "caloaxiseigval[3]/F");
  _outputTree->Branch("caloaxiseigvec",  &(caloaxiseigvec[0]), "caloaxiseigvec[3][3]/F");
  _outputTree->Branch("caloaxisentryhit",  &(caloaxisentryhit[0]), "caloaxisentryhit[3]/F");
  _outputTree->Branch("caloaxisexithit",  &(caloaxisexithit[0]), "caloaxisexithit[3]/F");
  _outputTree->Branch("caloaxispathlengthhit",  &(caloaxispathlengthhit), "caloaxispathlengthhit/F");


/*  _outputTree->Branch("calohits",        "std::vector< std::array<float, 4> >",      &calohits);
  _outputTree->Branch("calopcahits",     "std::vector< std::array<float, 4> >",      &calopcahits);
*/
  _outputTree->Branch("calohitsID",  &calohitsID);
  _outputTree->Branch("calohitsX",  &calohitsX);
  _outputTree->Branch("calohitsY",  &calohitsY);
  _outputTree->Branch("calohitsZ",  &calohitsZ);
  _outputTree->Branch("calohitsE",  &calohitsE);
  _outputTree->Branch("calohitsPL",  &calohitsPL);
  _outputTree->Branch("calopcahits0",  &calopcahits0);
  _outputTree->Branch("calopcahits1",  &calopcahits1);
  _outputTree->Branch("calopcahits2",  &calopcahits2);
}
 
  TObjArray *obj = _outputTree->GetListOfBranches();
  for (int ii = 0; ii < obj->GetEntries(); ii++)
  {
    TBranch *branch = (TBranch *)(obj->At(ii));
    branch->SetCompressionLevel(6);
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
 


  return true;
}

bool TreePersistence::BeginOfEvent() {
  static const std::string routineName("TreePersistence::BeginOfEvent");

  // Handle default objects
  if (!_evStore) { _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
  if (!_evStore) {COUT(ERROR) << "EventStore::\"evStore\"::NotFound." << ENDL; return false; } }

  //Add flag to be eventually set true if this event will be filled
  _evStore->AddObject("FillFlag",std::make_shared<bool>(false));

  return true;
}
      
bool TreePersistence::EndOfEvent() {
  static const std::string routineName("TreePersistence::EndOfEvent");

  // Handle default objects
  if (!_evStore) { _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
    if (!_evStore) {COUT(ERROR) << "EventStore::\"evStore\"::NotFound." << ENDL; return false; } }
  
  observer_ptr<bool> flagptr = nullptr;
  try { flagptr = _evStore->GetObject<bool>("FillFlag"); }
  catch(Retrieval::Exception &exc) { flagptr=NULL; }
  static bool flagptr_f=true; if(flagptr_f){ COUT(INFO)<<Form("%s::FillFlag::%s",routineName.c_str(),flagptr?"FOUND":"NOT-FOUND")<<ENDL; } flagptr_f=false;
  

  if( (*flagptr)==true)
    {

      if( bookMCtruthProcess )
	{
	  observer_ptr<MCtruthProcessStore> mcTruthProcessStore = nullptr;
	  try { mcTruthProcessStore = _evStore->GetObject<MCtruthProcessStore>("mcTruthProcessStore"); }
	  catch(Retrieval::Exception &exc) { mcTruthProcessStore=NULL; }
	  static bool mcTruthProcessStore_f=true; if(mcTruthProcessStore_f){ COUT(INFO)<<Form("%s::mcTruthProcessStore::%s",routineName.c_str(),mcTruthProcessStore?"FOUND":"NOT-FOUND")<<ENDL; } mcTruthProcessStore_f=false;

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
	    mcTracklengthcalox0 = mcTruthProcessStore->mcTracklengthcalox0;
	    mcTracklengthlysox0 = mcTruthProcessStore->mcTracklengthlysox0;
	    mcTracklenghtlysoafii = mcTruthProcessStore->mcTracklenghtlysoafii;
	    mcTracklenghtexactlysocm = mcTruthProcessStore->mcTracklenghtexactlysocm;
	    mcTrackcaloentry[0] = mcTruthProcessStore->mcTrackcaloentry[0];
	    mcTrackcaloentry[1] = mcTruthProcessStore->mcTrackcaloentry[1];
	    mcTrackcaloentry[2] = mcTruthProcessStore->mcTrackcaloentry[2];
	    mcTrackcaloexit[0] = mcTruthProcessStore->mcTrackcaloexit[0];
	    mcTrackcaloexit[1] = mcTruthProcessStore->mcTrackcaloexit[1];
	    mcTrackcaloexit[2] = mcTruthProcessStore->mcTrackcaloexit[2];
	    mcTrackcaloentryplane = mcTruthProcessStore->mcTrackcaloentryplane;
	    mcTrackcaloexitplane = mcTruthProcessStore->mcTrackcaloexitplane;
	    mcFirsthadint[0] = mcTruthProcessStore->mcFirsthadint[0];
	    mcFirsthadint[1] = mcTruthProcessStore->mcFirsthadint[1];
	    mcFirsthadint[2] = mcTruthProcessStore->mcFirsthadint[2];
	    mcHashadint = mcTruthProcessStore->mcHashadint;;
	    //
	    mcTruthProcessStore->Reset();
	  }
	}

      if( bookCaloGeomFidVolume )
	{
	  observer_ptr<CaloGeomFidVolumeStore> caloGeomFidVolumeStore = nullptr;
	  try { caloGeomFidVolumeStore = _evStore->GetObject<CaloGeomFidVolumeStore>("caloGeomFidVolumeStore"); }
	  catch(Retrieval::Exception &exc) { caloGeomFidVolumeStore=NULL; }
	  static bool caloGeomFidVolumeStore_f=true; if(caloGeomFidVolumeStore_f){ COUT(INFO)<<Form("%s::caloGeomFidVolumeStore::%s",routineName.c_str(),caloGeomFidVolumeStore?"FOUND":"NOT-FOUND")<<ENDL; } caloGeomFidVolumeStore_f=false;

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

	observer_ptr<Herd::CaloGlobStore> caloGlobStore = nullptr;
	try { caloGlobStore = _evStore->GetObject<Herd::CaloGlobStore>("caloGlobStore"); }
	catch(Retrieval::Exception &exc) { caloGlobStore=NULL; }
	static bool caloGlobStore_f=true; if(caloGlobStore_f){ COUT(INFO)<<Form("%s::caloGlobStore::%s",routineName.c_str(),caloGlobStore?"FOUND":"NOT-FOUND")<<ENDL; } caloGlobStore_f=false;

	if(caloGlobStore)
	  {
	    calonhits = caloGlobStore->calonhits;
	    calototedep = caloGlobStore->calototedep;
	    calonclusters = caloGlobStore->calonclusters;
	    caloicluster = caloGlobStore->caloicluster;
	    caloclusteredep = caloGlobStore->caloclusteredep;
	    caloclusteredepall = caloGlobStore->caloclusteredepall;
	    caloclusterhits = caloGlobStore->caloclusterhits;
	    caloclusterhitsall = caloGlobStore->caloclusterhitsall;
	    calonmiphitsontrack = caloGlobStore->calonmiphitsontrack;
	    calomiptrack = caloGlobStore->calomiptrack;
      
	    for(int ihit=0; ihit<(int)caloGlobStore->calohitsX.size(); ihit++){
	      calohitsID.push_back(caloGlobStore->calohitsID.at(ihit));
	      calohitsX.push_back(caloGlobStore->calohitsX.at(ihit));
	      calohitsY.push_back(caloGlobStore->calohitsY.at(ihit));
	      calohitsZ.push_back(caloGlobStore->calohitsZ.at(ihit));
	      calohitsE.push_back(caloGlobStore->calohitsE.at(ihit));
	      calohitsPL.push_back(caloGlobStore->calohitsPL.at(ihit));
	      calopcahits0.push_back(caloGlobStore->calopcahits0.at(ihit));
	      calopcahits1.push_back(caloGlobStore->calopcahits1.at(ihit));
	      calopcahits2.push_back(caloGlobStore->calopcahits2.at(ihit));

	    }
	    //
	    caloGlobStore->Reset();
	  }
      }

      if(bookCaloDig){
	auto caloDigStore = _evStore->GetObject<CaloDigStore>("caloDigStore");
	if(caloDigStore)
	  {

	    calototedepSPDE = caloDigStore->calototedepSPDE;
	    calototedepLPDE = caloDigStore->calototedepLPDE;
	    calototedepPDE = caloDigStore->calototedepPDE;
	    caloclusteredepSPDE = caloDigStore->caloclusteredepSPDE;
	    caloclusteredepLPDE = caloDigStore->caloclusteredepLPDE;
	    caloclusteredepPDE = caloDigStore->caloclusteredepPDE;

	    for(int ihit=0; ihit<(int)caloDigStore->calopdhitsSPDE.size(); ihit++){
	      calopdhitsSPDE.push_back(caloDigStore->calopdhitsSPDE.at(ihit));
	      calopdhitsSPDID.push_back(caloDigStore->calopdhitsSPDID.at(ihit));}
	    for(int ihit=0; ihit<(int)caloDigStore->calopdhitsLPDE.size(); ihit++){
	      calopdhitsLPDE.push_back(caloDigStore->calopdhitsLPDE.at(ihit));
	      calopdhitsLPDID.push_back(caloDigStore->calopdhitsLPDID.at(ihit));}
	    for(int ihit=0; ihit<(int)caloDigStore->calopdhitsPDE.size(); ihit++){
	      calopdhitsPDE.push_back(caloDigStore->calopdhitsPDE.at(ihit));
	      calopdhitsPDID.push_back(caloDigStore->calopdhitsPDID.at(ihit));}
	
	    //
	    caloDigStore->Reset();
	  }
      }
  
      if(bookCaloAxis){
    
	observer_ptr<Herd::CaloAxisProcessStore> caloAxisStore = nullptr;
	try { caloAxisStore = _evStore->GetObject<Herd::CaloAxisProcessStore>("caloAxisProcessStore"); }
	catch(Retrieval::Exception &exc) { caloAxisStore=NULL; }
	static bool caloAxisStore_f=true; if(caloAxisStore_f){ COUT(INFO)<<Form("%s::caloAxisProcessStore::%s",routineName.c_str(),caloAxisStore?"FOUND":"NOT-FOUND")<<ENDL; } caloAxisStore_f=false;

	if(caloAxisStore)
	  {
	    caloaxishits = caloAxisStore->caloaxishits;
	    caloaxispathlengthhit = caloAxisStore->caloaxispathlengthhit;

	    for(int i=0; i<3; i++){
	      caloaxiscog[i] = caloAxisStore->caloaxiscog[i];
	      caloaxissigma[i] = caloAxisStore->caloaxissigma[i];
	      caloaxisskew[i] = caloAxisStore->caloaxisskew[i];
	      caloaxiskurt[i] = caloAxisStore->caloaxiskurt[i];
	      caloaxisdir[i] = caloAxisStore->caloaxisdir[i];
	      caloaxiseigval[i] = caloAxisStore->caloaxiseigval[i];
	      caloaxisentryhit[i] = caloAxisStore->caloaxisentryhit[i];
	      caloaxisexithit[i] = caloAxisStore->caloaxisexithit[i];

	      for(int j=0; j<3; j++)
		{
		  caloaxiseigvec[i][j] = caloAxisStore->caloaxiseigvec[i][j];
		}
	    }
	    //
	    caloAxisStore->Reset();
	  }
      }

      _outputTree->Fill();
    }  
  
  calohitsID.clear();
  calohitsX.clear();
  calohitsY.clear();
  calohitsZ.clear();
  calohitsE.clear();
  calohitsPL.clear();
  calopcahits0.clear();
  calopcahits1.clear();
  calopcahits2.clear();
  

  return true;
}

//***************************

TreePersistenceHelper::TreePersistenceHelper(const std::string &name) :
  Algorithm{name}
  {
    DefineParameter("FillFlag", filltreethisevent);
  }

  bool TreePersistenceHelper::Initialize() {
  const std::string routineName("TreePersistenceHelper::Initialize");
  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL;  return false; }
  return true;
}

  bool TreePersistenceHelper::Process() {
  const std::string routineName("TreePersistenceHelper::Process");

  observer_ptr<bool> flagptr = nullptr;
  try { flagptr = _evStore->GetObject<bool>("FillFlag"); }
  catch(Retrieval::Exception &exc) { flagptr=NULL; }
  static bool flagptr_f=true; if(flagptr_f){ COUT(INFO)<<Form("%s::FillFlag::%s",routineName.c_str(),flagptr?"FOUND":"NOT-FOUND")<<ENDL; } flagptr_f=false;

  if(filltreethisevent) { *flagptr=true; }

  return true;
}

  bool TreePersistenceHelper::Finalize() {
  const std::string routineName("TreePersistenceHelper::Finalize");
  return true;
}
