/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoGenericTrap_h
#define GeoGenericTrap_h 1

#include "GeoModelKernel/GeoShape.h"
#include <vector>
#include <CLHEP/Vector/TwoVector.h>

typedef CLHEP::Hep2Vector GeoTwoVector;
typedef std::vector<GeoTwoVector> GeoGenericTrapVertices;

class GeoGenericTrap : public GeoShape
{
 public:
  GeoGenericTrap(double ZHalfLength, const GeoGenericTrapVertices& Vertices);

  virtual double volume() const;

  virtual const std::string& type() const;
  virtual ShapeType typeID() const;

  static const std::string& getClassType();
  static ShapeType getClassTypeID();

  virtual void exec(GeoShapeAction *action) const;

  double getZHalfLength() const;
  const GeoGenericTrapVertices& getVertices() const;

 protected:
  virtual ~GeoGenericTrap();

 private:
  GeoGenericTrap(const GeoGenericTrap &right);
  GeoGenericTrap& operator=(const GeoGenericTrap &right);

  static const std::string s_classType;
  static const ShapeType s_classTypeID;

  double m_zHalfLength;
  GeoGenericTrapVertices m_vertices;
};

inline const std::string& GeoGenericTrap::getClassType()
{
  return s_classType;
}

inline ShapeType GeoGenericTrap::getClassTypeID()
{
  return s_classTypeID;
}

#endif
