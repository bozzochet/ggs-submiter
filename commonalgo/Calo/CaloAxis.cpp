// Example headers
#include "CaloAxis.h"
#include "dataobjects/CaloGeoParams.h"

// Root headers
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"

// C/C++ standard headers
#include <numeric>
#include <cmath>

namespace Herd{
RegisterAlgorithm(CaloAxis);
RegisterAlgorithm(CaloAxisStore);


CaloAxis::CaloAxis(const std::string &name) :
  Algorithm{name},
  filterenable{true}
   {
    DefineParameter("filterenable",  filterenable); 
  }

bool CaloAxis::Initialize() {
  const std::string routineName("CaloAxis::Initialize");

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore");      if (!_evStore)   { COUT(ERROR) << "Event data store not found." << ENDL; return false; }
  _globStore = GetDataStoreManager()->GetGlobalDataStore("globStore"); if (!_globStore) { COUT(ERROR) << "Global data store not found." << ENDL; return false; }

  
  _processstore = std::make_shared<CaloAxisStore>("CaloAxisStore");
  
  // Setup the filter                                                                                                                                                                                                                       
  if (filterenable) SetFilterStatus(FilterStatus::ENABLED); else SetFilterStatus(FilterStatus::DISABLED);




  return true;
}

bool CaloAxis::Process() {
  const std::string routineName("CaloAxis::Process");

  //Add the ProcessStore object for this event to the event data store
  //_processstore->Reset();
  _evStore->AddObject("CaloAxisStore",_processstore);

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);

  //Set vector sizes
  ShowerEigenvalues.clear();
  ShowerEigenvectors.clear();

  auto calohits = _evStore->GetObject<CaloHits>("caloHitsMC");
  BuildAxis( *calohits );
  //for(int i=0; i<3; i++) printf("[%d] %f {%f %f %f}\n",i, ShowerEigenvalues[i], ShowerEigenvectors.at(i)[RefFrame::Coo::X],ShowerEigenvectors.at(i)[RefFrame::Coo::Y],ShowerEigenvectors.at(i)[RefFrame::Coo::Z]);

  _processstore->caloaxiscog[0] = (float)ShowerCOG[RefFrame::Coo::X];
  _processstore->caloaxiscog[1] = (float)ShowerCOG[RefFrame::Coo::Y];
  _processstore->caloaxiscog[2] = (float)ShowerCOG[RefFrame::Coo::Z];
  _processstore->caloaxisdir[0] = (float)ShowerDir[RefFrame::Coo::X];
  _processstore->caloaxisdir[1] = (float)ShowerDir[RefFrame::Coo::Y];
  _processstore->caloaxisdir[2] = (float)ShowerDir[RefFrame::Coo::Z];
  for(int i=0; i<3; i++)
    {
     _processstore->caloaxiseigval[i]    = (float)ShowerEigenvalues[i];
     _processstore->caloaxiseigvec[i][0] = (float)ShowerEigenvectors[i][RefFrame::Coo::X];
     _processstore->caloaxiseigvec[i][1] = (float)ShowerEigenvectors[i][RefFrame::Coo::Y];
     _processstore->caloaxiseigvec[i][2] = (float)ShowerEigenvectors[i][RefFrame::Coo::Z];
    }
  return true;
}

bool CaloAxis::DummyCaloCluster(){

  return true;     
}

bool CaloAxis::BuildAxis(CaloHits calohits){

  auto caloGeoParams = _globStore->GetObject<CaloGeoParams>("caloGeoParams");

  double threshold=0;
  std::vector<std::vector<double>> x;


  //Loop on hits, select hits with edep>threshold and create a vector containing coordinates [0,1,2] and weigths [3]
  //x[0]: Coo::X
  //x[1]: Coo::Y
  //x[2]: Coo::Z
  //x[3]: Hit::EDep
  int n=0;
  for (auto &hit : calohits) {
    if(hit.EDep() > threshold){
      std::vector<double> x_i;
      Point pos = caloGeoParams->Position(hit.VolumeID());
      x_i.push_back(pos[RefFrame::Coo::X]);
      x_i.push_back(pos[RefFrame::Coo::Y]);
      x_i.push_back(pos[RefFrame::Coo::Z]);
      x_i.push_back(hit.EDep());
      x.push_back(x_i);
      n++;
    }
  }  
  //Here we assume that calo hits are uncorrelated, so the covariance matrix is diagonal

  //Calculate the sum of weigths
  double sumw=0; for(int i=0; i<n; i++) { sumw += x[i][3]; }

  //Calculate the centroid of the energy deposit
  std::vector<double> cog; for(int j=0; j<3; j++){ cog.push_back(0); }
  for(int i=0; i<n; i++) { for(int j=0; j<3; j++) { cog[j] += (1/sumw) * x[i][3] * x[i][j]; } }// printf("%f %f\n", x[i][j], cog[j]);} 
    
  //Reposition the hit point to the energy centroid
  for(int j=0; j<n; j++) { for(int i=0; i<3; i++) x[j][i] -= cog[i]; }

  TMatrixD X(n,3);    //Data matrix (with respect to centroid)
  for(int i=0; i<n; i++){ for(int j=0; j<3; j++){ X[i][j] = x[i][j]; }}


 /*for(int i=0; i<n; i=i+10){
    printf("-- [%d][%d]  %f\n",i,0,X[i][0]);
    printf("-- [%d][%d]  %f\n",i,1,X[i][1]);
    printf("-- [%d][%d]  %f\n",i,2,X[i][2]);
    }
  printf("\n");
*/
  TMatrixD Xt(3,n);   //Data matrix transposed (with respect to centroid)
  for(int j=0; j<3; j++){ for(int i=0; i<n; i++){ Xt[j][i] = x[i][j]; }}

/*printf("%f %f\n", Xt[0][1],  X[1][0]);
printf("%f %f\n", Xt[1][20], X[20][1]);
printf("%f %f\n", Xt[2][40], X[40][2]);
*/
  TMatrixD XtX(Xt,TMatrixD::kMult,X);  //Xt * X
/* for(int i=0; i<3; i++){
    printf("-- %f %f %f\n",XtX[i][0],XtX[i][1],XtX[i][2]);
    }

  printf("\n");
*/
  //Calculate the covariance matrix as   C_i,j = (1/(sumw-1)) * sum_k [ (w_k) Xt_i,k * X_k,j ]
  //https://en.wikipedia.org/wiki/Sample_mean_and_covariance#Weighted_samples
  TMatrixDSym C(3);
  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      C[i][j] = 0; for(int k=0; k<n; k++) C[i][j] += (1/(sumw)) * x[k][3] * XtX[i][j];
    }
  }
