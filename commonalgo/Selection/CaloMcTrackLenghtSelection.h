#ifndef CALOMCTRACKLENGTHSELECTION
#define CALOMCTRACKLENGTHSELECTION

#include "algorithm/Algorithm.h"

using namespace EA;

class CaloMcTrackLengthSelection : public Algorithm {
public:

  CaloMcTrackLengthSelection(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();

private:

  

  // Algorithm parameters
  bool _filterenable;
  double _mincalotrackx0;
  bool _notfrombottom;

  // Utility variables
  void PrintCaloCubeMap();

  // Utility variables
  observer_ptr<EventDataStore> _evStore;
  observer_ptr<GlobalDataStore> _globStore;

};


#endif /* CALOMCTRACKLENGTHSELECTION */
