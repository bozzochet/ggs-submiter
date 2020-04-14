/*
 * CaloGeomFidVolumeAlgo.h
 *
 *  Created on: 21 Oct 2019
 *      Author: Lorenzo Pacini
 */

#ifndef HERD_CALOGEOMFIDVOLUMEALGO_H_
#define HERD_CALOGEOMFIDVOLUMEALGO_H_

#include "algorithm/Algorithm.h"
#include "common/DirectionsArray.h"
#include "dataobjects/Plane.h"
#include "dataobjects/CaloGeoParams.h"
#include "CaloGeomFidVolume.h"
#include <array>

using namespace EA;

class CaloGeomFidVolumeStore;


namespace Herd {

/*! @brief An algorithm which computes information about the track inside the Calo.
 * @class CaloGeomFidVolumeAlgo CaloGeomFidVolumeAlgo.h algorithms/geometry/CaloGeomFidVolumeAlgo.h
 *
 * <B>Needed event objects:</B>
 *
 *   name          |     type          |  store      | optional       | description
 * ----------------|-------------------|-------------|----------------|-------------------------
 * mcTruth         |    MCTruth        | evStore     |    no          | Info about MC truth
 *
 * <B>Produced event objects:</B>
 *
 *   name                       | type             |   store   | description
 * -----------------------------|------------------|-----------|----------------------------------------
 * trackInfoForCaloMC           | TrackInfoForCalo | evStore   | Container of information about the track for the Calo.
 *
 */
class CaloGeomFidVolumeAlgo : public Algorithm {
public:
  /*! @brief Constructor.
   *
   * @param name The name of the algorithm object.
   */
  CaloGeomFidVolumeAlgo(const std::string &name);

  /*! @brief Initializes the planes used to compute the track calo information.
   *
   * @return true if initialization is done without errors, false otherwise.
   */
  bool Initialize();

  /*! @brief Computes the track information, so far it use the MC truth.
   *
   * @return true if no error occurs during processing, false otherwise.
   */
  bool Process();

  /*! @brief Do nothing.
   *
   * @return true if no error occurs during finalization, false otherwise.
   */
  bool Finalize();

private:
  
  //Store pointer
  std::shared_ptr<CaloGeomFidVolumeStore> _processstore;

  observer_ptr<EventDataStore> _evStore; ///< Pointer to the event data store.
  //TrackInfoForCalo *_trackInfoCalo; ///< The TrackInfoForCalo object to fill with the computed information.
  DirectionsArray<Plane> _Planes; ///< The calo surface for each directions.

  bool filterenable;

  const float _XSideBig;    // cm
  const float _XSideSmall;  // cm
  const float _YSideBig;    // cm
  const float _YSideSmall;  // cm
  const float _ZCaloCenter; // cm
  const float _ZCaloHeight; // cm
  const float _phiXY;
  float cubeside; //cm
  float alpha;    //fraction of cube size to be contained in the fiducuial volume

  void FillCoo(const Herd::Point p, float coo[2][3], int index);

  // const float _meanActiveFractionZview; // 21*3/73.2; old parameter, not used now
  // const float _meanActiveFractionXview; // 21*3/79;
  // const float _meanActiveFractionYview; // 21*3/73.2;
  //const float _meanVolumeActiveFraction; // (7497*3*3*3)  / ( 33,4×33,2×73,2 + 4×( (79−33,4)×(73,2−33,2)×73,2 )÷2 ) =
                                         // 0,581323659
  //const float _LYSO_X0;                 // cm
};                                      // namespace Herd

} // namespace Herd

class CaloGeomFidVolumeStore : public Algorithm {
public:
  CaloGeomFidVolumeStore(const std::string &name);
  bool Initialize();
  bool Process();
  bool Finalize();
  bool Reset();

  float calofidvolalpha;
  bool calofidvolpass;
  short calofidvolxpos;
  short calofidvolxneg;
  short calofidvolypos;
  short calofidvolyneg;
  short calofidvolzpos;
  short calofidvolzneg;
  short calofidvolxnegyneg;
  short calofidvolxposyneg;
  short calofidvolxnegypos;
  short calofidvolxposypos;
  float calofidvolxposEntry[2][3];
  float calofidvolxnegEntry[2][3];
  float calofidvolyposEntry[2][3];
  float calofidvolynegEntry[2][3];
  float calofidvolzposEntry[2][3];
  float calofidvolznegEntry[2][3];
  float calofidvolxnegynegEntry[2][3];
  float calofidvolxposynegEntry[2][3];
  float calofidvolxnegyposEntry[2][3];
  float calofidvolxposyposEntry[2][3];

  
private:
};

#endif /* HERD_CALOGEOMFIDVOLUMEALGO_H_ */