/*  for(int i=0; i<3; i++){
    printf("%f\t%f\t%f\n",C[i][0],C[i][1],C[i][2]);
    }
  printf("\n");
  */

  //Get eigenvalues of the covariance matrix
  TMatrixDSymEigen E(C);
  TVectorD eigvaluesvec = E.GetEigenValues();
  TMatrixD eigvecmatrix = E.GetEigenVectors();
  //for(int i=0; i<3; i++) printf("[%d] %f\t {%f,%f,%f}\n", i, eigvaluesvec[i], eigvecmatrix[i][0], eigvecmatrix[i][1], eigvecmatrix[i][2]);
  //printf("\n");
  //Sort Eigenvector by decreasing eigenvalues
  TVectorD eigvec0( TVectorD(TMatrixTColumn_const<double>(eigvecmatrix, 0) ) ); //could be done more elegantly
  TVectorD eigvec1( TVectorD(TMatrixTColumn_const<double>(eigvecmatrix, 1) ) );
  TVectorD eigvec2( TVectorD(TMatrixTColumn_const<double>(eigvecmatrix, 2) ) );
  double eigval[3];   for(int i=0; i<3; i++){ eigval[i] = eigvaluesvec[i]; }
  std::vector<std::pair<double,TVectorD>> eigvec = { {eigval[0],eigvec0}, {eigval[1],eigvec1}, {eigval[2],eigvec2} };
  std::sort(eigvec.begin(),eigvec.end(), []( const std::pair<double,TVectorD>x, std::pair<double,TVectorD>y) {return x.first>y.first;});
   
  //Store values to containers
  ShowerCOG[RefFrame::Coo::X] = cog[0];
  ShowerCOG[RefFrame::Coo::Y] = cog[1];
  ShowerCOG[RefFrame::Coo::Z] = cog[2];

  double mag=0; for(int i=0; i<3; i++) mag+= pow(eigvec.at(0).second[i],2); mag=sqrt(mag); 
  ShowerDir[RefFrame::Coo::X] = eigvec.at(0).second[0]/mag;
  ShowerDir[RefFrame::Coo::Y] = eigvec.at(0).second[1]/mag;
  ShowerDir[RefFrame::Coo::Z] = eigvec.at(0).second[2]/mag;

  for(int i=0; i<3; i++) ShowerEigenvalues.push_back( eigvec.at(i).first );
  for(int i=0; i<3; i++) { 
    Vec3D v(eigvec.at(i).second[0],eigvec.at(i).second[1],eigvec.at(i).second[2]);
    ShowerEigenvectors.push_back(v); }
  
  x.clear();
  return true;
}


bool CaloAxis::Finalize() {
  const std::string routineName("CaloAxis::Finalize");
  return true;
}

//***************************

CaloAxisStore::CaloAxisStore(const std::string &name) :
  Algorithm{name}
   {
  }

  bool CaloAxisStore::Initialize() {
  const std::string routineName("CaloAxisStore::Initialize");
  Reset();
  return true;
}

  bool CaloAxisStore::Process() {
  const std::string routineName("CaloAxisStore::Process");
  return true;
}
  bool CaloAxisStore::Finalize() {
  const std::string routineName("CaloAxisStore::Finalize");
  return true;
}
bool CaloAxisStore::Reset() {
  const std::string routineName("CaloAxisStore::Finalize");

for(int i=0; i<3; i++){
  caloaxiscog[i] = -999.;
  caloaxisdir[i] = -999.;
  caloaxiseigval[i] = -999.;

  for(int j=0; j<3; j++)
    {
    caloaxiseigvec[i][j] = -999.;
    }
  }

  return true;
}

} //namespace Herd