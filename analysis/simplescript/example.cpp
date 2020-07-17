/*
 * analysis.C
 *
 *  Created on: 11 Feb 2019
 *      Author: Nicola Mori
 */

/* ****** Before loading this file you should load libHerdDataObjectsDict.so in the Root shell ***** */

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

#include <iostream>
#include <numeric>
#include <string>

#include "dataobjects/MCTruth.h"
#include "dataobjects/Point.h"

void AnalyzeMC(const std::string &inFileName, const std::string &outFileName) {

  TFile *digFile = TFile::Open(inFileName.c_str());
  if (!digFile) {
    std::cout << "Error: input file not found." << std::endl;
    return;
  }

  TTree *evTree = (TTree *)(digFile->Get("eventTree"));
  if (!evTree) {
    std::cout << "Error: event tree not found." << std::endl;
    return;
  }

  
  // Create a pointer for setting the address of the stkGeoParams branch
  // Herd::StkGeoParamsColl *stkGeoParamsColl = digFile->Get<Herd::StkGeoParamsColl>("stkGeoParamsCollWaferStrip");
  //Herd::StkGeoParamsColl *stkGeoParamsColl = (Herd::StkGeoParamsColl*) digFile->Get("stkGeoParamsCollWaferStrip");

  // Create a pointer for setting the address of the mcTruth branch
  Herd::MCTruth *mcTruth = nullptr;
  evTree->SetBranchAddress("mcTruth", &mcTruth);
  
  /*
// Create a pointer for setting the address of the caloHits branch
  Herd::CaloHits *caloHits = nullptr;
  evTree->SetBranchAddress("caloHitsMC", &caloHits);
  // Create a pointer for setting the address of the stkHits branch
  Herd::StkHitsColl *stkHitsColl = nullptr;
  evTree->SetBranchAddress("stkHitsCollMC", &stkHitsColl);
  // Create a pointer for setting the address of the scdHits branch
  Herd::ScdHitsColl *scdHitsColl = nullptr;
  evTree->SetBranchAddress("scdHitsCollMC", &scdHitsColl);
  */


  // Prepare output file and histograms
  TFile *outFile = TFile::Open(outFileName.c_str(), "RECREATE");

  for (int iEv = 0; iEv < evTree->GetEntries(); ++iEv) {
    evTree->GetEntry(iEv);


    /*
    // Compute CALO vars
    float totEDep = std::accumulate(caloHits->begin(), caloHits->end(), 0.,
                                    [](float sum, const Herd::Hit &hit) { return sum + hit.EDep(); });

    // Compute STK vars
    int nStkHits = 0;
    for (auto &layerHits : (*stkHitsColl)[Herd::RefFrame::Side::TOP]) {
      nStkHits += ((int)(layerHits.size()));
    }

    // Fill CALO histogram
    histoCaloAll.Fill(totEDep);
    if (nStkHits < 50) {
      histoCaloSel.Fill(totEDep);
    }

    float _distance = 0.1;
    float primStkEDep = 0;
    const auto &primaryPart = mcTruth->primaries.at(0);
    Herd::Line primaryTraj = Herd::Line(primaryPart.initialPosition, primaryPart.initialMomentum);
    for (auto side : Herd::RefFrame::Sides) {
      for (size_t iLayer = 0; iLayer < (*stkGeoParamsColl)[side].size(); iLayer++) {
        // get the geoParams for this layer
        const auto &geoParams = (*stkGeoParamsColl)[side][iLayer];
        auto normalAxis = Herd::RefFrame::ToAxis(geoParams.NormalDirection());

        // assume the normal position of the layer from one of its wafers
        float normalPos = geoParams.WaferPosition(0)[Herd::RefFrame::ToCoo(normalAxis)];

        // get intersection point between mctruth trajectory and layer
        auto intersectionPoint = primaryTraj.Intersection(normalAxis, normalPos);

        for (const auto &hit : (*stkHitsColl)[side][iLayer]){
          auto hitPos = geoParams.StripPosition(hit.VolumeID());

          auto segmCoo = Herd::RefFrame::ToCoo(geoParams.StripSegmentationDirections().first);

          if (abs(hitPos[segmCoo] - intersectionPoint[segmCoo]) < _distance) {
            primStkEDep += hit.EDep();
          }
        }
      }
    }

    */

    //Check MC interaction
    bool hasInteraction = mcTruth->primaries[0].HadronicInelasticInteractionIsPresent();
    printf("%s\n", hasInteraction ? "INT" : "NOINT");
    if (hasInteraction) {
      Herd::Point interactionPoint = mcTruth->primaries[0].HadronicInelasticInteraction().intPosition;
      printf("%f %f %f\n", interactionPoint[Herd::RefFrame::Coo::X], interactionPoint[Herd::RefFrame::Coo::Y], interactionPoint[Herd::RefFrame::Coo::Z] );
    }


  }

  digFile->Close();

  outFile->Close();
}
