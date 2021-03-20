/*
 * TreePersistence.cpp
 *
 *  Created on: 3 Feb 2020
 *      Author: Nicola Mori
 */

#ifndef TREEPERSISTENCE_H_
#define TREEPERSISTENCE_H_

// HerdSoftware headers
#include "dataobjects/Momentum.h"
#include "dataobjects/Point.h"

// EventAnalysis headers
#include "persistence/PersistenceService.h"
#include "algorithm/Algorithm.h"

#include <vector>

using namespace EA;

// HERD data object classes
namespace Herd {
class MCTruth;
}

// Root classes
class TTree;
class TFile;


class TreePersistence : public PersistenceService {
public:
 
  TreePersistence(const std::string &name, const std::string &output);

  bool Connect();
  bool Disconnect();
  bool BookEventObject(const std::string &objName, const std::string &objStore);
  bool BeginOfEvent();
  bool EndOfEvent();





private:
  // Pointers to Root objects
  std::unique_ptr<TFile> _outputFile;
  observer_ptr<TTree> _outputTree;

  // Output data
  bool bookMCtruthProcess;
  int mcNdiscarded;
  float mcDir[3];
  float mcCoo[3];
  float mcMom;
  float mcPhi;
  float mcCtheta;
  int mcStkintersections;
  float mcTracklengthcalox0;
  float mcTracklengthlysox0;
  float mcTracklenghtexactlysocm;
  float mcTracklenghtlysoafii;
  float mcTrackcaloentry[3];
  float mcTrackcaloexit[3];
  int mcTrackcaloentryplane;
  int mcTrackcaloexitplane;
  bool mcHashadint;
  float mcFirsthadint[3];

  bool bookCaloGeomFidVolume;
  float calofidvolalpha;
  bool calofidvolpass;
  short calofidvolxpos;
  short calofidvolxneg;
  short calofidvolypos;
  short calofidvolyneg;
  short calofidvolzpos;
  short calofidvolzneg;
  short calofidvolxnegyneg;
  short calofidvolxposyneg;
  short calofidvolxnegypos;
  short calofidvolxposypos;
  float calofidvolxposEntry[2][3];
  float calofidvolxnegEntry[2][3];
  float calofidvolyposEntry[2][3];
  float calofidvolynegEntry[2][3];
  float calofidvolzposEntry[2][3];
  float calofidvolznegEntry[2][3];
  float calofidvolxnegynegEntry[2][3];
  float calofidvolxposynegEntry[2][3];
  float calofidvolxnegyposEntry[2][3];
  float calofidvolxposyposEntry[2][3];
  
  bool bookCaloGlob;
  int calonhits;
  float calototedep;
  int calonclusters;
  int caloicluster;
  float caloclusteredep;
  float caloclusteredepall;
  int caloclusterhits;
  int caloclusterhitsall;
  short calonmiphitsontrack;
  float calomiptrack;

  bool bookCaloDig;
  float calototedepSPDE;
  float calototedepLPDE;
  float calototedepPDE;
  float caloclusteredepSPDE;
  float caloclusteredepLPDE;
  float caloclusteredepPDE;
  std::vector<float> calopdhitsSPDE;
  std::vector<int> calopdhitsSPDID;
  std::vector<float> calopdhitsLPDE;
  std::vector<int> calopdhitsLPDID;
  std::vector<float> calopdhitsPDE;
  std::vector<int> calopdhitsPDID;


  bool bookCaloAxis;
  unsigned short caloaxishits;
  float caloaxiscog[3];
  float caloaxissigma[3];
  float caloaxisskew[3];
  float caloaxiskurt[3];
  float caloaxisentryhit[3];
  float caloaxisexithit[3];
  float caloaxispathlengthhit;
  float caloaxisdir[3];
  float caloaxiseigval[3];
  float caloaxiseigvec[3][3];


/*
  std::vector< std::array<float, 4> > calohits;
  std::vector< std::array<float, 4> > calopcahits;
*/
std::vector<int> calohitsID;
std::vector<float> calohitsX;
std::vector<float> calohitsY;
std::vector<float> calohitsZ;
std::vector<float> calohitsE;
std::vector<float> calohitsPL;
std::vector<float> calopcahits0;
std::vector<float> calopcahits1;
std::vector<float> calopcahits2;


  // Flag for optional output objects
  //bool _isPSDBooked;
  //std::string _psdStoreName; // Name of the data store where the PSD data object is located

  // Utility variables
  StorePtr _evStore;
  //StorePtr _psdEvStore;

  // Configuration parameters
  //float _psdHitThreshold;
};

class TreePersistenceHelper : public Algorithm {
public:
  TreePersistenceHelper(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  StorePtr _evStore;
  bool filltreethisevent;
  //std::shared_ptr<bool> fillflag;
};

#endif /* TREEPERSISTENCE_H_ */
