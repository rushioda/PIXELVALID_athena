/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandle_Pixel.h"
#include "VP1PRDSystems/PRDHandle_Pixel.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "InDetPrepRawData/PixelCluster.h"
#include <QtGui/QColor>

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_Pixel::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasPixelGeometry() ? VP1SGContentsHelper(sys).getKeys<InDet::PixelClusterContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_Pixel::Imp {
public:
  PRDCommonFlags::InDetPartsFlags indetpartsflags;
  unsigned minNRDOPerCluster;
};

//____________________________________________________________________
PRDCollHandle_Pixel::PRDCollHandle_Pixel(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::Pixel,cd,key), d(new Imp)
{
  d->indetpartsflags = (PRDCommonFlags::BarrelPositive | PRDCommonFlags::BarrelNegative | PRDCommonFlags::EndCapPositive | PRDCommonFlags::EndCapNegative);
  d->minNRDOPerCluster = 2;
}

//____________________________________________________________________
PRDCollHandle_Pixel::~PRDCollHandle_Pixel()
{
  delete d;
}

//____________________________________________________________________
QColor PRDCollHandle_Pixel::defaultColor() const
{
  return QColor::fromRgbF(1.0, 0.666667, 0.5 );//beige
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_Pixel::addPRD( Trk::PrepRawData * prd )
{
  assert(dynamic_cast<InDet::PixelCluster*>(prd));
  return new PRDHandle_Pixel(this,static_cast<InDet::PixelCluster*>(prd));
}

//____________________________________________________________________
bool PRDCollHandle_Pixel::cut(PRDHandleBase*handlebase)
{
  PRDHandle_Pixel * handle = static_cast<PRDHandle_Pixel*>(handlebase);
  assert(handle);

  if (d->indetpartsflags!=PRDCommonFlags::All) {
    if (handle->isBarrel()) {
      if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::BarrelPositive):(d->indetpartsflags&PRDCommonFlags::BarrelNegative)))
 	return false;
    } else {
      if (!(handle->isPositiveZ()?(d->indetpartsflags&PRDCommonFlags::EndCapPositive):(d->indetpartsflags&PRDCommonFlags::EndCapNegative)))
 	return false;
    }
  }
  if (d->minNRDOPerCluster&&handle->cluster()->rdoList().size()<d->minNRDOPerCluster)
    return false;
  return true;
}

//____________________________________________________________________
void PRDCollHandle_Pixel::setPartsFlags(PRDCommonFlags::InDetPartsFlags flags ) {
  //NB: The code is this method is very similar in PRDCollHandle_Pixel::setPartsFlags, PRDCollHandle_SCT::setPartsFlags,
  //PRDCollHandle_TRT::setPartsFlags and and PRDCollHandle_SpacePoints::setPartsFlags
  //Fixme: base decision to recheck on visibility also!

  if (d->indetpartsflags==flags)
    return;

  bool barrelPosChanged = (d->indetpartsflags&PRDCommonFlags::BarrelPositive)!=(flags&PRDCommonFlags::BarrelPositive);
  bool barrelNegChanged = (d->indetpartsflags&PRDCommonFlags::BarrelNegative)!=(flags&PRDCommonFlags::BarrelNegative);
  bool endcapPosChanged = (d->indetpartsflags&PRDCommonFlags::EndCapPositive)!=(flags&PRDCommonFlags::EndCapPositive);
  bool endcapNegChanged = (d->indetpartsflags&PRDCommonFlags::EndCapNegative)!=(flags&PRDCommonFlags::EndCapNegative);
  bool barrelChanged = (barrelPosChanged || barrelNegChanged);
  bool endcapChanged = (endcapPosChanged || endcapNegChanged);
  d->indetpartsflags=flags;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    PRDHandle_Pixel* handle = static_cast<PRDHandle_Pixel*>(*it);

    if (handle->isBarrel()) {
      if (barrelChanged&&(handle->isPositiveZ()?barrelPosChanged:barrelNegChanged))
 	recheckCutStatus(handle);
    } else {
      if (endcapChanged&&(handle->isPositiveZ()?endcapPosChanged:endcapNegChanged))
 	recheckCutStatus(handle);
    }

  }
  largeChangesEnd();

}

//____________________________________________________________________
void PRDCollHandle_Pixel::setMinNRDOPerCluster(unsigned minnrdo)
{
  if (d->minNRDOPerCluster==minnrdo)
    return;
  bool cut_relaxed =  minnrdo<d->minNRDOPerCluster;
  d->minNRDOPerCluster = minnrdo;
  if (cut_relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_Pixel::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)),this,SLOT(setPartsFlags(PRDCommonFlags::InDetPartsFlags)));
  setPartsFlags(controller->inDetPartsFlags());

  connect(controller,SIGNAL(pixelMinNRDOPerClusterChanged(unsigned)),this,SLOT(setMinNRDOPerCluster(unsigned)));
  setMinNRDOPerCluster(controller->pixelMinNRDOPerCluster());
}
