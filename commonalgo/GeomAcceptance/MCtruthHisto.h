/*
 * CaloEDepHisto.h
 *
 *  Created on: 11 Feb 2019
 *      Author: Nicola Mori
 */

#ifndef MCTRUTHHISTO_H_
#define MCTRUTHHISTO_H_

#include "algorithm/Algorithm.h"

// HerdSoftware headers
#include "dataobjects/MCTruth.h"
#include "dataobjects/CaloHits.h"
#include "dataobjects/TrackInfoForCalo.h"

using namespace EA;

class TH1F;
class TH2F;
class TH3F;
class TGraph;
class TGraph2D;
class TVector3;

/*! @brief An algorithm that plots the distribution of energy deposits in the calorimeter.
 *
 * <B>Needed event objects:</B>
 *
 *   name       |     type    |  store  | optional | description
 * -------------|-------------|---------|----------|-------------------------
 * caloHits     |   CaloHits   | evStore |    no    | Hits in the CALO.
 *
 * <B>Produced gloabl objects:</B>
 *
 *   name           | type |   store   | description
 * -----------------|------|-----------|---------|-------------------------------
 * <algoName>       | TH1F | globStore | Histogram of energy deposit in the CALO.
 *
 * <B>Parameters:</B>
 *
 *  name       |         type       | default value | description
 * ------------|--------------------|---------------|-----------------------------------------------------------------------------------------
 *  axis       | std::vector<float> | {100, 0, 100} | The parameters of the X axis {nBins, minEDep, maEDep} [energy
 * deposits in GeV].
 */
class MCtruthHisto : public Algorithm {
public:
  /*! @brief Constructor.
   *
   * @param name The name of the algorithm object.
   */
  MCtruthHisto(const std::string &name);

  /*! @brief Initializes the histogram.
   *
   * @return true if initialization is done without errors, false otherwise.
   */
  bool Initialize();

  /*! @brief Fills the histogram.
   *
   * @return true if no error occurs during processing, false otherwise.
   */
  bool Process();

  /*! @brief Puts the histogram on the event store.
   *
   * @return true if no error occurs during finalization, false otherwise.
   */
  bool Finalize();

private:
  // Algorithm parameters
  //std::vector<float> _axis;

  // Created global objects
  // std::shared_ptr<TH1F> _histo; // Objects to be pushed on global store must be held by a shared_ptr
  std::shared_ptr<TH2F> _hgencthetaphi;
  std::shared_ptr<TH3F> _hgencoo;
  std::shared_ptr<TGraph2D> _ggencoo;
  std::shared_ptr<TGraph2D> _gcaloentry;
  std::shared_ptr<TGraph2D> _gcaloexit;
  std::shared_ptr<TH1F>_hshowerlength[Herd::RefFrame::NDirections][Herd::RefFrame::NDirections];

  //std::shared_ptr<TH2F> _hgencoo;

  // Utility variables
  observer_ptr<EventDataStore> _evStore; // Pointer to the event data store

  TVector3 InterceptX(double, const TVector3 &, const TVector3 &) const;
};

#endif /* MCTRUTHHISTO_H_ */
