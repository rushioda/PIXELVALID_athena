/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoTessellatedSolid_h
#define GeoTessellatedSolid_h 1

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoFacet.h"

class GeoTessellatedSolid : public GeoShape
{
 public:
  GeoTessellatedSolid();

  virtual double volume() const;

  virtual const std::string& type() const;
  virtual ShapeType typeID() const;

  static const std::string& getClassType();
  static ShapeType getClassTypeID();

  virtual void exec(GeoShapeAction *action) const;

  void addFacet(GeoFacet*);
  GeoFacet* getFacet(size_t) const;
  size_t getNumberOfFacets() const;

 protected:
  virtual ~GeoTessellatedSolid();

 private:
  GeoTessellatedSolid(const GeoTessellatedSolid &right);
  GeoTessellatedSolid& operator=(const GeoTessellatedSolid &right);

  static const std::string s_classType;
  static const ShapeType s_classTypeID;

  std::vector<GeoFacet*> m_facets;
};

inline const std::string& GeoTessellatedSolid::getClassType()
{
  return s_classType;
}

inline ShapeType GeoTessellatedSolid::getClassTypeID()
{
  return s_classTypeID;
}

#endif
