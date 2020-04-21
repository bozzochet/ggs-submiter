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
      _ZCaloHeight(73.2), _phiXY(atan2(-(-_XSideBig + _XSideSmall), (-_YSideSmall + _YSideBig))),alpha(1.),checkext{true},checkint{false},filterenable{true}
      //_meanActiveFractionZview(0.8606557), _meanActiveFractionXview(0.7974684), _meanActiveFractionYview(0.8606557),
      //_meanVolumeActiveFraction(0.569861492), _LYSO_X0(1.1) 
      {
  const std::string routineName = GetName() + "::CaloGeomFidVolumeAlgo";

  DeclareConsumedObject("mcTruth", ObjectCategory::EVENT, "evStore");

  DefineParameter("alpha", alpha);
  DefineParameter("filterenable", filterenable);
  DefineParameter("checkext", checkext);
  DefineParameter("checkint", checkint);



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

  shrink = 0; if(checkint) shrink=alpha*cubeside;

   // Top and bottom planes                                                                                                                                                                                                                  
  _Planes[RefFrame::Direction::Zpos] = Plane(Point(0., 0., _ZCaloCenter + _ZCaloHeight / 2. - shrink), M_PI, 0);
  _Planes[RefFrame::Direction::Zneg] = Plane(Point(0., 0., _ZCaloCenter - _ZCaloHeight / 2. + shrink), 0,    0);

  // planes parallel to X and Y axes                                                                                                                                                                                                        
  _Planes[RefFrame::Direction::Xneg] = Plane(Point(-_XSideBig / 2. + shrink, 0., _ZCaloCenter), M_PI / 2., M_PI);
  _Planes[RefFrame::Direction::Xpos] = Plane(Point(+_XSideBig / 2. - shrink, 0., _ZCaloCenter), M_PI / 2., 0);
  _Planes[RefFrame::Direction::Yneg] = Plane(Point(0., -_YSideBig / 2. + shrink, _ZCaloCenter), M_PI / 2., M_PI * 3. / 2.);
  _Planes[RefFrame::Direction::Ypos] = Plane(Point(0., +_YSideBig / 2. - shrink, _ZCaloCenter), M_PI / 2., M_PI / 2.);

  // inclined planes                                                                                                                                                                                                                        
  _Planes[RefFrame::Direction::XnegYneg] =
      Plane(Point((-_XSideBig / 2 - _XSideSmall / 2.) / 2. + shrink, (-_YSideBig / 2 - _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., M_PI + _phiXY);
  _Planes[RefFrame::Direction::XposYneg] =
      Plane(Point((_XSideBig / 2 + _XSideSmall / 2.) / 2. - shrink, (-_YSideBig / 2 - _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., 2 * M_PI - _phiXY);
  _Planes[RefFrame::Direction::XnegYpos] =
      Plane(Point((-_XSideBig / 2 - _XSideSmall / 2.) / 2. + shrink, (_YSideBig / 2 + _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., M_PI - _phiXY);
  _Planes[RefFrame::Direction::XposYpos] =
      Plane(Point((_XSideBig / 2 + _XSideSmall / 2.) / 2. - shrink, (_YSideBig / 2 + _YSideSmall / 2.) / 2., _ZCaloCenter),
            M_PI / 2., _phiXY);

    poct[0] = _Planes[RefFrame::Direction::Ypos].Intersection(Line(Point(+_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct0::"<<poct[0][RefFrame::Coo::X]<<"::"<<poct[0][RefFrame::Coo::Y]<<"::"<<poct[0][RefFrame::Coo::Z]<<ENDL;
    poct[1] = _Planes[RefFrame::Direction::Xpos].Intersection(Line(Point(0,_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct1::"<<poct[1][RefFrame::Coo::X]<<"::"<<poct[1][RefFrame::Coo::Y]<<"::"<<poct[1][RefFrame::Coo::Z]<<ENDL;
    double phiXposYpos = atan2( poct[0][RefFrame::Coo::Y]-poct[1][RefFrame::Coo::Y], poct[0][RefFrame::Coo::X]-poct[1][RefFrame::Coo::X]);
    COUT(INFO)<<"phiXposYpos::"<<phiXposYpos<<ENDL;
    _Planes[RefFrame::Direction::XposYpos] = Plane((poct[1]+poct[0])/2, M_PI/2, phiXposYpos+M_PI/2);
    Point poct00 = _Planes[RefFrame::Direction::XposYpos].Intersection(Line(Point(+_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct00::"<<poct00[RefFrame::Coo::X]<<"::"<<poct00[RefFrame::Coo::Y]<<"::"<<poct00[RefFrame::Coo::Z]<<ENDL;
    Point poct10 = _Planes[RefFrame::Direction::XposYpos].Intersection(Line(Point(0,_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct10::"<<poct10[RefFrame::Coo::X]<<"::"<<poct10[RefFrame::Coo::Y]<<"::"<<poct10[RefFrame::Coo::Z]<<ENDL;
    
    poct[2] = _Planes[RefFrame::Direction::Xpos].Intersection(Line(Point(0,-_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct2::"<<poct[2][RefFrame::Coo::X]<<"::"<<poct[2][RefFrame::Coo::Y]<<"::"<<poct[2][RefFrame::Coo::Z]<<ENDL;
    poct[3] = _Planes[RefFrame::Direction::Yneg].Intersection(Line(Point(+_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct3::"<<poct[3][RefFrame::Coo::X]<<"::"<<poct[3][RefFrame::Coo::Y]<<"::"<<poct[3][RefFrame::Coo::Z]<<ENDL;
    double phiXposYneg = atan2( poct[3][RefFrame::Coo::Y]-poct[2][RefFrame::Coo::Y], poct[3][RefFrame::Coo::X]-poct[2][RefFrame::Coo::X]);
    COUT(INFO)<<"phiXposYneg::"<<phiXposYneg<<ENDL;
    _Planes[RefFrame::Direction::XposYneg] = Plane((poct[3]+poct[2])/2, M_PI/2, phiXposYneg+M_PI/2);
    Point poct20 = _Planes[RefFrame::Direction::XposYneg].Intersection(Line(Point(0,-_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct20::"<<poct20[RefFrame::Coo::X]<<"::"<<poct20[RefFrame::Coo::Y]<<"::"<<poct20[RefFrame::Coo::Z]<<ENDL;
    Point poct30 = _Planes[RefFrame::Direction::XposYneg].Intersection(Line(Point(+_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct30::"<<poct30[RefFrame::Coo::X]<<"::"<<poct30[RefFrame::Coo::Y]<<"::"<<poct30[RefFrame::Coo::Z]<<ENDL;

    poct[4] = _Planes[RefFrame::Direction::Yneg].Intersection(Line(Point(-_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct4::"<<poct[4][RefFrame::Coo::X]<<"::"<<poct[4][RefFrame::Coo::Y]<<"::"<<poct[4][RefFrame::Coo::Z]<<ENDL;
    poct[5] = _Planes[RefFrame::Direction::Xneg].Intersection(Line(Point(0,-_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct5::"<<poct[5][RefFrame::Coo::X]<<"::"<<poct[5][RefFrame::Coo::Y]<<"::"<<poct[5][RefFrame::Coo::Z]<<ENDL;
    double phiXnegYneg = atan2( poct[5][RefFrame::Coo::Y]-poct[4][RefFrame::Coo::Y], poct[5][RefFrame::Coo::X]-poct[4][RefFrame::Coo::X]);
    COUT(INFO)<<"phiXnegYneg::"<<phiXnegYneg<<ENDL;
    _Planes[RefFrame::Direction::XnegYneg] = Plane((poct[5]+poct[4])/2, M_PI/2, phiXnegYneg+M_PI/2);
    Point poct40 = _Planes[RefFrame::Direction::XnegYneg].Intersection(Line(Point(-_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct40::"<<poct40[RefFrame::Coo::X]<<"::"<<poct40[RefFrame::Coo::Y]<<"::"<<poct40[RefFrame::Coo::Z]<<ENDL;
    Point poct50 = _Planes[RefFrame::Direction::XnegYneg].Intersection(Line(Point(0,-_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct50::"<<poct50[RefFrame::Coo::X]<<"::"<<poct50[RefFrame::Coo::Y]<<"::"<<poct50[RefFrame::Coo::Z]<<ENDL;

    poct[6] = _Planes[RefFrame::Direction::Xneg].Intersection(Line(Point(0,+_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct6::"<<poct[6][RefFrame::Coo::X]<<"::"<<poct[6][RefFrame::Coo::Y]<<"::"<<poct[6][RefFrame::Coo::Z]<<ENDL;
    poct[7] = _Planes[RefFrame::Direction::Ypos].Intersection(Line(Point(-_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct7::"<<poct[7][RefFrame::Coo::X]<<"::"<<poct[7][RefFrame::Coo::Y]<<"::"<<poct[7][RefFrame::Coo::Z]<<ENDL;
    double phiXnegYpos = atan2( poct[7][RefFrame::Coo::Y]-poct[6][RefFrame::Coo::Y], poct[7][RefFrame::Coo::X]-poct[6][RefFrame::Coo::X]);
    COUT(INFO)<<"phiXnegYpos::"<<phiXnegYpos<<ENDL;
    _Planes[RefFrame::Direction::XnegYpos] = Plane((poct[7]+poct[6])/2, M_PI/2, phiXnegYpos+M_PI/2);
    Point poct60 = _Planes[RefFrame::Direction::XnegYpos].Intersection(Line(Point(0,+_YSideSmall/2.,0),M_PI/2,0));
    COUT(INFO)<<"poct60::"<<poct60[RefFrame::Coo::X]<<"::"<<poct60[RefFrame::Coo::Y]<<"::"<<poct60[RefFrame::Coo::Z]<<ENDL;
    Point poct70 = _Planes[RefFrame::Direction::XnegYpos].Intersection(Line(Point(-_XSideSmall/2.,0,0),M_PI/2,M_PI/2));
    COUT(INFO)<<"poct70::"<<poct70[RefFrame::Coo::X]<<"::"<<poct70[RefFrame::Coo::Y]<<"::"<<poct70[RefFrame::Coo::Z]<<ENDL;

   mXNEGYNEG = tan(_Planes[RefFrame::Direction::XnegYneg].Azimuth() + M_PI / 2.);
   qXNEGYNEG = _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::Y] - mXNEGYNEG * _Planes[RefFrame::Direction::XnegYneg].Origin()[RefFrame::Coo::X];
   mXPOSYNEG = tan(_Planes[RefFrame::Direction::XposYneg].Azimuth() + M_PI / 2.);
   qXPOSYNEG = _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::Y] - mXPOSYNEG * _Planes[RefFrame::Direction::XposYneg].Origin()[RefFrame::Coo::X];
   mXNEGYPOS = tan(_Planes[RefFrame::Direction::XnegYpos].Azimuth() + M_PI / 2.);
   qXNEGYPOS = _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::Y] - mXNEGYPOS * _Planes[RefFrame::Direction::XnegYpos].Origin()[RefFrame::Coo::X];
   mXPOSYPOS = tan(_Planes[RefFrame::Direction::XposYpos].Azimuth() + M_PI / 2.);
   qXPOSYPOS = _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::Y] - mXPOSYPOS * _Planes[RefFrame::Direction::XposYpos].Origin()[RefFrame::Coo::X];
   COUT(INFO)<<"mXnegYneg::"<<mXNEGYNEG<<ENDL;
   COUT(INFO)<<"qXnegYneg::"<<qXNEGYNEG<<ENDL;
   COUT(INFO)<<"mXposYneg::"<<mXPOSYNEG<<ENDL;
   COUT(INFO)<<"qXposYneg::"<<qXPOSYNEG<<ENDL;
   COUT(INFO)<<"mXnegYpos::"<<mXNEGYPOS<<ENDL;
   COUT(INFO)<<"qXnegYpos::"<<qXNEGYPOS<<ENDL;
   COUT(INFO)<<"mXposYpos::"<<mXPOSYPOS<<ENDL;
   COUT(INFO)<<"qXposYpos::"<<qXPOSYPOS<<ENDL;

  return true;
}

bool CaloGeomFidVolumeAlgo::Process() {

    if(checkext) return CheckExt();
    if(checkint) return CheckInt();
    return true;
}

bool CaloGeomFidVolumeAlgo::CheckExt() {

  const std::string routineName = GetName() + "::Process";

  //Add the ProcessStore object for this event to the event data store
  //_processstore->Reset();
  _evStore->AddObject("caloGeomFidVolumeStore",_processstore);

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);

  auto mcTruth = _evStore->GetObject<MCTruth>("mcTruth");
  if (!mcTruth) {COUT(ERROR) << "mcTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;return false;}
  const Momentum &Mom = mcTruth->primaries[0].initialMomentum;
  const Point &Pos    = mcTruth->primaries[0].initialPosition;
  Line track(Pos, Mom);

  //Build additional planes
  Plane PlaneX6 (Point( -_XSideSmall/2., 0., _ZCaloCenter), M_PI / 2., M_PI);          //verify these angles...
  Plane PlaneX14(Point( +_XSideSmall/2., 0., _ZCaloCenter), M_PI / 2., 0);             //verify these angles...
  Plane PlaneY6 (Point(0., -_YSideSmall/2.,  _ZCaloCenter), M_PI / 2., M_PI * 3. / 2.);  //verify these angles...
  Plane PlaneY14(Point(0., +_YSideSmall/2.,  _ZCaloCenter), M_PI / 2., M_PI / 2.);       //verify these angles...

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
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                    &&   
      intXpos[RefFrame::Coo::Z] < 0.                              ) { nintXpos++; FillCoo(intXpos,_processstore->calofidvolxposEntry,nintXpos-1); }
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&   
      intZpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { nintXpos++; FillCoo(intZpos,_processstore->calofidvolxposEntry,nintXpos-1); }
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&
      intZneg[RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&   
      intZneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { nintXpos++; FillCoo(intZneg,_processstore->calofidvolxposEntry,nintXpos-1); }
  if( intY6  [RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intY6  [RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&
      intY6  [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intY6  [RefFrame::Coo::Z] < 0.                              ) { nintXpos++; FillCoo(intY6,_processstore->calofidvolxposEntry,nintXpos-1); }
  if( intY14 [RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intY14 [RefFrame::Coo::X] > +_XSideBig/2.  - alpha*cubeside  &&
      intY14 [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intY14 [RefFrame::Coo::Z] < 0.                              ) { nintXpos++; FillCoo(intY14,_processstore->calofidvolxposEntry,nintXpos-1); }
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
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                    &&   
      intXneg[RefFrame::Coo::Z] < 0.                              ) { nintXneg++; FillCoo(intXneg,_processstore->calofidvolxnegEntry,nintXneg-1); }
  if( intZpos[RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intZpos[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { nintXneg++; FillCoo(intZpos,_processstore->calofidvolxnegEntry,nintXneg-1); }
  if( intZneg[RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intZneg[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::Y] < +_YSideSmall/2.                    ) { nintXneg++; FillCoo(intZneg,_processstore->calofidvolxnegEntry,nintXneg-1); }
  if( intY6  [RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intY6  [RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intY6  [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intY6  [RefFrame::Coo::Z] < 0.                              ) {  nintXneg++; FillCoo(intY6,_processstore->calofidvolxnegEntry,nintXneg-1); }
  if( intY14 [RefFrame::Coo::X] < -_XSideBig/2. +  alpha*cubeside  &&   
      intY14 [RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intY14 [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intY14 [RefFrame::Coo::Z] < 0.                              ) { nintXneg++; FillCoo(intY14,_processstore->calofidvolxnegEntry,nintXneg-1); }
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
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                    &&   
      intYpos[RefFrame::Coo::Z] < 0.                              )  { nintYpos++; FillCoo(intYpos,_processstore->calofidvolyposEntry,nintYpos-1); }
  if( intZpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside    ) { nintYpos++; FillCoo(intZpos,_processstore->calofidvolyposEntry,nintYpos-1); }
  if( intZneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside    ) { nintYpos++; FillCoo(intZneg,_processstore->calofidvolyposEntry,nintYpos-1); }
  if( intX6  [RefFrame::Coo::Y] < +_YSideBig/2.                    &&   
      intX6  [RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside  &&
      intX6  [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intX6  [RefFrame::Coo::Z] < 0.                              ) { nintYpos++; FillCoo(intX6,_processstore->calofidvolyposEntry,nintYpos-1); }
  if( intX14 [RefFrame::Coo::Y] < +_YSideBig/2.                    &&  
      intX14 [RefFrame::Coo::Y] > +_YSideBig/2. -  alpha*cubeside  &&
      intX14 [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intX14 [RefFrame::Coo::Z] < 0.                              ) { nintYpos++; FillCoo(intX14,_processstore->calofidvolyposEntry,nintYpos-1); }
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
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                    &&   
      intYneg[RefFrame::Coo::Z] < 0.                              ) { nintYneg++; FillCoo(intYneg,_processstore->calofidvolynegEntry,nintYneg-1); }
  if( intZpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside    ) { nintYneg++; FillCoo(intZpos,_processstore->calofidvolynegEntry,nintYneg-1); }
  if( intZneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&   
      intZneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&     
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside    ) { nintYneg++; FillCoo(intZneg,_processstore->calofidvolynegEntry,nintYneg-1); }
  if( intX6  [RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intX6  [RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside  &&   
      intX6  [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intX6  [RefFrame::Coo::Z] < 0.                              ) { nintYneg++; FillCoo(intX6,_processstore->calofidvolynegEntry,nintYneg-1); }
  if( intX14 [RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intX14 [RefFrame::Coo::Y] < -_YSideBig/2. +  alpha*cubeside  &&    
      intX14 [RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intX14 [RefFrame::Coo::Z] < 0.                              ) { nintYneg++; FillCoo(intX14,_processstore->calofidvolynegEntry,nintYneg-1); }
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
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                    &&
      intZpos[RefFrame::Coo::Y] < mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZpos[RefFrame::Coo::Y] < mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZpos[RefFrame::Coo::Y] > mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG     ) { nintZpos++; FillCoo(intZpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::Z] < 0                                &&
      intYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) { nintZpos++; FillCoo(intYneg,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::Z] < 0                                &&
      intYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) { nintZpos++; FillCoo(intYpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Z] < 0                                &&
      intXneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) { nintZpos++; FillCoo(intXneg,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Z] < 0                                &&
      intXpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                    ) { nintZpos++; FillCoo(intXpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] > -_YSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] < -_YSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::Z] < 0                            &&
      intXposYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) { nintZpos++; FillCoo(intXposYneg,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Y] > -_YSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::Y] < -_YSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Z] < 0                            &&
      intXnegYneg[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) { nintZpos++; FillCoo(intXnegYneg,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] > +_YSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] < +_YSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::Z] < 0                            &&
      intXnegYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) { nintZpos++; FillCoo(intXnegYpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
  if( intXposYpos[RefFrame::Coo::X] > +_XSideSmall/2.                &&
      intXposYpos[RefFrame::Coo::X] < +_XSideBig/2.              &&
      intXposYpos[RefFrame::Coo::Y] > +_YSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] < +_YSideBig/2.                &&
      intXposYpos[RefFrame::Coo::Z] < 0                            &&
      intXposYpos[RefFrame::Coo::Z] > 0 - alpha*cubeside                                ) { nintZpos++; FillCoo(intXposYpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
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
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::X] > -_XSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                    &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                    &&
      intZneg[RefFrame::Coo::Y] < mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZneg[RefFrame::Coo::Y] < mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZneg[RefFrame::Coo::Y] > mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG     ) { nintZneg++; FillCoo(intZneg,_processstore->calofidvolznegEntry,nintZneg-1); }      
  if( intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                        ) { nintZneg++; FillCoo(intYneg,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                  &&
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) { nintZneg++; FillCoo(intYpos,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intXneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) { nintZneg++; FillCoo(intXneg,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Y] < +_YSideSmall/2.                  &&
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                    &&
      intXpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                      ) { nintZneg++; FillCoo(intXpos,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] > -_YSideBig/2.                &&
      intXposYneg[RefFrame::Coo::Y] < -_YSideSmall/2.              &&
      intXposYneg[RefFrame::Coo::Z] > -_ZCaloHeight                &&
      intXposYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) { nintZneg++; FillCoo(intXposYneg,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Y] > -_YSideBig/2.                &&
      intXnegYneg[RefFrame::Coo::Y] < -_YSideSmall/2.              &&
      intXnegYneg[RefFrame::Coo::Z] > -_ZCaloHeight                &&
      intXnegYneg[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) { nintZneg++; FillCoo(intXnegYneg,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] > +_YSideSmall/2.              &&
      intXnegYpos[RefFrame::Coo::Y] < +_YSideBig/2.                &&
      intXnegYpos[RefFrame::Coo::Z] > -_ZCaloHeight                &&
      intXnegYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) { nintZneg++; FillCoo(intXnegYpos,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( intXposYpos[RefFrame::Coo::X] > +_XSideSmall/2.                &&
      intXposYpos[RefFrame::Coo::X] < +_XSideBig/2.              &&
      intXposYpos[RefFrame::Coo::Y] > +_YSideSmall/2.              &&
      intXposYpos[RefFrame::Coo::Y] < +_YSideBig/2.                &&
      intXposYpos[RefFrame::Coo::Z] > -_ZCaloHeight                &&
      intXposYpos[RefFrame::Coo::Z] < -_ZCaloHeight + alpha*cubeside                    ) { nintZneg++; FillCoo(intXposYpos,_processstore->calofidvolznegEntry,nintZneg-1); }   
  if( nintZneg >  2) {COUT(ERROR)<<"intZneg>2"<<ENDL;}// return false;}     
  if( nintZneg == 2) {
    _processstore->calofidvolzneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolzneg=0;}
    
//CORNER CAP CHECK: XNEGYNEG
  int nintXnegYneg=0; 
  if( intXnegYneg[RefFrame::Coo::X] > -_XSideBig/2.                 &&   
      intXnegYneg[RefFrame::Coo::X] < -_XSideSmall/2.               &&   
      intXnegYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXnegYneg[RefFrame::Coo::Z] < 0.                              ) { nintXnegYneg++; FillCoo(intXnegYneg,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
  if( intXneg[RefFrame::Coo::Y] > -_YSideSmall/2.                   &&   
      intXneg[RefFrame::Coo::Y] < -_YSideSmall/2. + alpha*cubeside  &&     
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXneg[RefFrame::Coo::Z] < 0.                                  ) { nintXnegYneg++; FillCoo(intXneg,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
  if( intYneg[RefFrame::Coo::X] > -_XSideSmall/2.                     &&   
      intYneg[RefFrame::Coo::X] < -_XSideSmall/2. +  alpha*cubeside   &&   
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYneg[RefFrame::Coo::Z] < 0.                                 )  { nintXnegYneg++; FillCoo(intYneg,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
  if( intZpos[RefFrame::Coo::X] > -_XSideBig/2.                     &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                     &&
      intZpos[RefFrame::Coo::Y] > mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG  &&
      intZpos[RefFrame::Coo::Y] < mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG  + alpha*cubeside) { nintXnegYneg++; FillCoo(intZpos,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
  if( intZneg[RefFrame::Coo::X] > -_XSideBig/2.                     &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                     &&
      intZneg[RefFrame::Coo::Y] > mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG  &&
      intZneg[RefFrame::Coo::Y] < mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG  + alpha*cubeside) { nintXnegYneg++; FillCoo(intZneg,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
  if( nintXnegYneg >  2) {COUT(ERROR)<<"nintXegYneg>2"<<ENDL;}// return false;}    
  if( nintXnegYneg == 2) {
    _processstore->calofidvolxnegyneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxnegyneg=0;}

//CORNER CAP CHECK: XPOSYNEG
  int nintXposYneg=0; 
  if( intXposYneg[RefFrame::Coo::X] > +_XSideSmall/2.               &&   
      intXposYneg[RefFrame::Coo::X] < +_XSideBig/2.                 &&   
      intXposYneg[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXposYneg[RefFrame::Coo::Z] < 0.                              ) { nintXposYneg++; FillCoo(intXposYneg,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
  if( intXpos[RefFrame::Coo::Y] > -_YSideSmall/2.                   &&   
      intXpos[RefFrame::Coo::Y] < -_YSideSmall/2. + alpha*cubeside  &&     
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXpos[RefFrame::Coo::Z] < 0.                                  ) { nintXposYneg++; FillCoo(intXpos,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
  if( intYneg[RefFrame::Coo::X] < +_XSideSmall/2.                   &&   
      intYneg[RefFrame::Coo::X] > +_XSideSmall/2  - alpha*cubeside  &&     
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYneg[RefFrame::Coo::Z] < 0.                                  ) { nintXposYneg++; FillCoo(intYneg,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                     &&   
      intZpos[RefFrame::Coo::Y] > -_YSideBig/2.                     &&
      intZpos[RefFrame::Coo::Y] > mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZpos[RefFrame::Coo::Y] < mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  + alpha*cubeside) { nintXposYneg++; FillCoo(intZpos,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                     &&   
      intZneg[RefFrame::Coo::Y] > -_YSideBig/2.                     &&
      intZneg[RefFrame::Coo::Y] > mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZneg[RefFrame::Coo::Y] < mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG + alpha*cubeside) { nintXposYneg++; FillCoo(intZneg,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
  if( nintXposYneg >  2) {COUT(ERROR)<<"nintXposYneg>2"<<ENDL; }//return false;}    
  if( nintXposYneg == 2) {
    _processstore->calofidvolxposyneg=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxposyneg=0;}  

  

//CORNER CAP CHECK: XNEGYPOS
  int nintXnegYpos=0; 
  if( intXnegYpos[RefFrame::Coo::X] > -_XSideBig/2.                 &&   
      intXnegYpos[RefFrame::Coo::X] < -_XSideSmall/2.               &&   
      intXnegYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXnegYpos[RefFrame::Coo::Z] < 0.                              ) { nintXnegYpos++; FillCoo(intXnegYpos,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); } 
  if( intXneg[RefFrame::Coo::Y] < +_YSideSmall/2.                   &&   
      intXneg[RefFrame::Coo::Y] > +_YSideSmall/2. - alpha*cubeside  &&
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXneg[RefFrame::Coo::Z] < 0.                                  ) { nintXnegYpos++; FillCoo(intXneg,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); }  
  if( intYpos[RefFrame::Coo::X] > -_XSideSmall/2.                   &&   
      intYpos[RefFrame::Coo::X] < -_XSideSmall/2  + alpha*cubeside  &&     
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYpos[RefFrame::Coo::Z] < 0.                                  ) { nintXnegYpos++; FillCoo(intYpos,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); } 
  if( intZpos[RefFrame::Coo::X] > -_XSideBig/2.                     &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                     &&
      intZpos[RefFrame::Coo::Y] < mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  - alpha*cubeside) { nintXnegYpos++; FillCoo(intZpos,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); } 
  if( intZneg[RefFrame::Coo::X] > -_XSideBig/2.                     &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                     &&
      intZneg[RefFrame::Coo::Y] < mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS - alpha*cubeside) { nintXnegYpos++; FillCoo(intZneg,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); }  
  if( nintXnegYpos >  2) {COUT(ERROR)<<"nintXnegYpos>2"<<ENDL; }//return false;}    
  if( nintXnegYpos == 2) {
    _processstore->calofidvolxnegypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxnegypos=0;}

//CORNER CAP CHECK: XPOSYPOS
  int nintXposYpos=0; 
  if( intXposYpos[RefFrame::Coo::X] > +_XSideSmall/2.               &&   
      intXposYpos[RefFrame::Coo::X] < +_XSideBig/2.                 &&   
      intXposYpos[RefFrame::Coo::Z] > -_ZCaloHeight                 &&   
      intXposYpos[RefFrame::Coo::Z] < 0.                              ) { nintXposYpos++; FillCoo(intXposYpos,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
  if( intXpos[RefFrame::Coo::Y] < +_YSideSmall/2.                   &&   
      intXpos[RefFrame::Coo::Y] > +_YSideSmall/2. - alpha*cubeside  &&     
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intXpos[RefFrame::Coo::Z] < 0.                                  ) { nintXposYpos++; FillCoo(intXpos,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
  if( intYpos[RefFrame::Coo::X] < +_XSideSmall/2.                   &&   
      intYpos[RefFrame::Coo::X] > +_XSideSmall/2  - alpha*cubeside  &&     
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight                     &&   
      intYpos[RefFrame::Coo::Z] < 0.                                  ) { nintXposYpos++; FillCoo(intYpos,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
  if( intZpos[RefFrame::Coo::X] < +_XSideBig/2.                     &&   
      intZpos[RefFrame::Coo::Y] < +_YSideBig/2.                     &&
      intZpos[RefFrame::Coo::Y] < mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  - alpha*cubeside) { nintXposYpos++; FillCoo(intZpos,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
  if( intZneg[RefFrame::Coo::X] < +_XSideBig/2.                     &&   
      intZneg[RefFrame::Coo::Y] < +_YSideBig/2.                     &&
      intZneg[RefFrame::Coo::Y] < mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS - alpha*cubeside) { nintXposYpos++; FillCoo(intZneg,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
  if( nintXposYpos >  2) {COUT(ERROR)<<"nintXposYpos>2"<<ENDL;}// return false;}    
  if( nintXposYpos == 2) {
    _processstore->calofidvolxposypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::REJECT);
    }
  else
    {_processstore->calofidvolxposypos=0;} 

  return true;
}

bool CaloGeomFidVolumeAlgo::CheckInt() {
    
  const std::string routineName = GetName() + "::Process";

  //Add the ProcessStore object for this event to the event data store
  //_processstore->Reset();
  _evStore->AddObject("caloGeomFidVolumeStore",_processstore);

  //Set Filter Status
  SetFilterResult(FilterResult::ACCEPT);

  auto mcTruth = _evStore->GetObject<MCTruth>("mcTruth");
  if (!mcTruth) {COUT(ERROR) << "mcTruth not present for event " << GetEventLoopProxy()->GetCurrentEvent() << ENDL;return false;}
  const Momentum &Mom = mcTruth->primaries[0].initialMomentum;
  const Point &Pos    = mcTruth->primaries[0].initialPosition;
  Line track(Pos, Mom);

  //Get Intersection with planes
  Point intXneg = _Planes[RefFrame::Direction::Xneg].Intersection(track);
  Point intXpos = _Planes[RefFrame::Direction::Xpos].Intersection(track);
  Point intYneg = _Planes[RefFrame::Direction::Yneg].Intersection(track);
  Point intYpos = _Planes[RefFrame::Direction::Ypos].Intersection(track);
  Point intXnegYneg = _Planes[RefFrame::Direction::XnegYneg].Intersection(track);
  Point intXnegYpos = _Planes[RefFrame::Direction::XnegYpos].Intersection(track);
  Point intXposYneg = _Planes[RefFrame::Direction::XposYneg].Intersection(track);
  Point intXposYpos = _Planes[RefFrame::Direction::XposYpos].Intersection(track);
  Point intZneg = _Planes[RefFrame::Direction::Zneg].Intersection(track);
  Point intZpos = _Planes[RefFrame::Direction::Zpos].Intersection(track);
  
  _processstore->calofidvolpass=true;
  _processstore->calofidvolalpha=alpha;

  int nint=0;
  //LATERAL CAP CHECK: XPOS
  if( intXpos[RefFrame::Coo::Y] < poct[1][RefFrame::Coo::Y]        &&   
      intXpos[RefFrame::Coo::Y] > poct[2][RefFrame::Coo::Y]        &&   
      intXpos[RefFrame::Coo::Z] > -_ZCaloHeight + alpha*cubeside   &&   
      intXpos[RefFrame::Coo::Z] < 0 - alpha*cubeside                ) { nint++;       COUT(INFO)<<"XPOS"<<ENDL;
}//FillCoo(intXpos,_processstore->calofidvolxposEntry,nintXpos-1); }

  //LATERAL CAP CHECK: XNEG
  if( intXneg[RefFrame::Coo::Y] < poct[6][RefFrame::Coo::Y]        &&   
      intXneg[RefFrame::Coo::Y] > poct[5][RefFrame::Coo::Y]        &&   
      intXneg[RefFrame::Coo::Z] > -_ZCaloHeight + alpha*cubeside   &&   
      intXneg[RefFrame::Coo::Z] < 0 - alpha*cubeside                ) { nint++;       COUT(INFO)<<"XNEG"<<ENDL;
}//FillCoo(intXneg,_processstore->calofidvolxnegEntry,nintXneg-1); }

//LATERAL CAP CHECK: YPOS
  if( intYpos[RefFrame::Coo::X] < poct[0][RefFrame::Coo::X]        &&   
      intYpos[RefFrame::Coo::X] > poct[7][RefFrame::Coo::X]        &&   
      intYpos[RefFrame::Coo::Z] > -_ZCaloHeight + alpha*cubeside   &&   
      intYpos[RefFrame::Coo::Z] < 0 - alpha*cubeside                )  { nint++;       COUT(INFO)<<"YPOS"<<ENDL;
}//FillCoo(intYpos,_processstore->calofidvolyposEntry,nintYpos-1); }
  
//LATERAL CAP CHECK: YNEG
  if( intYneg[RefFrame::Coo::X] < poct[3][RefFrame::Coo::X]        &&   
      intYneg[RefFrame::Coo::X] > poct[4][RefFrame::Coo::X]        &&   
      intYneg[RefFrame::Coo::Z] > -_ZCaloHeight + alpha*cubeside   &&   
      intYneg[RefFrame::Coo::Z] < 0. - alpha*cubeside               ) { nint++;       COUT(INFO)<<"YNEG"<<ENDL;
}//FillCoo(intYneg,_processstore->calofidvolynegEntry,nintYneg-1); }

//TOP CAP CHECK: ZPOS
  // Assume an octagon
  if( intZpos[RefFrame::Coo::X] < poct[1][RefFrame::Coo::X]         && 
      intZpos[RefFrame::Coo::X] > poct[6][RefFrame::Coo::X]         &&  
      intZpos[RefFrame::Coo::Y] > poct[0][RefFrame::Coo::Y]         && 
      intZpos[RefFrame::Coo::Y] < poct[7][RefFrame::Coo::Y]         &&  
      intZpos[RefFrame::Coo::Y] < mXNEGYPOS * intZpos[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZpos[RefFrame::Coo::Y] < mXPOSYPOS * intZpos[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZpos[RefFrame::Coo::Y] > mXPOSYNEG * intZpos[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZpos[RefFrame::Coo::Y] > mXNEGYNEG * intZpos[RefFrame::Coo::X] + qXNEGYNEG     ) { nint++;       COUT(INFO)<<"ZPOS"<<ENDL;
}// FillCoo(intZpos,_processstore->calofidvolzposEntry,nintZpos-1); }    
  //BOTTOM CAP CHECK: ZNEG
  // Assume an octagon
  if( intZneg[RefFrame::Coo::X] < poct[1][RefFrame::Coo::X]         &&   
      intZneg[RefFrame::Coo::X] > poct[6][RefFrame::Coo::X]         && 
      intZneg[RefFrame::Coo::Y] > poct[0][RefFrame::Coo::Y]         && 
      intZneg[RefFrame::Coo::Y] < poct[7][RefFrame::Coo::Y]         &&  
      intZneg[RefFrame::Coo::Y] < mXNEGYPOS * intZneg[RefFrame::Coo::X] + qXNEGYPOS  &&
      intZneg[RefFrame::Coo::Y] < mXPOSYPOS * intZneg[RefFrame::Coo::X] + qXPOSYPOS  &&
      intZneg[RefFrame::Coo::Y] > mXPOSYNEG * intZneg[RefFrame::Coo::X] + qXPOSYNEG  &&
      intZneg[RefFrame::Coo::Y] > mXNEGYNEG * intZneg[RefFrame::Coo::X] + qXNEGYNEG     ) { nint++;       COUT(INFO)<<"ZNEG"<<ENDL;
}// FillCoo(intZneg,_processstore->calofidvolznegEntry,nintZneg-1); }      
    
//CORNER CAP CHECK: XNEGYNEG
  if( intXnegYneg[RefFrame::Coo::X] > poct[5][RefFrame::Coo::X]         &&   
      intXnegYneg[RefFrame::Coo::X] < poct[4][RefFrame::Coo::X]         &&   
      intXnegYneg[RefFrame::Coo::Z] > -_ZCaloHeight   + alpha*cubeside  &&   
      intXnegYneg[RefFrame::Coo::Z] < 0.              - alpha*cubeside   ) { nint++;      COUT(INFO)<<"XNEG-YNEG"<<ENDL;
}// FillCoo(intXnegYneg,_processstore->calofidvolxnegynegEntry,nintXnegYneg-1); }   
 
//CORNER CAP CHECK: XPOSYNEG
  if( intXposYneg[RefFrame::Coo::X] > poct[3][RefFrame::Coo::X]         &&   
      intXposYneg[RefFrame::Coo::X] < poct[2][RefFrame::Coo::X]         &&   
      intXposYneg[RefFrame::Coo::Z] > -_ZCaloHeight   + alpha*cubeside  &&   
      intXposYneg[RefFrame::Coo::Z] < 0.              - alpha*cubeside   ) { nint++;       COUT(INFO)<<"XPOS-YNEG"<<ENDL;
}// FillCoo(intXposYneg,_processstore->calofidvolxposynegEntry,nintXposYneg-1); } 
 
//CORNER CAP CHECK: XNEGYPOS
  if( intXnegYpos[RefFrame::Coo::X] > poct[6][RefFrame::Coo::X]         &&   
      intXnegYpos[RefFrame::Coo::X] < poct[7][RefFrame::Coo::X]         &&   
      intXnegYpos[RefFrame::Coo::Z] > -_ZCaloHeight   + alpha*cubeside  &&   
      intXnegYpos[RefFrame::Coo::Z] < 0.              - alpha*cubeside   ) { nint++;       COUT(INFO)<<"XNEG-YPOS"<<ENDL;
}// FillCoo(intXnegYpos,_processstore->calofidvolxnegyposEntry,nintXnegYpos-1); } 

//CORNER CAP CHECK: XPOSYPOS
  if( intXposYpos[RefFrame::Coo::X] > poct[0][RefFrame::Coo::X]         &&   
      intXposYpos[RefFrame::Coo::X] < poct[1][RefFrame::Coo::X]         &&   
      intXposYpos[RefFrame::Coo::Z] > -_ZCaloHeight   + alpha*cubeside  &&   
      intXposYpos[RefFrame::Coo::Z] < 0.              - alpha*cubeside   ) { nint++;       COUT(INFO)<<"XPOS-YPOS"<<ENDL;
}// FillCoo(intXposYpos,_processstore->calofidvolxposyposEntry,nintXposYpos-1); } 
 
    COUT(INFO)<<nint<<ENDL;

 if( nint > 2) { return false; }

  if( nint == 2) {
    //_processstore->calofidvolxposypos=1; _processstore->calofidvolpass=false;
    SetFilterResult(FilterResult::ACCEPT);
    _processstore->calofidvolpass=false;
    }
  else
    {
    SetFilterResult(FilterResult::REJECT);
    _processstore->calofidvolpass=true;
}

  return true;
}


bool CaloGeomFidVolumeAlgo::Finalize() { return true; }


void CaloGeomFidVolumeAlgo::FillCoo(const Herd::Point p, float coo[2][3], int index) {
coo[index][0] = p[RefFrame::Coo::X];
coo[index][1] = p[RefFrame::Coo::Y];
coo[index][2] = p[RefFrame::Coo::Z];
return;
}

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
    for(int ii=0; ii<2; ii++){
    for(int jj=0; jj<3; jj++){
    calofidvolxposEntry[ii][jj] = -999;
    calofidvolxnegEntry[ii][jj] = -999;
    calofidvolyposEntry[ii][jj] = -999;
    calofidvolynegEntry[ii][jj] = -999;
    calofidvolzposEntry[ii][jj] = -999;
    calofidvolznegEntry[ii][jj] = -999;
    calofidvolxnegynegEntry[ii][jj] = -999;
    calofidvolxposynegEntry[ii][jj] = -999;
    calofidvolxnegyposEntry[ii][jj] = -999;
    calofidvolxposyposEntry[ii][jj] = 999;
    }}
  return true;
}

