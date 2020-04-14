/*
 * TreePersistence.cpp
 *
 *  Created on: 3 Feb 2020
 *      Author: Nicola Mori
 */

#ifndef TREEPERSISTENCE_H_
#define TREEPERSISTENCE_H_

// HerdSoftware headers
#include "dataobjects/Momentum.h"
#include "dataobjects/Point.h"

// EventAnalysis headers
#include "persistence/PersistenceService.h"

using namespace EA;

// HERD data object classes
namespace Herd {
class MCTruth;
}

// Root classes
class TTree;
class TFile;

/*! @brief A persistence which saves select event information on a simple Root tree.
 *
 * This class saves some event information as leaves of a Root tree.
 */

class TreePersistence : public PersistenceService {
public:
  /*! @brief Constructor.
   *
   * @param name The name of the persistence object.
   * @param output Name of the output file.
   */
  TreePersistence(const std::string &name, const std::string &output);

  /*! @brief Set up the output.
   *
   * Opens the output file and creates the output tree.
   *
   * @return true if the setup procedure succeeds.
   */
  bool Connect();

  /*! @brief finalize the output.
   *
   * Writes the tree and closes the output file.
   *
   * @return true if the setup finalization succeeds.
   */
  bool Disconnect();

  /*! @brief Book the optional objects.
   *
   * Currently the only optional objects that can be booked is "psdHitsCollMC". Booking it, the multiplicity of the hits
   * on top PSD will be saved in the output file.
   *
   * @param objName The name of the object to be booked.
   * @param objStore the data store
   * @return true if the object has been correctly booked.
   */
  bool BookEventObject(const std::string &objName, const std::string &objStore);

  /*! @brief Fill the tree branches with information from current event.
   *
   * @return false if the needed event objects are not available.
   */
  bool EndOfEvent();





private:
  // Pointers to Root objects
  std::unique_ptr<TFile> _outputFile;
  observer_ptr<TTree> _outputTree;

  // Output data
  bool bookMCtruthProcess;
  int mcNdiscarded;
  float mcDir[3];
  float mcCoo[3];
  float mcMom;
  float mcPhi;
  float mcCtheta;
  int mcStkintersections;
  float mcTracklengthcalox0;
  float mcTrackcaloentry[3];
  float mcTrackcaloexit[3];
  int mcTrackcaloentryplane;
  int mcTrackcaloexitplane;

  bool bookCaloGeomFidVolume;
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
  
  // Flag for optional output objects
  //bool _isPSDBooked;
  //std::string _psdStoreName; // Name of the data store where the PSD data object is located

  // Utility variables
  StorePtr _evStore;
  //StorePtr _psdEvStore;

  // Configuration parameters
  //float _psdHitThreshold;
};

#endif /* TREEPERSISTENCE_H_ */
