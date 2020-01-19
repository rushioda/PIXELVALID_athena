//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "QatPlotting/PlotBand1D.h"
#include "QatPlotting/AbsPlotter.h"
#include "QatPlotting/LinToLog.h"
#include "QatDataAnalysis/Query.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <stdexcept>

class PlotBand1D::Clockwork {

public:

  Clockwork() :
    function1(nullptr),
    function2(nullptr),
    rect(),
    domainRestriction(nullptr),
    myProperties(nullptr),defaultProperties() {}

  ~Clockwork() {
    if (domainRestriction) delete domainRestriction;
    if (function1) delete function1;
    if (function2) delete function2;
  }

  static bool intersect(const QRectF * rect, const QPointF & p1, const QPointF & p2, QPointF & p) {
    double min=rect->top();
    double max=rect->bottom();
    double y1=p1.y();
    double y2=p2.y();
    if (rect->contains(p1) && rect->contains(p2)) {
      return false;
    }
    else if (y1 < min || y2 < min) {
      if (QLineF(p1,p2).intersect(QLineF(rect->topLeft(),rect->topRight()),&p)!=QLineF::BoundedIntersection){
	    return false;
      }
    }
    else if (y1 > max || y2 > max) {
      if (QLineF(p1,p2).intersect(QLineF(rect->bottomLeft(),rect->bottomRight()),&p)!=QLineF::BoundedIntersection){
	    return false;
      }
    }
    return true;
  }

  static bool maxOut(const QRectF * rect, const QPointF & p1, QPointF & p) {
    double min=rect->top();
    double max=rect->bottom();
    double y1=p1.y();
    if (rect->contains(p1)) {
      p=p1;
      return false;
    }
    if (y1>max) {
      p.setX(p1.x());
      p.setY(max);
    }
    else if (y1<min) {
      p.setX(p1.x());
      p.setY(min);
    }
    return true;
  }

  static void moveTo(QPainterPath *path, const QMatrix & m, const QPointF & p, const LinToLog *linToLogX=NULL, const LinToLog *linToLogY=NULL) {
    double x=p.x(),y=p.y();
    if (linToLogX) x = (*linToLogX)(x);
    if (linToLogY) y = (*linToLogY)(y);
    
    path->moveTo(m.map(QPointF(x,y)));
  }

  static void lineTo(QPainterPath *path, const QMatrix & m, const QPointF & p, const LinToLog *linToLogX=NULL, const LinToLog *linToLogY=NULL) {
    double x=p.x(),y=p.y();
    if (linToLogX) x = (*linToLogX)(x);
    if (linToLogY) y = (*linToLogY)(y);
    path->lineTo(m.map(QPointF(x,y)));
  }

  // This is state:
  const Genfun::AbsFunction            *function1;          // The function defining the band.
  const Genfun::AbsFunction            *function2;          // The function defining the band.
  QRectF                                rect;               // The "natural" bounding rectangle
  const Cut<double>                    *domainRestriction;  // Domain restrictions.
  Properties                           *myProperties;       // My properties.  May 
  Properties                            defaultProperties;

};

// Constructor
PlotBand1D::PlotBand1D(const Genfun::AbsFunction & function1,
		       const Genfun::AbsFunction & function2,
			       const QRectF & naturalRectangle):
  Plotable(),c(new Clockwork())
{
  c->function1=function1.clone();
  c->function2=function2.clone();
  c->rect=naturalRectangle;
  c->domainRestriction=nullptr;
}

PlotBand1D::PlotBand1D(const Genfun::AbsFunction & function1,
		       const Genfun::AbsFunction & function2,
			       const Cut<double> & domainRestriction,
			       const QRectF & naturalRectangle):
  Plotable(),c(new Clockwork()) 
{
  c->function1=function1.clone();
  c->function2=function2.clone();
  c->rect=naturalRectangle;
  c->domainRestriction=domainRestriction.clone();
}

// Copy constructor:
PlotBand1D::PlotBand1D(const PlotBand1D & source):
  Plotable(),c(new Clockwork())
{
  c->function1=source.c->function1->clone();
  c->function2=source.c->function2->clone();
  c->rect=source.c->rect;
  c->domainRestriction=source.c->domainRestriction ? source.c->domainRestriction->clone() : nullptr;
}

// Assignment operator:
PlotBand1D & PlotBand1D::operator=(const PlotBand1D & source){
  if (&source!=this) {
  //sroe: what should be assigned, and how?
    if (c->function1) delete c->function1;
    if (c->function2) delete c->function2;
    c->function1=source.c->function1->clone();
    c->function2=source.c->function2->clone();
    c->rect=source.c->rect;
    if (c->myProperties) delete c->myProperties;
    c->myProperties = (source.c->myProperties)? (new Properties(*(source.c->myProperties))) : nullptr;
    delete c->domainRestriction;
    c->domainRestriction = (source.c->domainRestriction) ? (source.c->domainRestriction->clone()) : nullptr;
  }
  return *this;
} 
 

#include <iostream>
// Destructor
PlotBand1D::~PlotBand1D(){
  delete c;
}



const QRectF & PlotBand1D::rectHint() const {
  return c->rect;
}


