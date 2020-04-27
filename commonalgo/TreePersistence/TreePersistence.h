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
  bool BeginningOfEvent();
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
  float mcTrackcaloentry[3];
  float mcTrackcaloexit[3];
  int mcTrackcaloentryplane;
  int mcTrackcaloexitplane;

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
  
  bool bookCaloAxis;
  float caloaxiscog[3];
  float caloaxisdir[3];
  float caloaxiseigval[3];
  float caloaxiseigvec[3][3];

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
