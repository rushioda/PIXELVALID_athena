/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackCollHandle_TruthTracks         //
//                                                            //
//  Description: Collection handle for HepMC/simhit based     //
//               truth tracks                                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKCOLLHANDLE_TRUTHTRACKS_H
#define TRACKCOLLHANDLE_TRUTHTRACKS_H

#include "VP1TrackSystems/TrackCollHandleBase.h"

class SimHitHandleBase;

class TrackCollHandle_TruthTracks : public TrackCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  TrackCollHandle_TruthTracks(TrackSysCommonData *,
			      const QString& name);
  virtual ~TrackCollHandle_TruthTracks();

  virtual void setupSettingsFromControllerSpecific(TrackSystemController*);

protected:
  virtual COLOURBY defaultColourBy () const { return COLOUR_BYPID; }
  virtual bool allowColourByPID() const { return true; }
  virtual bool load();
  virtual bool cut(TrackHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(0.42f, 0.96f, 0.16f); }

private slots:
  void setCutFromIROnly(bool);
  void setCutExcludeBarcodeZero(bool);
  void setCutExcludeNeutrals(bool);
  void setShowAscObjs(bool);

private:

  class Imp;
  Imp * d;
  void fixPDGCode(SimHitHandleBase*) const;
};

#endif
