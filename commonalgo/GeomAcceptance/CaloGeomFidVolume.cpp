/*
 * CaloGeomFidVolumeAlgo.cpp
 *
 *  Created on: 03 June 2019
 *      Author: Lorenzo Pacini
 */

// HerdSoftware headers
#include "dataobjects/Line.h"
#include "dataobjects/MCTruth.h"
#include "dataobjects/Momentum.h"
#include "dataobjects/Point.h"
#include "math.h"
#include "CaloGeomFidVolume.h"

// C/C++ standard headers
#include <numeric>

#define DEBUG false

using namespace std;

namespace Herd {

RegisterAlgorithm(CaloGeomFidVolumeAlgo);
RegisterAlgorithm(CaloGeomFidVolumeStore);

CaloGeomFidVolumeAlgo::CaloGeomFidVolumeAlgo(const std::string &name)
    : Algorithm{name}, _XSideBig(79), _XSideSmall(33.4), _YSideBig(73.2), _YSideSmall(32.4), _ZCaloCenter(-36.6),
      _ZCaloHeight(73.2), _phiXY(atan2(-(-_XSideBig + _XSideSmall), (-_YSideSmall + _YSideBig))),alpha(1.),filterenable{true}
      //_meanActiveFractionZview(0.8606557), _meanActiveFractionXview(0.7974684), _meanActiveFractionYview(0.8606557),
      //_meanVolumeActiveFraction(0.569861492), _LYSO_X0(1.1) 
      {
  const std::string routineName = GetName() + "::CaloGeomFidVolumeAlgo";

  DeclareConsumedObject("mcTruth", ObjectCategory::EVENT, "evStore");

  DefineParameter("alpha", alpha);
  DefineParameter("filterenable", filterenable);

   // Top and bottom planes                                                                                                                                                                                                                  
  _Planes[RefFrame::Direction::Zpos] = Plane(Point(0, 0, _ZCaloCenter + _ZCaloHeight / 2.), M_PI, 0);
  _Planes[RefFrame::Direction::Zneg] = Plane(Point(0., 0., _ZCaloCenter - _ZCaloHeight / 2.), 0, 0);

  // planes parallel to X and Y axes                                                                                                                                                                                                        
  _Planes[RefFrame::Direction::Xneg] = Plane(Point(-_XSideBig / 2., 0., _ZCaloCenter), M_PI / 2., M_PI);
  _Planes[RefFrame::Direction::Xpos] = Plane(Point(+_XSideBig / 2., 0., _ZCaloCenter), M_PI / 2., 0);
  _Planes[RefFrame::Direction::Yneg] = Plane(Point(0., -_YSideBig / 2., _ZCaloCenter), M_PI / 2., M_PI * 3. / 2.);
  _Planes[RefFrame::Direction::Ypos] = Plane(Point(0., +_YSideBig / 2., _ZCaloCenter), M_PI / 2., M_PI / 2.);

  // inclined planes                                                                                                                                                                                                                        
  _Planes[RefFrame::Direction::XnegYneg] =
      Plane(Point((-_XSideBig / 2 - _XSideSmall / 2.) / 2., (-_YSideBig / 2 - _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., M_PI + _phiXY);
  _Planes[RefFrame::Direction::XposYneg] =
      Plane(Point((_XSideBig / 2 + _XSideSmall / 2.) / 2., (-_YSideBig / 2 - _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., 2 * M_PI - _phiXY);
  _Planes[RefFrame::Direction::XnegYpos] =
      Plane(Point((-_XSideBig / 2 - _XSideSmall / 2.) / 2., (_YSideBig / 2 + _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., M_PI - _phiXY);
  _Planes[RefFrame::Direction::XposYpos] =
      Plane(Point((_XSideBig / 2 + _XSideSmall / 2.) / 2., (_YSideBig / 2 + _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., _phiXY);

}

bool CaloGeomFidVolumeAlgo::Initialize() {
  const std::string routineName = GetName() + "::Initialize";

  // Setup the filter                                                                                                                                                                                                                       
  if (filterenable) SetFilterStatus(FilterStatus::ENABLED); else SetFilterStatus(FilterStatus::DISABLED);

  _evStore = GetDataStoreManager()->GetEventDataStore("evStore");
  if (!_evStore) { COUT(ERROR) << "Event data store not found." << ENDL; return false; }

  _processstore = std::make_shared<CaloGeomFidVolumeStore>("caloGeomFidVolumeStore");
  COUT(INFO) << "InitializedProcessStore::" <<_processstore << ENDL;

auto globStore = GetDataStoreManager()->GetGlobalDataStore("globStore");
  if (!globStore) {COUT(ERROR) << "Global data store not found." << ENDL;}
  auto caloGeoParams = globStore->GetObject<Herd::CaloGeoParams>("caloGeoParams");
  if (!caloGeoParams) {COUT(ERROR) << "Event data store not found." << ENDL;}
  cubeside = caloGeoParams->CubeSize();

  return true;
}

bool CaloGeomFidVolumeAlgo::Process() {
  const std::string routineName = GetName() + "::Process";

  //Add the ProcessStore object for this event to the event data store
  _processstore->Reset();
  _evStore->AddObject("caloGeomFidVolumeStore",_processstore);

  auto mcTruth = _evStore->GetObject<MCTruth>("mcTruth");
  if (!mcTruth) {COUT(ERROR) << "mcTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;return false;}
  const Momentum &Mom = mcTruth->primaries[0].initialMomentum;
  const Point &Pos    = mcTruth->primaries[0].initialPosition;
  Line track(Pos, Mom);

  //Build additional planes
  Plane PlaneX6 (Point( -_XSideSmall / 2., 0., _ZCaloCenter), M_PI / 2., M_PI);          //verify these angles...
  Plane PlaneX14(Point( +_XSideSmall / 2., 0., _ZCaloCenter), M_PI / 2., 0);             //verify these angles...
  Plane PlaneY6 (Point(0., -_YSideSmall / 2., _ZCaloCenter), M_PI / 2., M_PI * 3. / 2.);  //verify these angles...
  Plane PlaneY14(Point(0., +_YSideSmall / 2., _ZCaloCenter), M_PI / 2., M_PI / 2.);       //verify these angles...

  //Prepare additional variables
  double mXNEGYNEG = tan(_Planes[RefFrame::Direction::XnegYneg].Azimuth() + M_PI / 2.);
  double qXNEGYNEG = _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::Y] - mXNEGYNEG * _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::X];
  double mXPOSYNEG = tan(_Planes[RefFrame::Direction::XposYneg].Azimuth() + M_PI / 2.);
  double qXPOSYNEG = _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::Y] - mXPOSYNEG * _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::X];
  double mXNEGYPOS = tan(_Planes[RefFrame::Direction::XnegYpos].Azimuth() + M_PI / 2.);
  double qXNEGYPOS = _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::Y] - mXNEGYPOS * _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::X];
  double mXPOSYPOS = tan(_Planes[RefFrame::Direction::XposYpos].Azimuth() + M_PI / 2.);
  double qXPOSYPOS = _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::Y] - mXPOSYPOS * _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::X];
  
  //Get Intersection with planes
  Point intXneg = _Planes[RefFrame::Direction::Xneg].Intersection(track);
  Point intXpos = _Planes[RefFrame::Direction::Xpos].Intersection(track);
  Point intX6   = PlaneX6. Intersection(track);
  Point intX14  = PlaneX14.Intersection(track);
  Point intYneg = _Planes[RefFrame::Direction::Yneg].Intersection(track);
  Point intYpos = _Planes[RefFrame::Direction::Ypos].Intersection(track);
  Point intY6   = PlaneY6. Intersection(track);
  Point intY14  = PlaneY14.Intersection(track);
  Point intXnegYneg = _Planes[RefFrame::Direction::XnegYneg].Intersection(track);
  Point intXnegYpos = _Planes[RefFrame::Direction::XnegYpos].Intersection(track);
  Point intXposYneg = _Planes[RefFrame::Direction::XposYneg].Intersection(track);
  Point intXposYpos = _Planes[RefFrame::Direction::XposYpos].Intersection(track);
  Point intZneg = _Planes[RefFrame::Direction::Zneg].Intersection(track);
  Point intZpos = _Planes[RefFrame::Direction::Zpos].Intersection(track);
  
  _processstore->calofidvolpass=true;
  _processstore->calofidvolalpha=alpha;

//LATERAL CAP CHECK: XPOS
  int nintXpos=0;
  if( intXpos[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&   
      intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXpos[RefFrame::Coo::Z] < 0.                              ) nintXpos++;
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&   
      intZpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) nintXpos++;
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&
      intZneg[RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&   
      intZneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) nintXpos++;
  if( intY6  [RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intY6  [RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&
      intY6  [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intY6  [RefFrame::Coo::Z] < 0.                              ) nintXpos++;
  if( intY14 [RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intY14 [RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&
      intY14 [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intY14 [RefFrame::Coo::Z] < 0.                              ) nintXpos++;
  if( nintXpos >  2) {COUT(ERROR)<<"intXpos>2"<<ENDL;} //}return false;}    
  if( nintXpos == 2) {    
    _processstore->calofidvolxpos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxpos=0;} 

  //LATERAL CAP CHECK: XNEG
  int nintXneg=0;
  if( intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&   
      intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXneg[RefFrame::Coo::Z] < 0.                              ) { /*COUT(ERROR)<<"Xneg"<<ENDL;*/ nintXneg++; }
  if( intZpos[RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intZpos[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { /*COUT(ERROR)<<"Xneg"<<ENDL;*/ nintXneg++; }
  if( intZneg[RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intZneg[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { /*COUT(ERROR)<<"Xneg"<<ENDL;*/ nintXneg++; }
  if( intY6  [RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intY6  [RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intY6  [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intY6  [RefFrame::Coo::Z] < 0.                              ) { /*COUT(ERROR)<<"Xneg"<<ENDL;*/ nintXneg++; }
  if( intY14 [RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intY14 [RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intY14 [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intY14 [RefFrame::Coo::Z] < 0.                              ) { /*COUT(ERROR)<<"Xneg"<<ENDL;*/ nintXneg++; }
  if( nintXneg >  2) {COUT(ERROR)<<"intXneg>2"<<ENDL; 
    COUT(ERROR)<<GetEventLoopProxy()->GetCurrentEvent()<<ENDL;
    COUT(ERROR)<<nintXneg<<ENDL;
    COUT(ERROR)<<intXneg[RefFrame::Coo::X]<<" "<<intXneg[RefFrame::Coo::Y]<<" "<<intXneg[RefFrame::Coo::Z]<<ENDL;
    COUT(ERROR)<<intZpos[RefFrame::Coo::X]<<" "<<intZpos[RefFrame::Coo::Y]<<" "<<intZpos[RefFrame::Coo::Z]<<ENDL;
    COUT(ERROR)<<intZneg[RefFrame::Coo::X]<<" "<<intZneg[RefFrame::Coo::Y]<<" "<<intZneg[RefFrame::Coo::Z]<<ENDL;
    COUT(ERROR)<<intY6  [RefFrame::Coo::X]<<" "<<intY6  [RefFrame::Coo::Y]<<" "<<intY6  [RefFrame::Coo::Z]<<ENDL;
    COUT(ERROR)<<intY14 [RefFrame::Coo::X]<<" "<<intY14 [RefFrame::Coo::Y]<<" "<<intY14 [RefFrame::Coo::Z]<<ENDL;
  }
  //return false;}    
  if( nintXneg == 2) {
    _processstore->calofidvolxneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxneg=0;}
    
//LATERAL CAP CHECK: YPOS
  int nintYpos=0;
  if( intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&   
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intYpos[RefFrame::Coo::Z] < 0.                              ) nintYpos++;
  if( intZpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside    ) nintYpos++;
  if( intZneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside    ) nintYpos++;
  if( intX6  [RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intX6  [RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside  &&
      intX6  [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intX6  [RefFrame::Coo::Z] < 0.                              ) nintYpos++;
  if( intX14 [RefFrame::Coo::Y] < +_YSideBig/2.                    &&  
      intX14 [RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside  &&
      intX14 [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intX14 [RefFrame::Coo::Z] < 0.                              ) nintYpos++;
  if( nintYpos >  2) {COUT(ERROR)<<"intYpos>2"<<ENDL;}// return false;}    
  if( nintYpos == 2) {
    _processstore->calofidvolypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolypos=0;}


  
//LATERAL CAP CHECK: YNEG
  int nintYneg=0;
  if( intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&   
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intYneg[RefFrame::Coo::Z] < 0.                              ) nintYneg++;
  if( intZpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside    ) nintYneg++;
  if( intZneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside    ) nintYneg++;
  if( intX6  [RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intX6  [RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside  &&   
      intX6  [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intX6  [RefFrame::Coo::Z] < 0.                              ) nintYneg++;
  if( intX14 [RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intX14 [RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside  &&    
      intX14 [RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intX14 [RefFrame::Coo::Z] < 0.                              ) nintYneg++;
  if( nintYneg >  2) {COUT(ERROR)<<"intYneg>2"<<ENDL;}// return false;}    
  if( nintYneg == 2) {    
    _processstore->calofidvolyneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolyneg=0;} 

//TOP CAP CHECK: ZPOS
  // Assume an octagon
  int nintZpos=0;
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                  &&   
      intZpos[RefFrame::Coo::X] > -_XSideBig/2.                  &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                  &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                  &&
      intZpos[RefFrame::Coo::Y] < mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZpos[RefFrame::Coo::Y] < mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZpos[RefFrame::Coo::Y] > mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG     ) nintZpos++;       
  if( intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::Z] < 0                             &&
      intYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) nintZpos++;
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::Z] < 0                             &&
      intYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) nintZpos++;
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Z] < 0                             &&
      intXneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) nintZpos++;
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::X] < +_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Z] < 0                             &&
      intXpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) nintZpos++;
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] > -_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] < -_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::Z] < 0                         &&
      intXposYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) nintZpos++;
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Y] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::Y] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Z] < 0                         &&
      intXnegYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) nintZpos++;
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] > +_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] < +_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::Z] < 0                         &&
      intXnegYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) nintZpos++;
  if( intXposYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXposYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] > +_XSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] < +_XSideBig/2.                &&
      intXposYpos[RefFrame::Coo::Z] < 0                         &&
      intXposYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) nintZpos++;
  if( nintZpos >  2) {COUT(ERROR)<<"intZpos>2"<<ENDL;}// return false;}     
  if( nintZpos == 2) {
    _processstore->calofidvolzpos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolzpos=0;} 
  
  //BOTTOM CAP CHECK: ZNEG
  // Assume an octagon
  int nintZneg=0;
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                 &&   
      intZneg[RefFrame::Coo::X] > -_XSideBig/2.                  &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                  &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                  &&
      intZneg[RefFrame::Coo::Y] < mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZneg[RefFrame::Coo::Y] < mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZneg[RefFrame::Coo::Y] > mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG     ) nintZneg++;       
  if( intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                        ) nintZneg++;
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) nintZneg++;
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intXneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) nintZneg++;
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::X] < +_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&
      intXpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) nintZneg++;
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] > -_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] < -_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::Z] > -_ZCaloHeight             &&
      intXposYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) nintZneg++;
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Y] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::Y] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Z] > -_ZCaloHeight             &&
      intXnegYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) nintZneg++;
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] > +_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] < +_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::Z] > -_ZCaloHeight             &&
      intXnegYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) nintZneg++;
  if( intXposYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXposYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] > +_XSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] < +_XSideBig/2.                &&
      intXposYpos[RefFrame::Coo::Z] > -_ZCaloHeight             &&
      intXposYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) nintZneg++;
  if( nintZpos >  2) {COUT(ERROR)<<"intZpos>2"<<ENDL;}// return false;}     
  if( nintZpos == 2) {
    _processstore->calofidvolzneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolzneg=0;}
    
