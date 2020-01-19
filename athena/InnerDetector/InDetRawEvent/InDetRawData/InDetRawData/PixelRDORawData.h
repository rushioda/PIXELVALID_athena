/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelRawData.h
//   Header file for class PixelRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for Pixel
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef INDETRAWDATA_PIXELRDORAWDATA_H
#define INDETRAWDATA_PIXELRDORAWDATA_H

// Base classes
#include "InDetRawData/InDetRawData.h"


class PixelRDORawData :   public InDetRawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline compact id of the readout channel, 
  // the word
  PixelRDORawData(const Identifier rdoId, const unsigned int word);

  // Destructor:
  virtual ~PixelRDORawData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////


  // pure virtual
  virtual int getToT() const = 0;    // Time over Threshold value 0-255
  virtual int getBCID() const = 0;   // Beam Crossing ID
  virtual int getLVL1A()const = 0; // Level 1 accept, 0-15, used if reading 
                             // consecutive BCOs
  virtual int getLVL1ID()const = 0;  // ATLAS LVL1 0-255
  ///////////////////////////////////////////////////////////////////
  // Static methods:
  ///////////////////////////////////////////////////////////////////



  // Create a new PixelRawData and return a pointer to it:
  //  static PixelRawData *newObject(const Identifier rdoId, const unsigned int word);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  
  PixelRDORawData();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // INDETRAWDATA_PIXELRAWDATA_H