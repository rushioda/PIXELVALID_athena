/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODSysCommonData                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "AODSysCommonData.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1AODSystems/AODSystemController.h"
#include "AODHandleBase.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QTreeWidgetItem>

#include<map>
#include "xAODBase/IParticle.h"
#include <Inventor/SoPath.h>

//____________________________________________________________________
class AODSysCommonData::Imp {
public:
  std::map<SoNode*,AODHandleBase*> nodeToHandle;
  AODHandleBase* last_selectedHandle;
};

//____________________________________________________________________
AODSysCommonData::AODSysCommonData(VP1AODSystem * sys,AODSystemController * controller)
  : VP1HelperClassBase(sys,"AODSysCommonData"), d(new Imp), m_3dsystem(sys),
    m_controller(controller), m_extrapolator(0)
{
  m_singlePoint = new SoPointSet;
  m_singlePoint->ref();
  SoVertexProperty * vertices = new SoVertexProperty;
  vertices->vertex.set1Value(0,0.0f,0.0f,0.0f);
  m_singlePoint->numPoints=1;
  m_singlePoint->vertexProperty.setValue(vertices);
  
  d->last_selectedHandle=0;
}

//____________________________________________________________________
AODSysCommonData::~AODSysCommonData()
{
  m_singlePoint->unref();
  delete d;
}

void AODSysCommonData::registerHandle( AODHandleBase* h ){
  // if (d->nodeToHandle.find(node)!=d->nodeToHandle.end()){
  //   messageVerbose("AODSysCommonData::registerHandle - handle already registered.");
  // }
  d->nodeToHandle[h->nodes()]=h;
}

void AODSysCommonData::deregisterHandle( AODHandleBase* h ){
  // if (d->nodeToTrackHandle.find(node)==d->nodeToHandle.end()){
  //   message("AODSysCommonData::deregisterHandle - handle not registered!");
  // }
  
  for (auto it : d->nodeToHandle) {
    if (it.second == h) {
      d->nodeToHandle.erase(it.first);
      break;
    }
  }
}
AODHandleBase* AODSysCommonData::getHandleFromNode( SoNode* node ) {
  auto it = d->nodeToHandle.find(node);
  if (it==d->nodeToHandle.end()){
    message("AODSysCommonData::getHandleFromNode - handle not registered!");
    message("AODSysCommonData::getHandleFromNode - have this many handles:"+str(d->nodeToHandle.size()));
    
    return 0;
  }
  return it->second;
}

//____________________________________________________________________
AODHandleBase* AODSysCommonData::getHandleFromNode(const SoPath*path) {
  const int n(path?path->getLength():0);
  AODHandleBase * handle(0);
  for (int i = 0; i < n; ++i) {
    if (path->getNodeFromTail(i)->getTypeId()==SoSeparator::getClassTypeId()) {
      handle = getHandleFromNode(static_cast<SoSeparator*>(path->getNodeFromTail(i)));
      if (handle)
        return handle;
    }
  }
  return 0;
}

SoNode* AODSysCommonData::getNodeFromBrowser( QTreeWidgetItem* item ){
  message("AODSysCommonData::getNodeFromBrowser - about to loop over this many handles:"+str(d->nodeToHandle.size()));
  if (!item)
    return 0;
  std::map<SoNode*,AODHandleBase*>::iterator it = d->nodeToHandle.begin(), itEnd=d->nodeToHandle.end();
  for (; it!=itEnd;++it)
    if (it->second->browserTreeItem()==item) return it->first;
  return 0;
}



void AODSysCommonData::setLastSelectedHandle(AODHandleBase* h) {
  d->last_selectedHandle=h;
}
