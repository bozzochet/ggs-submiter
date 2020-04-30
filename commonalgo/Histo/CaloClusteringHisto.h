#ifndef CALOCLUSTERINGHISTO_H_
#define CALOCLUSTERINGHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloClusteringHisto : public Algorithm {
public:

  CaloClusteringHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);
  

  // Created global objects
  std::shared_ptr<TH2F> hncaloclusters;         // Objects to be pushed on global store must be held by a shared_ptr
  std::shared_ptr<TH3F> honecaloclusterhitsedep;
  std::shared_ptr<TH3F> honecaloclusterhitsleak;

  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store
};

#endif /* CALOCLUSTERINGHISTO_H_ */