// Get the graphic description::
void PlotBand1D::describeYourselfTo(AbsPlotter *plotter) const {
  

  QPen pen = properties().pen;
  QBrush brush=properties().brush;


  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  {
    unsigned int dim = c->function1->dimensionality();
    if (dim!=1) throw std::runtime_error("PlotBand1D:  requires a function of exactly 1 argument");
  }  
  {
    unsigned int dim = c->function2->dimensionality();
    if (dim!=1) throw std::runtime_error("PlotBand1D:  requires a function of exactly 1 argument");
  }  

  double minX=plotter->rect()->left(), maxX=plotter->rect()->right(); 
  double minY=plotter->rect()->top(),  maxY=plotter->rect()->bottom();
 

  int NPOINTS = 100; 
  double delta = (maxX-minX)/NPOINTS;

  const LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : NULL;
  const LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : NULL;

  if (brush.style()==Qt::NoBrush) {
    for (int b=0;b<2;b++) {
      
      const Genfun::AbsFunction * function = b==0 ? c->function1: c->function2;

      // Just a line:
      
      Query<QPointF>      cachedPoint;
      QPainterPath        *path=NULL; 
      bool                empty=true;
      for (int i=0; i<NPOINTS+1;i++) {
	bool                closePath=false;
	double x = minX + i*delta;
	if (!c->domainRestriction || (*c->domainRestriction)(x)) {  // Check that X is in function domain 
	  
	  double y = (*function) (x);
	  QPointF point(x,y);
	  if (y < maxY && y > minY) { // IN RANGE
	    if (!path) path = new QPainterPath();
	    if (empty) {
	      empty=false;
	      if (cachedPoint.isValid()) {
		QPointF intersection;
		Clockwork::intersect(plotter->rect(),cachedPoint, point,intersection);
		Clockwork::moveTo(path,m,intersection,toLogX,toLogY);
		Clockwork::lineTo(path,m,point,toLogX,toLogY);
	      }
	      else {
		Clockwork::moveTo(path,m, point,toLogX,toLogY);
	      }
	    }
	    else {
	      Clockwork::lineTo(path,m,point,toLogX, toLogY);
	    }
	  }	
	  else { // OUT OF RANGE
	    if (path) {
	      if (cachedPoint.isValid()) {
		QPointF intersection;
		Clockwork::intersect(plotter->rect(),cachedPoint, point,intersection);
		Clockwork::lineTo(path,m,intersection,toLogX,toLogY);
	      }
	      closePath=true;
	      empty    =true;
	    }
	  }
	  cachedPoint=point;
	}
	else {   // OUT OF DOMAIN 
	  if (path) {
	    closePath=true;
	    empty=true;
	  }
	  cachedPoint=Query<QPointF>();
	}
	if (i==NPOINTS && path) closePath=true;
	if (closePath) {
	  QGraphicsPathItem *polyline=new QGraphicsPathItem(*path);
	  polyline->setPen(pen);
	  polyline->setBrush(brush);
	  polyline->setMatrix(mInverse);
	  plotter->scene()->addItem(polyline);
	  plotter->group()->addToGroup(polyline);
	  delete path;
	  path=NULL;
	}
      }
    }
  }
  else {
    // A fill pattern of some sort..
    double yBase=0;
    QPainterPath        *path=NULL; 
    bool                empty=true;
    for (int b=0;b<2;b++) {
      for (int i=0; i<NPOINTS+1;i++) {
	bool                closePath=false;
	double x = b ? minX + i*delta : maxX-i*delta;
	if (!c->domainRestriction || (*c->domainRestriction)(x)) {  // Check that X is in function domain 
	  
	  double y = b? (*c->function1) (x) : (*c->function2) (x) ;
	  QPointF point(x,y);
	  
	  if (!path) path = new QPainterPath();
	  if (empty) {
	    empty=false;
	    Clockwork::moveTo(path,m, point, toLogX,toLogY);
	  }
	  
	  
	  if (y < maxY && y > minY) { // IN RANGE
	    Clockwork::lineTo(path,m,point,toLogX,toLogY);
	  }	
	  else { // OUT OF RANGE
	    QPointF maxOut;
	    Clockwork::maxOut(plotter->rect(), point,maxOut);
	    Clockwork::lineTo(path,m,maxOut,toLogX,toLogY);
	  }
	}
	else {   // OUT OF DOMAIN 
	  if (path) {
	    closePath=true;
	    empty=true;
	  }
	}
	if (b==1 && i==NPOINTS && path) {
	  closePath=true;
	}
	if (closePath) {
	  QPointF position=mInverse.map(path->currentPosition());
	  position.setY(yBase);
	  Clockwork::lineTo(path,m,position,toLogX,toLogY);
	  QGraphicsPathItem *polyline=new QGraphicsPathItem(*path);
	  polyline->setPen(pen);
	  polyline->setBrush(brush);
	  polyline->setMatrix(mInverse);
	  plotter->scene()->addItem(polyline);
	  plotter->group()->addToGroup(polyline);
	  delete path;
	  path=NULL;
	}
      }
    }
  }
  delete toLogX;
  delete toLogY;
}



const PlotBand1D::Properties & PlotBand1D::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotBand1D::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotBand1D::resetProperties() {
  delete c->myProperties;
}
