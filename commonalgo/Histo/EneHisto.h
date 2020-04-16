#ifndef ENEHISTO_H_
#define ENEHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers
#include "dataobjects/MCTruth.h"

using namespace EA;

class TH1F;

class EneHisto : public Algorithm {
public:
  EneHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:

  std::vector<float> axispar;
  double *axis;
  bool logaxis;
  std::string title;

  std::string var;
  
  void GenerateLogBinning();
  void GenerateBinning();
  

  // Created global objects
  std::shared_ptr<TH1F> histo; // Objects to be pushed on global store must be held by a shared_ptr

  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store
};

#endif /* ENEHISTOHISTO_H_ */
