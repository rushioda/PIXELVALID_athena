/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1QtUtils                          //
//                                                            //
//  Description: Misc. utilities.                             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1QTUTILS_H
#define VP1QTUTILS_H

#include <QtCore/QString>
//NB: Keep includes very few here!
class QImage;

class VP1QtUtils {
public:

  ////////////////////////////////////////////////////////////
  //       For getting/setting environment variables        //
  ////////////////////////////////////////////////////////////

  static bool environmentVariableIsSet( const QString& name );//If variable is at all defined (returns true if variable has value "0")
  static QString environmentVariableValue( const QString& name );//Returns "" if not defined.
  static bool environmentVariableIsOn( const QString& name );//Returns false if undefined or value is "0", "", "false"/"off"/"no" (any capitalisation). Otherwise returns true.

  static void setEnvironmentVariable( const QString& name, const QString& content );
  static void unsetEnvironmentVariable( const QString& name );

  //////////////////////////////////////
  //       Image manipulations        //
  //////////////////////////////////////

  static QImage fadeImage(QImage img0, QImage img1, double fadefact );

private:
  VP1QtUtils();
  ~VP1QtUtils();
};

#endif
