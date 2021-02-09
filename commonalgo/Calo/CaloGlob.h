#ifndef CALOGLOB_H_
#define CALOGLOB_H_

#include "algorithm/Algorithm.h"
#include "dataobjects/Line.h"
#include "dataobjects/Hit.h"
#include "dataobjects/Plane.h"

// HerdSoftware headers
#include "TH1F.h"
#include "TH2F.h"

using namespace EA;

namespace Herd {

class CaloGlobStore;

class CaloGlob : public Algorithm {
public:
  CaloGlob(const std::string &name);

  bool Initialize();

  bool Process();

  bool Finalize();

private:
  bool filterenable;
  bool calohitscutmc;
  bool fillpathl;
  float pcaedepth;

  double GetHitPathLenght(const Line* track, const Hit* hit);
  double PointLineDistance(const Point p, const Line l);
  int MipOnTrack( double pathl, double edep );

  std::shared_ptr<TH1F> hcalohitsedep;
  std::shared_ptr<TH1F> hcalohitspathl;
  std::shared_ptr<TH1F> hcalohitsedeppathl;
  std::shared_ptr<TH1F> hcalohitontrackedep;
  std::shared_ptr<TH1F> hcalohitontrackedeplin;

  std::shared_ptr<TH2F> hcalopathldist;
  //Store pointer
  std::shared_ptr<CaloGlobStore> _processstore;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store

};

class CaloGlobStore : public Algorithm {
public:
  
  CaloGlobStore(const std::string &name);
  bool Initialize();

  bool Process();

  bool Finalize();

  bool Reset();

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
  
  std::vector<int> calohitsID;
  std::vector<float> calohitsX;
  std::vector<float> calohitsY;
  std::vector<float> calohitsZ;
  std::vector<float> calohitsE;
  std::vector<float> calohitsPL;
  std::vector<float> calopcahits0;
  std::vector<float> calopcahits1;
  std::vector<float> calopcahits2;

  
private:
  // Algorithm parameters
  // Created global objects

  // Utility variables
};

}

#endif /* CALOGLOB_H_ */

