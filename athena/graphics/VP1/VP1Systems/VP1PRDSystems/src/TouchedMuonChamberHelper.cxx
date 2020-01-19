/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TouchedMuonChamberHelper          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/TouchedMuonChamberHelper.h"
#include <QtCore/QTimer>
#include <map>
#include <set>
#include <iostream>

//____________________________________________________________________
class TouchedMuonChamberHelper::Imp {
public:
  TouchedMuonChamberHelper * theclass;
  bool updatesnotcheduled;//true if a timer has already been fired to check status after
  inline void ensureScheduleUpdate();
  std::set<GeoPVConstLink> lastEmittedSet;
  std::map<GeoPVConstLink,unsigned> pv2count;//For keeping track of the number
		                             //of shown objects's within a given
                                             //chamber at any given time.
  std::set<GeoPVConstLink> touchedByTracksSet; // Filled by signal emitted by TrackSystem
};


//____________________________________________________________________
TouchedMuonChamberHelper::TouchedMuonChamberHelper( QObject * parent )
  : QObject(parent), d(new Imp)
{
  d->theclass = this;
  d->updatesnotcheduled = true;
}

//____________________________________________________________________
TouchedMuonChamberHelper::~TouchedMuonChamberHelper()
{
  delete d;
}

//____________________________________________________________________
inline void TouchedMuonChamberHelper::Imp::ensureScheduleUpdate()
{
  if (updatesnotcheduled) {
    updatesnotcheduled = false;
    QTimer::singleShot(0, theclass, SLOT(checkForChangeInTouchedChambers()));
  }
}

//____________________________________________________________________
void TouchedMuonChamberHelper::incrementNumberOfObjectsForPV(const GeoPVConstLink& chamberPV)
{
  std::map<GeoPVConstLink,unsigned>::iterator it(d->pv2count.find(chamberPV));
  if (it==d->pv2count.end()) {
    d->pv2count[chamberPV] = 1;
    d->ensureScheduleUpdate();//First object in chamber
    return;
  }
  ++(it->second);
  if (d->updatesnotcheduled&&it->second==1)
    d->ensureScheduleUpdate();//First object in chamber
}

//____________________________________________________________________
void TouchedMuonChamberHelper::decrementNumberOfObjectsForPV(const GeoPVConstLink& chamberPV)
{
  std::map<GeoPVConstLink,unsigned>::iterator it(d->pv2count.find(chamberPV));
  if (it==d->pv2count.end()||it->second==0) {
    std::cout<<" TouchedMuonChamberHelper::decrementNumberOfObjectsForPV Error: Asked to decrement to negative values!"<<std::endl;
    return;
  }
  --(it->second);
  if (d->updatesnotcheduled&&it->second==0)
    d->ensureScheduleUpdate();//Last object in chamber
}

//____________________________________________________________________
bool TouchedMuonChamberHelper::isTouchedByTrack(const GeoPVConstLink& chamberPV)
{
  return (d->touchedByTracksSet.find(chamberPV)!=d->touchedByTracksSet.end());
}

//____________________________________________________________________
void TouchedMuonChamberHelper::checkForChangeInTouchedChambers()
{
  d->updatesnotcheduled = true;

  std::set<GeoPVConstLink> touchedchambers;
  std::map<GeoPVConstLink,unsigned>::const_iterator it(d->pv2count.begin()),itE(d->pv2count.end());
  for (;it!=itE;++it) {
    if (it->second>0)
      touchedchambers.insert(it->first);
  }

  if (d->lastEmittedSet!=touchedchambers) {
    d->lastEmittedSet = touchedchambers;
    emit touchedMuonChambersChanged(touchedchambers);
  }
}

void TouchedMuonChamberHelper::updateTouchedByTracks(const std::set<GeoPVConstLink>& set){
  // std::cout<<"TouchedMuonChamberHelper::updateTouchedByTracks - set is this big: "<<set.size()<<std::endl;
  d->touchedByTracksSet=set;
  emit muonChambersTouchedByTracksChanged();
}

//____________________________________________________________________
void TouchedMuonChamberHelper::eraseEventData()
{
  d->pv2count.clear();
  d->touchedByTracksSet.clear();
  checkForChangeInTouchedChambers();
}
