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


#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/AbsPlotter.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemGroup>
#include <iostream>
#include <iomanip>
#include <sstream>
namespace{
  const CLHEP::HepVector defaultVector(2);
};

class PlotErrorEllipse::Clockwork {

  public:

  Clockwork():X(defaultVector), U0(defaultVector),U1(defaultVector), 
              nRectangle(),myProperties(nullptr), defaultProperties(), style() {}
  ~Clockwork() { delete myProperties;}


  CLHEP::HepVector                      X;
  CLHEP::HepVector                      U0;
  CLHEP::HepVector                      U1;
  QRectF                                nRectangle;         // The "natural" bounding rectangle
  Properties                            *myProperties;
  Properties                            defaultProperties;
  Style                                 style;
};

// Constructor
PlotErrorEllipse::PlotErrorEllipse( 
				   double x, double y,
				   double sx2, double sy2, double sxy, Style style)
  :Plotable(),c(new Clockwork())

{
  c->X=CLHEP::HepVector(2);
  c->U0=CLHEP::HepVector(2);
  c->U1=CLHEP::HepVector(2);
  c->style=style;
  
  CLHEP::HepSymMatrix M(2);
  M[0][0]=sx2;
  M[0][1]=sxy;
  M[1][1]=sy2;

  c->X[0]=x;
  c->X[1]=y;
  double T = M.trace();
  double D = M.determinant();
  double L0 = (T+sqrt(T*T-4*D))/2.0;
  double L1 = (T-sqrt(T*T-4*D))/2.0;

  if (sxy!=0) {
    c->U0[0]=1;
    c->U0[1]=(L0-sx2)/sxy;
    c->U1[0]=1;
    c->U1[1]=(L1-sx2)/sxy;
    c->U0 /= c->U0.norm();
    c->U1 /= c->U1.norm();
    c->U0 *= sqrt(L0);
    c->U1 *= sqrt(L1);
  }
  else {
    c->U0[0]=sqrt(sx2);
    c->U0[1]=0;
    c->U1[0]=0;
    c->U1[1]=sqrt(sy2);
  }

  double factor=0;
  if (c->style==ONEUNITCHI2) {
    factor=1;
  }
  else if (c->style==ONESIGMA) {
    factor=sqrt(2.30);
  }
  else if (c->style==TWOSIGMA) {
    factor=sqrt(6.18);
  }
  else if (c->style==THREESIGMA) {
    factor=sqrt(11.83);
  }
  else if (c->style==NINETY) {
    factor=sqrt(4.61);
  }
  else if (c->style==NINETYFIVE) {
    factor=sqrt(5.99);
  }
  else if (c->style==NINETYNINE) {
    factor=sqrt(9.21);
  }

  c->U0 *= factor; // For 68% confidence:
  c->U1 *= factor; // For 68% confidence;

  // Now make the bounding box:
  double minX=1E-30,maxX=-1E30;
  double minY=1E-30,maxY=-1E30;
  if (c->U0[0]>maxX) maxX=c->U0[0];
  if (c->U0[0]<minX) minX=c->U0[0];
  if (c->U1[0]>maxX) maxX=c->U1[0];
  if (c->U1[0]<minX) minX=c->U1[0];
  if (c->U0[1]>maxY) maxY=c->U0[1];
  if (c->U0[1]<minY) minY=c->U0[1];
  if (c->U1[1]>maxY) maxY=c->U1[1];
  if (c->U1[1]<minY) minY=c->U1[1];

  minX += x;
  maxX += x;
  minY += y;
  maxY += y;
  //  c->nRectangle=QRectF(QPointF(minX-(maxX-minX)/2.0,minY+(maxY-minY)/2.0),QSizeF(maxX-minX, maxY-minY));
  double max = std::max(c->U0.norm(),c->U1.norm());

  c->nRectangle=QRectF(c->X[0]-2*max, c->X[1]-2*max, 4*max, 4*max);

}



// Destructor
PlotErrorEllipse::~PlotErrorEllipse(){
  delete c;
}


// Get the "natural maximum R"
const QRectF & PlotErrorEllipse::rectHint() const {
  return c->nRectangle;
}



// Describe to plotter, in terms of primitives:
void PlotErrorEllipse::describeYourselfTo(AbsPlotter *plotter) const{
  if (plotter->isLogY()) return;
  if (plotter->isLogX()) return;

  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  QPainterPath path;
  bool started=false;
  for (int i=0;i<360;i++) {
    CLHEP::HepVector V=c->U0*sin(2*M_PI*i/360.0) + c->U1*cos(2*M_PI*i/360.0)+c->X;
    if (plotter->rect()->contains(QPointF(V[0],V[1]))){
      double x = V[0];
      double y = V[1];
      if (!started) {
	started=true;
	path.moveTo(m.map(QPointF(x,y)));
      }
      else {
	path.lineTo(m.map(QPointF(x,y)));
      }
    }
  }

  QGraphicsPathItem *polyline=new QGraphicsPathItem(path);
  polyline->setPen(pen);
  polyline->setBrush(brush);
  polyline->setMatrix(mInverse);
  plotter->scene()->addItem(polyline);
  plotter->group()->addToGroup(polyline);

}



const PlotErrorEllipse::Properties & PlotErrorEllipse::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotErrorEllipse::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotErrorEllipse::resetProperties() {
  delete c->myProperties;
}

