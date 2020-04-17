#ifndef CALOAXIS_H_
#define CALOAXIS_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers                                                                                                                                                         
#include "dataobjects/CaloClusters.h"
#include "dataobjects/CaloHits.h"

#include "dataobjects/CaloGeoParams.h"

using namespace EA;

namespace Herd{

class CaloAxisStore;

class CaloAxis : public Algorithm {
public:
  CaloAxis(const std::string &name);

  bool Initialize();

  bool Process();

  bool Finalize();

  Point ShowerCOG;
  Point ShowerDir;
  std::vector<double> ShowerEigenvalues;
  std::vector<Vec3D> ShowerEigenvectors;
  

private:
  bool filterenable;
  bool DummyCaloCluster();
  bool BuildAxis(CaloHits);


  //Store pointer
  std::shared_ptr<CaloAxisStore> _processstore;
  CaloHits calohits;


  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store
  observer_ptr<GlobalDataStore> _globStore; // Pointer to the event data store
};

class CaloAxisStore : public Algorithm {
public:
  
  CaloAxisStore(const std::string &name);
  bool Initialize();

  bool Process();

  bool Finalize();

  bool Reset();

 float caloaxiscog[3];
  float caloaxisdir[3];
  float caloaxiseigval[3];
  float caloaxiseigvec[3][3];
private:
  // Algorithm parameters
  // Created global objects
 

  // Utility variables
};

#endif /* CALOAXIS_H_ */
}