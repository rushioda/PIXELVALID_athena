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


#include "QatPlotting/PlotOrbit.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "QatPlotting/AbsPlotter.h"

#include "QatDataAnalysis/Query.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <stdexcept>


// Constructor
PlotOrbit::PlotOrbit(const Genfun::AbsFunction & functionX,
		     const Genfun::AbsFunction & functionY,
		     double t0, double t1):
  Plotable(),c(new Clockwork())
{
  c->functionX=functionX.clone();
  c->functionY=functionY.clone();
  c->t0=t0;
  c->t1=t1;
}

// Copy constructor:
PlotOrbit::PlotOrbit(const PlotOrbit & source):
  Plotable(),c(new Clockwork())
{
  c->functionX=source.c->functionX->clone();
  c->functionY=source.c->functionY->clone();
  c->rect=source.c->rect;
  c->t0=source.c->t0;
  c->t1=source.c->t1;

}

// Assignment operator:
PlotOrbit & PlotOrbit::operator=(const PlotOrbit & source){
  if (&source!=this) {
    if (c->functionX) delete c->functionX;
    if (c->functionY) delete c->functionY;
    if (c->myProperties) delete c->myProperties;
    c->functionX=source.c->functionX->clone();
    c->functionY=source.c->functionY->clone();
    c->rect=source.c->rect;
    c->t0=source.c->t0;
    c->t1=source.c->t1;
    c->myProperties = source.c->myProperties ? new Properties(*source.c->myProperties):nullptr;

  }
  return *this;
} 
 

#include <iostream>
// Destructor
PlotOrbit::~PlotOrbit(){
  delete c;
}



const QRectF & PlotOrbit::rectHint() const {
  static const int NSTEPS=500;// Synch to value in describeYourself();
  double interval = (c->t1-c->t0);
  double d=interval/NSTEPS;
  double minX=1E100,maxX=-minX, minY=minX, maxY=maxX;

  for (int i=0;i<NSTEPS;i++) {
    double t = c->t0 + i*d;
    double x= (*c->functionX)(t);
    double y= (*c->functionY)(t);
    minX=std::min(minX,x);
    maxX=std::max(maxX,x);
    minY=std::min(minY,y);
    maxY=std::max(maxY,y);
  }
  double iX=maxX-minX,iY=maxY-minY;
  minX-=iX/10.0;
  minY-=iY/10.0;
  maxX+=iX/10.0;
  maxY+=iY/10.0;
  c->rect.setLeft(minX);
  c->rect.setRight(maxX);
  c->rect.setTop(minY);
  c->rect.setBottom(maxY);

  return c->rect;
}


// Get the graphic description::
void PlotOrbit::describeYourselfTo(AbsPlotter *plotter) const {
  

  QPen pen = properties().pen;


  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  {  
    unsigned int dimX = c->functionX->dimensionality();
    unsigned int dimY = c->functionY->dimensionality();
    if (dimX!=1|| dimY!=1) throw std::runtime_error("PlotOrbit:  requires two functions of exactly 1 argument");
  }
  double minX=plotter->rect()->left(), maxX=plotter->rect()->right(); 
  double minY=plotter->rect()->top(),  maxY=plotter->rect()->bottom();
 

  int NPOINTS = 500; // Synch to value in rectHint();
  double interval = (c->t1-c->t0);
  double d=interval/NPOINTS;

  const LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : NULL;
  const LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : NULL;

  {
    // Just a line:

    Query<QPointF>      cachedPoint;
    QPainterPath        *path=NULL; 
    bool                empty=true;
    for (int i=0; i<NPOINTS+1;i++) {
      bool                closePath=false;
      double t = c->t0 + d*i;
      double x = (*c->functionX)(t);
      double y = (*c->functionY)(t);
      {  
	
	//double y = (*c->function) (x);
	QPointF point(x,y);
	if (y < maxY && y > minY && x < maxX && x > minX) { // IN RANGE
	  if (!path) path = new QPainterPath();
	  if (empty) {
	    empty=false;
	    if (cachedPoint.isValid()) {
	      QPointF intersection;
	      if (Clockwork::intersect(plotter->rect(),cachedPoint, point,intersection)) {
		Clockwork::moveTo(path,m,intersection,toLogX,toLogY);
		Clockwork::lineTo(path,m,point,toLogX,toLogY);
	      }
	      else {
		if (Clockwork::maxOut(plotter->rect(), point, intersection)) {
		  Clockwork::moveTo(path,m,intersection,toLogX,toLogY);
		  Clockwork::lineTo(path,m,point,toLogX,toLogY);
		}
		else {
		  std::cerr << "Intersection is failing" << std::endl;
		}
	      }
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
	      if (Clockwork::intersect(plotter->rect(),cachedPoint, point,intersection)) {
		Clockwork::lineTo(path,m,intersection,toLogX,toLogY);
	      }
	      else {
		if (Clockwork::maxOut(plotter->rect(), point, intersection)) {
		  Clockwork::moveTo(path,m,intersection,toLogX,toLogY);
		  Clockwork::lineTo(path,m,point,toLogX,toLogY);
		}
		else {
		  std::cerr << "Intersection is failing" << std::endl;
		}
	      }
	    }
	    closePath=true;
	    empty    =true;
	  }
	}
	cachedPoint=point;
      }
      if (i==NPOINTS && path) closePath=true;
      if (closePath) {
	QGraphicsPathItem *polyline=new QGraphicsPathItem(*path);
	polyline->setPen(pen);
	polyline->setMatrix(mInverse);
	plotter->scene()->addItem(polyline);
	plotter->group()->addToGroup(polyline);
	delete path;
	path=NULL;
      }
    }
  }
  
  delete toLogX;
  delete toLogY;
}


const PlotOrbit::Properties & PlotOrbit::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotOrbit::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotOrbit::resetProperties() {
  delete c->myProperties;
}
