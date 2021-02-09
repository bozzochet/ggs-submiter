#ifndef CALODIG_H_
#define CALODIG_H_

#include "algorithm/Algorithm.h"
#include "dataobjects/Line.h"
#include "dataobjects/Hit.h"
#include "dataobjects/Plane.h"

// HerdSoftware headers
#include "TH1F.h"
#include "TH2F.h"

using namespace EA;

namespace Herd {

class CaloDigStore;

class CaloDig : public Algorithm {
public:
  CaloDig(const std::string &name);

  bool Initialize();

  bool Process();

  bool Finalize();

private:
  bool filterenable;
  std::shared_ptr<TH2F> hcalototedepSPDE;
  std::shared_ptr<TH2F> hcalototedepLPDE;
  std::shared_ptr<TH2F> hcalototedepPDE;
  std::shared_ptr<TH2F> hcaloclusteredepSPDE;
  std::shared_ptr<TH2F> hcaloclusteredepLPDE;
  std::shared_ptr<TH2F> hcaloclusteredepPDE;

  //Store pointer
  std::shared_ptr<CaloDigStore> _processstore;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store

};

class CaloDigStore : public Algorithm {
public:
  
  CaloDigStore(const std::string &name);
  bool Initialize();

  bool Process();

  bool Finalize();

  bool Reset();

  float calototedepSPDE;
  float calototedepLPDE;
  float calototedepPDE;
  float caloclusteredepSPDE;
  float caloclusteredepLPDE;
  float caloclusteredepPDE;
  std::vector<float> calopdhitsSPDE;
  std::vector<float> calopdhitsSPDID;
  std::vector<float> calopdhitsLPDE;
  std::vector<float> calopdhitsLPDID;
  std::vector<float> calopdhitsPDE;
  std::vector<float> calopdhitsPDID;

  
private:
  // Algorithm parameters
  // Created global objects

  // Utility variables
};

}

#endif /* CALODIG_H_ */

