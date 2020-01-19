/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleHandle_TrackParticle       //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLEHANDLE_TRACKPARTICLE_H
#define IPARTICLEHANDLE_TRACKPARTICLE_H

#include "IParticleHandleBase.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <QList>

class SoLineSet;

class IParticleHandle_TrackParticle : public IParticleHandleBase {
public:

  IParticleHandle_TrackParticle(IParticleCollHandleBase*, const xAOD::TrackParticle * );
  virtual ~IParticleHandle_TrackParticle();

  virtual bool has3DObjects();
  virtual void clear3DObjects();
  virtual SoNode* nodes();
  void addParameterShapes();
  /// Make the line, but only using information on the track (a spline fit will be done from the parameters)
  void addLine_FromTrackParticle();
  /// Extrapolate the initial track parameters to get the line (a spline fit is done between the extrapolated parameters)
  void addLine_Extrapolated();
  /// Fill d->line with a SoLineSet made from a spline fit between the information in positions and momenta.
  void fillLineFromSplineFit( const std::vector<Amg::Vector3D>& positions, 
  const std::vector<Amg::Vector3D>& momenta);
  void linear(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, float t);

  void bezier(Amg::Vector3D& output, Amg::Vector3D& a, Amg::Vector3D& b, Amg::Vector3D& c, Amg::Vector3D& d, float t);

  virtual QStringList clicked() const;
  virtual Amg::Vector3D momentum() const;
  const xAOD::IParticle& iParticle() const;
  virtual double charge() const ;//!< Returns unknown() in case of trouble.
	
  virtual unsigned summaryValue(xAOD::SummaryType type) const;
  virtual unsigned getNPixelHits()   const;
  virtual unsigned getNSCTHits()     const;
  virtual unsigned getNTRTHits()     const;
  virtual unsigned getNMuonPrecisionLayers() const;
  virtual unsigned getNMuonPrecisionHoleLayers()     const;
  virtual unsigned getNMuonPhiLayers()     const;
  virtual unsigned getNMuonPhiHoleLayers()     const;

  virtual QString type() const { return QString("TrackParticle"); } //!< return very short word with type (maybe link with collection type?)

  /// This returns the information shown about the object in the object browser
  QString shortInfo() const;
  
  void fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) ;
  
  // Some specific stuff used by base collection to draw tracks
  const QList<std::pair<xAOD::ParameterPosition, Amg::Vector3D> >& parametersAndPositions() const;
  
  virtual void dumpToJSON( std::ofstream& ) const ;

private:

  class Imp;
  Imp * d;
  void ensureInitSubSysHitInfo() const;

};

#endif
