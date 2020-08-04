/*
 * CaloLongitudinalProfile.h
 *
 *  Created on: 27 April 2020
 *      Author: Valerio Vagelli
 */

/*! @file CaloLongitudinalProfile.h CaloLongitudinalProfile class declaration, */

#ifndef HERD_CALOLongitudinalProfile_H_
#define HERD_CALOLongitudinalProfile_H_

// HerdSoftware headersCalo
#include "common/Vec3D.h"
#include "dataobjects/Point.h"

// C/C++ standard headers
#include <array>
#include <numeric>
#include <map>

namespace Herd {

enum class CaloLongitudinalProfileAlgoType {
  NONE, /**< Reconstruction algorithm not specified */
  PCA /**< Principal Component Analysis algorithm, generated with PcaCaloLongitudinalProfileTrackingAlgo */
 };

/*! @brief Container of information about the Calorimeter Shower %LongitudinalProfile.
 * @struct CaloLongitudinalProfile CaloLongitudinalProfile.h dataobjects/CaloLongitudinalProfile.h
 *
 * blablabla
 */
struct CaloLongitudinalProfile {

  /// @brief Flag provinding the algorithm used for the reconstruction of the CaloLongitudinalProfile object
  CaloLongitudinalProfileAlgoType longProfAlgo;

  /// @brief Number of hits used for LongitudinalProfile reconstruction
  unsigned short longProfHits;

/*! @brief Vector of hits of CaloLongitudinalProfile in LongitudinalProfile reference frame (centered around COG, rotated by principal axes)
   *
   * 
   *
   */
  std::vector<Hit> longProfFrameHits;


  /*! @brief %LongitudinalProfile Point of first interaction
   *
   * Expressed in Axis reference frame (cm)
   *
   */
  Point longProfFirstInt;
  

  /*! @brief Default constructor.
   *
   * Creates a CaloLongitudinalProfile with standard values
   *
   */

  
  CaloLongitudinalProfile() { Reset(); };

  /*!
   * @brief Set the members to default values
   *
   */
  void Reset();
};

} // namespace Herd

#endif /* HERD_CALOLongitudinalProfile_H_ */