//CORNER CAP CHECK: XNEGYNEG
  int nintXegYneg=0; 
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.                  &&   
      intXnegYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXnegYneg[RefFrame::Coo::Z] < 0.                              ) nintXegYneg++;
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                   &&   
      intXneg[RefFrame::Coo::Y] < -_YSideSmall/2. + alpha*cubeside  &&     
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXneg[RefFrame::Coo::Z] < 0.                                  ) nintXegYneg++;
  if( intYneg[RefFrame::Coo::Y] < +_YSideBig/2.                        &&   
      intYneg[RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside      &&   
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXneg[RefFrame::Coo::Z] < 0.                                 ) nintXegYneg++;
  if( intZpos[RefFrame::Coo::X] > -_XSideBig/2.                       &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                        &&
      intZpos[RefFrame::Coo::Y] > mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG  &&
      intZpos[RefFrame::Coo::Y] < mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG  + alpha*cubeside) nintXegYneg++;
  if( intZneg[RefFrame::Coo::X] > -_XSideBig/2.                        &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                        &&
      intZneg[RefFrame::Coo::Y] > mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG  &&
      intZneg[RefFrame::Coo::Y] < mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG  + alpha*cubeside) nintXegYneg++;
  if( nintXegYneg >  2) {COUT(ERROR)<<"nintXegYneg>2"<<ENDL;}// return false;}    
  if( nintXegYneg == 2) {
    _processstore->calofidvolxnegyneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxnegyneg=0;}

//CORNER CAP CHECK: XPOSYNEG
  int nintXposYneg=0; 
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.                  &&   
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intXposYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXposYneg[RefFrame::Coo::Z] < 0.                              ) nintXposYneg++;
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                   &&   
      intXpos[RefFrame::Coo::Y] < -_YSideSmall/2. + alpha*cubeside  &&     
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXpos[RefFrame::Coo::Z] < 0.                                  ) nintXposYneg++;
  if( intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                   &&   
      intYneg[RefFrame::Coo::X] > +_XSideSmall/2  - alpha*cubeside  &&     
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYneg[RefFrame::Coo::Z] < 0.                                  ) nintXposYneg++;
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                        &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                        &&
      intZpos[RefFrame::Coo::Y] > mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZpos[RefFrame::Coo::Y] < mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  + alpha*cubeside) nintXposYneg++;
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                    &&
      intZneg[RefFrame::Coo::Y] > mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZneg[RefFrame::Coo::Y] < mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG + alpha*cubeside) nintXposYneg++;
  if( nintXposYneg >  2) {COUT(ERROR)<<"nintXposYneg>2"<<ENDL; }//return false;}    
  if( nintXposYneg == 2) {
    _processstore->calofidvolxposyneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxposyneg=0;}  

  

