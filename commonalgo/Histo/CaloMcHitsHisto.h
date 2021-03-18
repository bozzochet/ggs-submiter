#ifndef CALOMCHITSHISTO_H_
#define CALOMCHITSHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloMcHitsHisto : public Algorithm {
public:

  CaloMcHitsHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);

  // Created global objects
  std::shared_ptr<TH2F> hcalohitsemc;
  std::shared_ptr<TH2F> hcalohitsedep;
  std::shared_ptr<TH2F> hcalohitsemcall;
  std::shared_ptr<TH2F> hcalohitsedepall;

  float _edepth;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;
};

#endif /* CALOMCHITSHISTO_H_ */
