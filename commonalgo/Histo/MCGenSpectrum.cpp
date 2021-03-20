// Example headers
#include "MCGenSpectrum.h"

// Root headers
#include "TH1F.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>

RegisterAlgorithm(MCGenSpectrum);

//MCGenSpectrum::MCGenSpectrum(const std::string &name) : Algorithm{name}, axispar{100., 1., 100000.}, logaxis{true}, title("title"), momrange{-1.,-1.}, index{-1} {
  MCGenSpectrum::MCGenSpectrum(const std::string &name) : Algorithm{name}, naxisbins{100}, axismin{1}, axismax{100000}, logaxis{true}, title("title"), minmom{-1.}, maxmom{-1.}, index{-1} {
  //
  DefineParameter("naxisbins", naxisbins);
  DefineParameter("axismin", axismin);
  DefineParameter("axismax", axismax);
  //DefineParameter("axispar", axispar);
  DefineParameter("logaxis", logaxis);
  DefineParameter("title",   title);
  //DefineParameter("momrange",  momrange);
  DefineParameter("minmom",  minmom);
  DefineParameter("maxmom",  maxmom);
  
  DefineParameter("index",   index);

}

bool MCGenSpectrum::Initialize() {
  const std::string routineName("MCGenSpectrum::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore"); if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }

  // Check the user setting for the axis
  //if (axispar.size() != 3) { COUT(ERROR) << "The axis must be specified by exactly 3 parameters" << ENDL; return false; }
  //if ((float)((int)(axispar[0])) != axispar[0]) { COUT(ERROR) << "The number of bins is not an integer." << ENDL;  }
  //if (axispar[0] < 0.) { COUT(ERROR) << "The number of bins is a negative value." << ENDL; }
  //if (axispar[1] >= axispar[2]) { COUT(ERROR) << "The lower axis limit is greater or equal to the upper limit." << ENDL; }
  if ((float)((int)(naxisbins)) != naxisbins) { COUT(ERROR) << "The number of bins is not an integer." << ENDL;  }
  if (naxisbins < 0) { COUT(ERROR) << "The number of bins is a negative value." << ENDL; }
  if (axismin >= axismax) { COUT(ERROR) << "The lower axis limit is greater or equal to the upper limit." << ENDL; }

  if( index != -1) {COUT(ERROR) << "Only index -1 supported" << ENDL; }

  if( logaxis ){ GenerateLogBinning(); }
  else         { GenerateBinning();    } 

  // Create the histogram
  //histo = std::make_shared<TH1F>("hmcgenspectrum", title.c_str(), axispar[0], axis);
  histo = std::make_shared<TH1F>("hmcgenspectrum", title.c_str(), naxisbins, axis);
  histo->GetXaxis()->SetTitle("MC Momentum (GV()");

  ngen=0;

  return true;
}

bool ParseFromFileName(){
  return true;
}

bool MCGenSpectrum::Process() {
  const std::string routineName("MCGenSpectrum::Process");

  auto mctruth = _evStore->GetObject<Herd::MCTruth>("mcTruth");
  if (!mctruth) { COUT(DEBUG) << "MCTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL; return false; }
  ngen += mctruth->nDiscarded+1;

  return true;
}

bool MCGenSpectrum::Finalize() {
  const std::string routineName("MCGenSpectrum::Finalize");

  //index==-1
  printf("ngen::%d\n",ngen);
  for(int ibin=1; ibin<histo->GetNbinsX(); ibin++)
  {
   //if( histo->GetBinLowEdge(ibin+1) < momrange[0])    //bin is outside range of generated values
    if( histo->GetBinLowEdge(ibin+1) < minmom)    //bin is outside range of generated values
       {  histo->SetBinContent(ibin,0); }
   // else if(histo->GetBinLowEdge(ibin) > momrange[1])  //bin is outside range of generated values
    else if(histo->GetBinLowEdge(ibin) > maxmom)  //bin is outside range of generated values
       {  histo->SetBinContent(ibin,0); }
      else                                        //bin is inside range of generated values
      {
     // double lowedge  = std::max(histo->GetBinLowEdge(ibin),   momrange[0]);
     // double highedge = std::min(histo->GetBinLowEdge(ibin+1), momrange[1]);
     // double w = (log10(highedge) - log10(lowedge)) / (log10(momrange[1]) - log10(momrange[0]));
     double lowedge  = std::max(histo->GetBinLowEdge(ibin),   minmom);
     double highedge = std::min(histo->GetBinLowEdge(ibin+1), maxmom);
     double w = (log10(highedge) - log10(lowedge)) / (log10(maxmom) - log10(minmom));
      histo->SetBinContent(ibin, ngen * w);
      }
  }

  auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }
  globStore->AddObject(histo->GetName(), histo);
  
  return true;
}

void MCGenSpectrum::GenerateLogBinning(){
  //axis = new Double_t[(int)axispar[0]+1];
  //Double_t log_interval = ( log10(axispar[2]) - log10(axispar[1]) ) / (int)axispar[0];
  //for(int i=0; i<=(int)axispar[0]; i++) axis[i] = pow( 10, log10(axispar[1]) + i * log_interval );
   axis = new Double_t[naxisbins+1];
  Double_t log_interval = ( log10(axismax) - log10(axismin) ) / naxisbins;
  for(int i=0; i<=naxisbins; i++) axis[i] = pow( 10, log10(axismin) + i * log_interval );
}
void MCGenSpectrum::GenerateBinning(){
 // axis = new Double_t[(int)axispar[0]+1];
 // Double_t interval = ( axispar[2] - axispar[1] ) / (int)axispar[0];
 // for(int i=0; i<=(int)axispar[0]; i++) axis[i] = axispar[1] + i*interval;
  axis = new Double_t[(int)naxisbins+1];
  Double_t interval = ( axismax - axismin ) / naxisbins;
  for(int i=0; i<=(int)naxisbins; i++) axis[i] = axismin + i*interval;
}