//CORNER CAP CHECK: XNEGYPOS
  int nintXnegYpos=0; 
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.                  &&   
      intXnegYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXnegYpos[RefFrame::Coo::Z] < 0.                              ) nintXnegYpos++;
  if( intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                   &&   
      intXneg[RefFrame::Coo::Y] > +_YSideSmall/2. - alpha*cubeside  &&     
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXneg[RefFrame::Coo::Z] < 0.                                  ) nintXnegYpos++;
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                   &&   
      intYpos[RefFrame::Coo::X] < -_XSideSmall/2  + alpha*cubeside  &&     
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYpos[RefFrame::Coo::Z] < 0.                                  ) nintXnegYpos++;
  if( intZpos[RefFrame::Coo::X] > -_XSideBig/2.                        &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                        &&
      intZpos[RefFrame::Coo::Y] < mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  - alpha*cubeside) nintXnegYpos++;
  if( intZneg[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                    &&
      intZneg[RefFrame::Coo::Y] < mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS - alpha*cubeside) nintXnegYpos++;
  if( nintXnegYpos >  2) {COUT(ERROR)<<"nintXnegYpos>2"<<ENDL; }//return false;}    
  if( nintXnegYpos == 2) {
    _processstore->calofidvolxnegypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxnegypos=0;}

//CORNER CAP CHECK: XPOSYPOS
  int nintXposYpos=0; 
  if( intXposYpos[RefFrame::Coo::X] > +_XSideSmall/2.                  &&   
      intXposYpos[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intXposYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXposYpos[RefFrame::Coo::Z] < 0.                              ) nintXposYpos++;
  if( intXpos[RefFrame::Coo::Y] < +_YSideSmall/2.                   &&   
      intXpos[RefFrame::Coo::Y] > +_YSideSmall/2. - alpha*cubeside  &&     
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXpos[RefFrame::Coo::Z] < 0.                                  ) nintXposYpos++;
  if( intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                   &&   
      intYpos[RefFrame::Coo::X] > +_XSideSmall/2  - alpha*cubeside  &&     
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYpos[RefFrame::Coo::Z] < 0.                                  ) nintXposYpos++;
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                        &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                        &&
      intZpos[RefFrame::Coo::Y] < mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  - alpha*cubeside) nintXposYpos++;
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                    &&
      intZneg[RefFrame::Coo::Y] < mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS - alpha*cubeside) nintXposYpos++;
  if( nintXposYpos >  2) {COUT(ERROR)<<"nintXposYpos>2"<<ENDL;}// return false;}    
  if( nintXposYpos == 2) {
    _processstore->calofidvolxposypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxposypos=0;} 

  



  SetFilterResult(FilterResult::ACCEPT);
  return true;
}

