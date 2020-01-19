/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_MDT_H
#define PRDCOLLHANDLE_MDT_H

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1Base/VP1Interval.h"

class PRDCollHandle_MDT : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_MDT(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_MDT();

  bool highLightMasked() { return m_highLightMasked; }
  int highLightADCBelow() { return m_highLightADCBelow; }
  enum PROJECTION { NONE, TOTUBES, TOCHAMBERS };
  PROJECTION projection() const { return m_projection; }


public slots:
  void setMinNHitsPerStation(unsigned);
  void setAllowedADCValues(VP1Interval);
  void setExcludeMaskedHits(bool);
  void setStatus(QString);
  void setHighLightByMask(bool);
  void setHighLightByUpperADCBound(int);
  void setEnableProjections( bool );
  void setAppropriateProjection( int );//0: No projections, 1: Project to end of tubes, 2: Project to end of chamber volume.
  void setLimitToActiveChambers(bool);
  void muonChambersTouchedByTracksChanged(void);//!< Inform this handle that it might need to recheck cuts 

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);

  virtual DETAIL defaultDetailLevel() const { return DETAILED; }

  bool cut(PRDHandleBase*);

  void eraseEventDataSpecific();
  void postLoadInitialisation();
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 600.0f*600.0f; }
  QColor defaultColor() const;

private:

  class Imp;
  //  friend class Imp;
  Imp * d;

  //Here for inlines:
  bool m_highLightMasked;
  int m_highLightADCBelow;
  PROJECTION m_projection;
};

#endif
