#ifndef CALODIGHISTO_H_
#define CALODIGHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloDigHisto : public Algorithm {
public:

  CaloDigHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);
  

  // Created global objects
  std::shared_ptr<TH2F> hpdemchit;
  std::shared_ptr<TH2F> hlpdemchit;
  std::shared_ptr<TH2F> hspdemchit;
  std::shared_ptr<TH2F> hpdelpdehit;
  std::shared_ptr<TH2F> hpdespdehit;
  std::shared_ptr<TH2F> htotmcpde;
  std::shared_ptr<TH2F> htotmclpde;
  std::shared_ptr<TH2F> htotmcspde;
  std::shared_ptr<TH2F> hreldiffpde;
  std::shared_ptr<TH2F> hmchitpdehit;
  std::shared_ptr<TH2F> htotmc;
  std::shared_ptr<TH2F> hreldiffpdeaboveth;
  std::shared_ptr<TH2F> htotmcaboveth;
  std::shared_ptr<TH2F> hmchitpdehitreldiff;
  std::shared_ptr<TH2F> hmchitpdehitreldiffall;


  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;
};

#endif /* CALODIGHISTO_H_ */