bool CaloGeomFidVolumeAlgo::Finalize() { return true; }

}



//***************************

CaloGeomFidVolumeStore::CaloGeomFidVolumeStore(const std::string &name) :
  Algorithm{name}
   {
  }

  bool CaloGeomFidVolumeStore::Initialize() {
  const std::string routineName("CaloGeomFidVolumeStore::Initialize");
  Reset();
  return true;
}

  bool CaloGeomFidVolumeStore::Process() {
  const std::string routineName("CaloGeomFidVolumeStore::Process");
  return true;
}
  bool CaloGeomFidVolumeStore::Finalize() {
  const std::string routineName("CaloGeomFidVolumeStore::Finalize");
  return true;
}
bool CaloGeomFidVolumeStore::Reset() {
  const std::string routineName("CaloGeomFidVolumeStore::Finalize");

  calofidvolalpha = 0;
  calofidvolpass = true;
  calofidvolxpos = 0;
  calofidvolxneg = 0;
  calofidvolypos = 0;
  calofidvolyneg = 0;
  calofidvolzpos = 0;
  calofidvolzneg = 0;
  calofidvolxnegyneg = 0;
  calofidvolxposyneg = 0;
  calofidvolxnegypos = 0;
  calofidvolxposypos = 0;

  return true;
}

