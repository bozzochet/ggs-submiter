#ifndef CALOMCHITSNUMBERSELECTION
#define CALOMCHITSNUMBERSELECTION

#include "algorithm/Algorithm.h"

using namespace EA;

class CaloMcHitsNumberSelection : public Algorithm {
public:

  CaloMcHitsNumberSelection(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:

  // Algorithm parameters
  bool _filterenable;
  int _minhits;
  float _edepth;
  
  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;

};


#endif /* CALOMCHITSNUMBERSELECTION */
