/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray0D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY0D_H
#define TRKDETDESCRUTILS_BINNEDARRAY0D_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray0D

   BinnedArray0D - BinnedArray flavour with just one entry,
   allows to have the same structure for just one single
   contained object. 

   @author Andreas.Salzburger@cern.ch
   */

  template <class T> class BinnedArray0D : public BinnedArray<T> {

    public:
     /**Default Constructor - needed for inherited classes */
     BinnedArray0D() throw (GaudiException) :
      BinnedArray<T>(),
      m_object(0),
      m_arrayObjects()
     {}

     /**Constructor  from oneobject  */
     BinnedArray0D(const SharedObject<const T>& obj) throw (GaudiException) :
      BinnedArray<T>(),
      m_object(obj),
      m_arrayObjects(1,obj.getPtr())
    {}

     /**Copy Constructor - copies only pointers !*/
     BinnedArray0D(const BinnedArray0D& barr) throw (GaudiException):
      m_object(barr.m_object),
      m_arrayObjects(barr.m_arrayObjects)
    {}

     /**Assignment operator*/
     BinnedArray0D& operator=(const BinnedArray0D& barr)
      {
        if (this != &barr){
          delete m_arrayObjects; m_arrayObjects = 0;
          m_object = barr.m_object;
          m_object = barr.m_arrayObjects;
       }
        return *this;
      }
     /** Implizit Constructor */
     BinnedArray0D* clone() const
     { return new BinnedArray0D(*this); }

     /**Virtual Destructor*/
     ~BinnedArray0D()
      { delete m_arrayObjects; }


     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined */
     const T* object(const Amg::Vector2D&) const
     { return m_object.getPtr(); }

     /** Returns the pointer to the templated class object from the BinnedArray
         it returns 0 if not defined */
     const T* object(const Amg::Vector3D& gp) const
     { return m_object.getPtr(); }


     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined */
     const T* entryObject(const Amg::Vector3&) const
     { return m_object.getPtr(); }


     /** Returns the pointer to the templated class object from the BinnedArray (params: glopos and glomom)
      */
     const T* nextObject(const Amg::Vector3D&, const Amg::Vector3D&, bool associatedResult=true) const
     { return m_object.getPtr(); }

     /** Return all objects of the Array */
     const std::vector< const T* >& arrayObjects() const {
       return m_arrayObjects;
     }

     /** Number of Entries in the Array */
     unsigned int arrayObjectsNumber() const { return 1; }

     /** Return the BinUtility*/
     const BinUtility* binUtility() const { return 0; }

     /** Return the transform*/
     const Amg::Transform3D* transform() const { return 0; }

     /** Reposition */
     void updateTransform( Amg::Transform3D& ) const { }

    private:
     SharedObject<const T>                          m_array;        //!< direct storage of the object
     std::vector< const T* >                        m_arrayObjects; //!< forced 1D vector of pointers to class

  };


} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY1D_H