/*bool CaloGeomFidVolumeAlgo::_CheckIfPointIsInCaloSurface(Point p, RefFrame::Direction view) {
  // top or bottom surface: computed as a rectangle + 4 triangle
  if (view == RefFrame::Direction::Zpos || view == RefFrame::Direction::Zneg) {
    // check the big rectangle
    if ((p[RefFrame::Coo::X] > -_XSideBig / 2.) && (p[RefFrame::Coo::X] < _XSideBig / 2.) &&
        (p[RefFrame::Coo::Y] > -_YSideBig / 2.) && (p[RefFrame::Coo::Y] < _YSideBig / 2.)) {
      // check if it above the lines which define the Y neg inclined planes
      double mXNEGYNEG = tan(_Planes[RefFrame::Direction::XnegYneg].Azimuth() + M_PI / 2.);
      double qXNEGYNEG = _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::Y] -
                         mXNEGYNEG * _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::X];
      double mXPOSYNEG = tan(_Planes[RefFrame::Direction::XposYneg].Azimuth() + M_PI / 2.);
      double qXPOSYNEG = _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::Y] -
                         mXPOSYNEG * _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::X];
      if ((p[RefFrame::Coo::Y] < (mXNEGYNEG * p[RefFrame::Coo::X] + qXNEGYNEG)) ||
          (p[RefFrame::Coo::Y] < (mXPOSYNEG * p[RefFrame::Coo::X] + qXPOSYNEG))) {
        return false;
      }
      // check if it below the lines which define y pos planes
      double mXNEGYPOS = tan(_Planes[RefFrame::Direction::XnegYpos].Azimuth() + M_PI / 2.);
      double qXNEGYPOS = _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::Y] -
                         mXNEGYPOS * _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::X];
      double mXPOSYPOS = tan(_Planes[RefFrame::Direction::XposYpos].Azimuth() + M_PI / 2.);
      double qXPOSYPOS = _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::Y] -
                         mXPOSYPOS * _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::X];
      if ((p[RefFrame::Coo::Y] > (mXNEGYPOS * p[RefFrame::Coo::X] + qXNEGYPOS)) ||
          (p[RefFrame::Coo::Y] > (mXPOSYPOS * p[RefFrame::Coo::X] + qXPOSYPOS))) {
        return false;
      }
      // line is in the top or bottom planes
      return true;
    }
  } else if (view == RefFrame::Direction::Xneg || view == RefFrame::Direction::Xpos) {
    if ((p[RefFrame::Coo::Y] > -_YSideSmall / 2.) && (p[RefFrame::Coo::Y] < _YSideSmall / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  } else if (view == RefFrame::Direction::Yneg || view == RefFrame::Direction::Ypos) {
    if ((p[RefFrame::Coo::X] > -_XSideSmall / 2.) && (p[RefFrame::Coo::X] < _XSideSmall / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  } else if (view == RefFrame::Direction::XnegYneg) {
    if ((p[RefFrame::Coo::X] > -_XSideBig / 2.) && (p[RefFrame::Coo::X] < -_XSideSmall / 2.) &&
        (p[RefFrame::Coo::Y] > -_YSideBig / 2.) && (p[RefFrame::Coo::Y] < -_YSideSmall / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  } else if (view == RefFrame::Direction::XposYneg) {
    if ((p[RefFrame::Coo::X] > _XSideSmall / 2.) && (p[RefFrame::Coo::X] < _XSideBig / 2.) &&
        (p[RefFrame::Coo::Y] > -_YSideBig / 2.) && (p[RefFrame::Coo::Y] < -_YSideSmall / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  } else if (view == RefFrame::Direction::XposYpos) {
    if ((p[RefFrame::Coo::X] > _XSideSmall / 2.) && (p[RefFrame::Coo::X] < _XSideBig / 2.) &&
        (p[RefFrame::Coo::Y] > _YSideSmall / 2.) && (p[RefFrame::Coo::Y] < _YSideBig / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  } else if (view == RefFrame::Direction::XnegYpos) {
    if ((p[RefFrame::Coo::X] > -_XSideBig / 2.) && (p[RefFrame::Coo::X] < -_XSideSmall / 2.) &&
        (p[RefFrame::Coo::Y] > _YSideSmall / 2.) && (p[RefFrame::Coo::Y] < _YSideBig / 2.) &&
        (p[RefFrame::Coo::Z] > (_ZCaloCenter - _ZCaloHeight / 2.)) &&
        (p[RefFrame::Coo::Z] < (_ZCaloCenter + _ZCaloHeight / 2.))) {
      return true;
    }
  }

  return false;
}

float CaloGeomFidVolumeAlgo::_PntDistance(Point p1, Point p2) {
  const std::string routineName = GetName() + "::_PntDistance";

  float dist = 0;
  for (int iVect = 0; iVect < 3; iVect++) {
    dist += pow(p1[static_cast<RefFrame::Coo>(iVect)] - p2[static_cast<RefFrame::Coo>(iVect)], 2);
  }
  dist = sqrt(dist);
  return dist;
}
*/
/*
float CaloGeomFidVolumeAlgo::_ComputeTrackLengthCaloX0() {

  float projXcm = _trackInfoCalo->entrance[RefFrame::Coo::X] - _trackInfoCalo->exit[RefFrame::Coo::X];
  float projYcm = _trackInfoCalo->entrance[RefFrame::Coo::Y] - _trackInfoCalo->exit[RefFrame::Coo::Y];
  float projZcm = _trackInfoCalo->entrance[RefFrame::Coo::Z] - _trackInfoCalo->exit[RefFrame::Coo::Z];
  float projXX0 = projXcm * _meanActiveFractionXview / _LYSO_X0;
  float projYX0 = projYcm * _meanActiveFractionYview / _LYSO_X0;
  float projZX0 = projZcm * _meanActiveFractionZview / _LYSO_X0;
  float retval = sqrt(projXX0 * projXX0 + projYX0 * projYX0 + projZX0 * projZX0);


  return retval;
}
*/
/*
bool CaloGeomFidVolumeAlgo::_ComputeTrackInfoForCalo(const Point startingPoint, Vec3D direction) {

  const std::string routineName = GetName() + "::_ComputeTrackInfoForCalo";

  // line with MC track
  Line MCtrack(startingPoint, direction);

  // compute intersections with the infinite Calo surfaces
  DirectionsArray<Point> intersections;
  for_each(RefFrame::Directions.begin(), RefFrame::Directions.end(),
           [&](auto &itDir) { intersections[itDir] = _Planes[itDir].Intersection(MCtrack); });

  if (DEBUG) {
    cout << "Position: ";
    startingPoint.Dump();
    cout << "Direction: ";
    direction.Dump();
    auto itName = RefFrame::DirectionName.begin();
    for_each(RefFrame::Directions.begin(), RefFrame::Directions.end(), [&itName, &intersections](auto &itDir) {
      cout << "Int with dir " << *itName << " : ";
      intersections[itDir].Dump();
      itName++;
    });
  }

  // Check if intercept are included in planes
  int Nint = 0;
  vector<Point> IntPoints;
  vector<RefFrame::Direction> IntInfos;
  for_each(RefFrame::Directions.begin(), RefFrame::Directions.end(), [&](auto &itDir) {
    if (Nint < 2 && this->_CheckIfPointIsInCaloSurface(intersections[itDir], itDir)) {
      Nint++;
      IntPoints.push_back(intersections[itDir]);
      IntInfos.push_back(itDir);
    }
  });

  if (DEBUG) {
    cout << "Interaction: ";
    for (auto itInfo : IntInfos)
      cout << (int)itInfo << " ";
    cout << endl;
    for (auto itPoint : IntPoints)
      itPoint.Dump();
    cout << endl;
  }

  if (Nint != 2) {
    if (Nint > 0) {
      // COUT(WARNING) << "N. of intersection are: " << Nint << "instead of 2, can be due to a numerical approximation
      // (~ 1/10^6 events)" << ENDL;
    }
    _trackInfoCalo->Reset();
    return false; // no hit in the calo surfaces
  }

  // check where is the first hit using the distance between the startingPoint and the hit surfaces.
  float distFirst = _PntDistance(IntPoints[0], startingPoint);
  float distSecond = _PntDistance(IntPoints[1], startingPoint);
  Point tmpIntPoint;
  RefFrame::Direction tmpIntInfo;
  if (distFirst > distSecond) { // the first point is not the entrance point.
    tmpIntPoint = IntPoints[0];
    IntPoints[0] = IntPoints[1];
    IntPoints[1] = tmpIntPoint;
    tmpIntInfo = IntInfos[0];
    IntInfos[0] = IntInfos[1];
    IntInfos[1] = tmpIntInfo;
  }

  float trackLengthCm = _PntDistance(IntPoints[0], IntPoints[1]);
  _trackInfoCalo->entrance = IntPoints[0];
  _trackInfoCalo->exit = IntPoints[1];
  _trackInfoCalo->entrancePlane = IntInfos[0];
  _trackInfoCalo->exitPlane = IntInfos[1];
  _trackInfoCalo->trackLengthCaloCm = trackLengthCm;
  _trackInfoCalo->trackLengthLYSOCm = trackLengthCm * _meanVolumeActiveFraction;
  _trackInfoCalo->trackLengthCaloX0 = _trackInfoCalo->trackLengthLYSOCm / _LYSO_X0;
  _trackInfoCalo->trackLengthLYSOX0 = _trackInfoCalo->trackLengthCaloX0;

  return true;
}

}
*/
 // namespace Herd
