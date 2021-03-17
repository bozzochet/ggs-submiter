#ifndef CALOAXISHISTO_H_
#define CALOAXISHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers

using namespace EA;

class TH1F;
class TH2F;
class TH3F;

class CaloAxisHisto : public Algorithm {
public:

  CaloAxisHisto(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:
  
  double *GenerateLogBinning(int nbins, double min, double max);
  double *GenerateBinning(int nbins, double min, double max);
  
  // Created global objects
  std::shared_ptr<TH3F> hcaloaxistheta;
  std::shared_ptr<TH3F> hcaloaxisphi;
  std::shared_ptr<TH3F> hcaloaxisdir[3];
  std::shared_ptr<TH3F> hcaloaxisthetaface;
  std::shared_ptr<TH3F> hcaloaxisphiface;
  std::shared_ptr<TH3F> hcaloaxisdirface[3];
  std::shared_ptr<TH3F> hcaloaxisthetadiff;
  std::shared_ptr<TH3F> hcaloaxisphidiff;
  std::shared_ptr<TH3F> hcaloaxisdirdiff[3];
  std::shared_ptr<TH2F> hcaloaxisdiff;
  std::shared_ptr<TH3F> hcaloaxiscogxy;
  std::shared_ptr<TH3F> hcaloaxiscogxz;
  std::shared_ptr<TH3F> hcaloaxiscogyz;

  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;
};

#endif /* CALOAXISHISTO_H_ */
