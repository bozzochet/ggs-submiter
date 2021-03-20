#ifndef CALOENERGYDEPOSITHISTO
#define CALOENERGYDEPOSITHISTO

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloEnergyDepositHisto : public Algorithm {
public:

  CaloEnergyDepositHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);

  // Created global objects
  std::shared_ptr<TH2F> hcaloedep;
  std::shared_ptr<TH3F> hcaloedepfrac;
  std::shared_ptr<TH3F> hcaloedepleak;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;
};

#endif /* CALOENERGYDEPOSITHISTO */
