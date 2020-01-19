/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Floor                          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GuideLineSystems/VP1Floor.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/SbColor4f.h>

#include "GaudiKernel/SystemOfUnits.h"

//____________________________________________________________________
bool VP1Floor::calcParsFromExtentAndSpacing( VP1HelperClassBase*helper,const double& extent, const double& spacing, const int& nmaxlimit, int& nmax, double& distmax )
{
  if (extent<0.0||spacing<=0.0)
    return false;
  nmax=static_cast<int>(extent/spacing+0.5);
  if (nmax<1)
    nmax =1;
  if (nmax>nmaxlimit) {
    helper->message("Too many lines requested. All will not be shown.");
    nmax=nmaxlimit;
  }
  distmax = nmax*spacing;
  return true;
}

//____________________________________________________________________
class VP1Floor::Imp {
public:
  Imp(VP1Floor *,
      SoSeparator * attachsep);
  VP1Floor * theclass;
  SoSeparator * attachSep;

  bool shown;
  SbColor4f colourAndTransp;
  double extent;
  double spacing;
  double vertpos;

  SoSeparator * sep;

  void rebuild3DObjects();
  void updateColour();
};

//____________________________________________________________________
VP1Floor::VP1Floor(SoSeparator * attachsep,
		   IVP1System * sys,QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"VP1Floor"), d(new Imp(this,attachsep))
{
}

//____________________________________________________________________
VP1Floor::~VP1Floor()
{
  setShown(false);
  if (d->sep)
    d->sep->unref();
  d->attachSep->unref();
  delete d;
}

//____________________________________________________________________
VP1Floor::Imp::Imp(VP1Floor *tc,SoSeparator * as)
  : theclass(tc), attachSep(as), shown(false),
    colourAndTransp(SbColor4f(1,1,1,1)),extent(10), spacing(1), vertpos(0), sep(0)
{
  attachSep->ref();
}

//____________________________________________________________________
void VP1Floor::Imp::rebuild3DObjects()
{
  theclass->messageVerbose("(Re)building 3D objects");

  if (sep) {
    sep->removeAllChildren();
  } else {
    sep = new SoSeparator;
    sep->ref();
  }

  const bool save = sep->enableNotify(false);

  int nmax; double distmax;
  if (!calcParsFromExtentAndSpacing( theclass, extent, spacing, VP1Floor::nMax(), nmax, distmax )) {
    nmax = 10;
    distmax = 10*Gaudi::Units::m;
    theclass->message("ERROR: Problems calculating nmax/distmax.");
  }

  SoVertexProperty * floor_vertices = new SoVertexProperty();
  int ivert(0);
  int nsublines(0);
  for (int ix = -nmax; ix<=nmax; ++ix) {
    double x = ix*spacing;
    floor_vertices->vertex.set1Value(ivert++,x,vertpos,-distmax);
    floor_vertices->vertex.set1Value(ivert++,x,vertpos,+distmax);
    ++nsublines;
  }
  for (int iz = -nmax; iz<=nmax; ++iz) {
    double z = iz*spacing;
    floor_vertices->vertex.set1Value(ivert++,-distmax,vertpos,z);
    floor_vertices->vertex.set1Value(ivert++,+distmax,vertpos,z);
    ++nsublines;
  }

  floor_vertices->materialBinding=SoMaterialBinding::OVERALL;
  floor_vertices->normalBinding=SoNormalBinding::OVERALL;
  SoLineSet * line = new SoLineSet();
  line->numVertices.enableNotify(FALSE);
  line->numVertices.setNum(nsublines);
  for (int i=0;i<nsublines;++i)
    line->numVertices.set1Value(i,2);
  line->vertexProperty = floor_vertices;
  line->numVertices.enableNotify(TRUE);
  line->numVertices.touch();

  sep->addChild(line);
  updateColour();

  if (save) {
    sep->enableNotify(true);
    sep->touch();
  }

}

//____________________________________________________________________
void VP1Floor::Imp::updateColour()
{
  theclass->messageVerbose("Updating packed colour");
  if (!sep||sep->getNumChildren()<1)
    return;
  SoNode * n = sep->getChild(0);
  if (!n||n->getTypeId()!=SoLineSet::getClassTypeId())
    return;
  SoLineSet * line = static_cast<SoLineSet*>(n);
  SoVertexProperty * vertices = static_cast<SoVertexProperty *>(line->vertexProperty.getValue());
  if (!vertices)
    return;
  vertices->orderedRGBA = colourAndTransp.getPackedValue();
}

//____________________________________________________________________
void VP1Floor::setShown(bool b)
{
  messageVerbose("Signal received: setShown("+str(b)+")");
  if (d->shown==b)
    return;
  d->shown=b;
  if (d->shown) {
    d->rebuild3DObjects();
    if (d->attachSep->findChild(d->sep)<0)
      d->attachSep->addChild(d->sep);
  } else {
    if (d->sep&&d->attachSep->findChild(d->sep)>=0)
      d->attachSep->removeChild(d->sep);
  }
}

//____________________________________________________________________
void VP1Floor::setColourAndTransp(const SbColor4f&ct)
{
  messageVerbose("Signal received in setColourAndTransp slot.");
  if (d->colourAndTransp==ct)
    return;
  d->colourAndTransp=ct;
  if (d->shown)
    d->updateColour();
}

//____________________________________________________________________
void VP1Floor::setExtent(const double& e)
{
  messageVerbose("Signal received: setExtent("+str(e)+")");
  if (d->extent==e)
    return;
  d->extent=e;
  if (d->shown)
    d->rebuild3DObjects();
}

//____________________________________________________________________
void VP1Floor::setSpacing(const double&s)
{
  messageVerbose("Signal received: setSpacing("+str(s)+")");
  if (d->spacing==s)
    return;
  d->spacing=s;
  if (d->shown)
    d->rebuild3DObjects();
}

//____________________________________________________________________
void VP1Floor::setVerticalPosition(const double&p)
{
  messageVerbose("Signal received: setVerticalPosition("+str(p)+")");
  if (d->vertpos==p)
    return;
  d->vertpos=p;
  if (d->shown)
    d->rebuild3DObjects();
}
