#ifndef MCGENSPECTRUM_H_
#define MCGENSPECTRUM_H_

#include "algorithm/Algorithm.h"
#include "analysis/AnalysisManager.h"

// HerdSoftware headers
#include "dataobjects/MCTruth.h"

using namespace EA;

class TH1F;

class MCGenSpectrum : public Algorithm {
public:
  MCGenSpectrum(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:

  std::vector<double> axispar;
  double *axis;
  bool logaxis;
  std::string title;
  
  void GenerateLogBinning();
  void GenerateBinning();
  

  int ngen;
  std::vector<double> momrange;
  float index;
  
  std::shared_ptr<TH1F> histo; // Objects to be pushed on global store must be held by a shared_ptr

  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store
};

#endif /* MCGENSPECTRUM */
