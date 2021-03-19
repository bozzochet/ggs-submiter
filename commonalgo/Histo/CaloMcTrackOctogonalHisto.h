#ifndef CALOMCTRACKOCTOGONALHISTO_H_
#define CALOMCTRACKOCTOGONALHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloMcTrackOctogonalHisto : public Algorithm {
public:

  CaloMcTrackOctogonalHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);

  // Created global objects
  std::shared_ptr<TH3F> hcalomctrackentryxy;
  std::shared_ptr<TH3F> hcalomctrackentryyz;
  std::shared_ptr<TH3F> hcalomctrackentryxz;
  std::shared_ptr<TH3F> hcalomctrackexitxy;
  std::shared_ptr<TH3F> hcalomctrackexityz;
  std::shared_ptr<TH3F> hcalomctrackexitxz;

  std::shared_ptr<TH2F> hcalomctracklenx0;
  std::shared_ptr<TH2F> hcalomctracklencm;

  std::shared_ptr<TH3F> hcalomctrackaftintcm;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;
};

#endif /* CALOMCTRACKOCTOGONALHISTO_H_ */
