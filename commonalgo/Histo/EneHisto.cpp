// Example headers
#include "EneHisto.h"

// Root headers
#include "TH1F.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>

RegisterAlgorithm(EneHisto);

//EneHisto::EneHisto(const std::string &name) : Algorithm{name}, axispar{100., 0., 100.}, logaxis{false}, var{"MCmom"}, title("title") {
EneHisto::EneHisto(const std::string &name) : Algorithm{name}, naxisbins{100}, axismin{0.}, axismax{0.}, logaxis{false}, var{"MCmom"}, title("title") {
  //DefineParameter("axispar", axispar);
  DefineParameter("naxisbins", naxisbins);
  DefineParameter("axismin", axismin);
  DefineParameter("axismax", axismax);
  DefineParameter("logaxis", logaxis);
  DefineParameter("title",   title);
  DefineParameter("var",     var);     
}

bool EneHisto::Initialize() {
  const std::string routineName("EneHisto::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }

  // Check the user setting for the axis
  //if (axispar.size() != 3) { COUT(ERROR) << "The axis must be specified by exactly 3 parameters" << ENDL; return false; }
  //if ((float)((int)(axispar[0])) != axispar[0]) { COUT(ERROR) << "The number of bins is not an integer." << ENDL;  }
  //if (axispar[0] < 0.) { COUT(ERROR) << "The number of bins is a negative value." << ENDL; }
  //if (axispar[1] >= axispar[2]) { COUT(ERROR) << "The lower axis limit is greater or equal to the upper limit." << ENDL; }

  if ((float)((int)(naxisbins)) != naxisbins) { COUT(ERROR) << "The number of bins is not an integer." << ENDL;  }
  if (naxisbins < 0) { COUT(ERROR) << "The number of bins is a negative value." << ENDL; }
  if (axismin >= axismax) { COUT(ERROR) << "The lower axis limit is greater or equal to the upper limit." << ENDL; }


  if( logaxis ){ GenerateLogBinning(); }
  else         { GenerateBinning();    } 

  // Create the histogram
  //histo = std::make_shared<TH1F>(GetName().c_str(), title.c_str(), axispar[0], axis);
  histo = std::make_shared<TH1F>(GetName().c_str(), title.c_str(), naxisbins, axis);
  if( strcmp(var.c_str(),"MCmom")==0 ) histo->GetXaxis()->SetTitle("MC Momentum (GV)");

  return true;
}

bool EneHisto::Process() {
  const std::string routineName("EneHisto::Process");

  if( strcmp(var.c_str(),"MCmom")==0 ){
    auto mctruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
    if (!mctruth) { COUT(DEBUG) << "MCTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
    float mcmom = std::sqrt(mctruth->primaries.at(0).initialMomentum * mctruth->primaries.at(0).initialMomentum);
    histo->Fill(mcmom);
  }

  return true;
}

bool EneHisto::Finalize() {
  const std::string routineName("CaloEDepCut::Finalize");
  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  globStore->AddObject(GetName(), histo);
  return true;
}

void EneHisto::GenerateLogBinning(){
  //axis = new Double_t[(int)axispar[0]+1];
  //Double_t log_interval = ( log10(axispar[2]) - log10(axispar[1]) ) / (int)axispar[0];
  //for(int i=0; i<=(int)axispar[0]; i++) axis[i] = pow( 10, log10(axispar[1]) + i * log_interval );
  axis = new Double_t[naxisbins+1];
  Double_t log_interval = ( log10(axismax) - log10(axismin) ) / naxisbins;
  for(int i=0; i<=naxisbins; i++) axis[i] = pow( 10, log10(axismin) + i * log_interval );

}
void EneHisto::GenerateBinning(){
 // axis = new Double_t[(int)axispar[0]+1];
 // Double_t interval = ( axispar[2] - axispar[1] ) / (int)axispar[0];
 // for(int i=0; i<=(int)axispar[0]; i++) axis[i] = axispar[1] + i*interval;
  axis = new Double_t[(int)naxisbins+1];
  Double_t interval = ( axismax - axismin ) / naxisbins;
  for(int i=0; i<=(int)naxisbins; i++) axis[i] = axismin + i*interval;
}
