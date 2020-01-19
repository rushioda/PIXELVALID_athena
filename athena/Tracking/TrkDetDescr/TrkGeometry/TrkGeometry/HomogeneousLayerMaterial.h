/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HomogeneousLayerMaterial.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETY_HOMOGENOUSLAYERMATERIAL_H
#define TRKGEOMETY_HOMOGENOUSLAYERMATERIAL_H

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//Trk
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <vector>

class HomogeneousLayerMaterialCnv_p1;

namespace Trk {


  class BinUtility;
  /** 
   @class HomogeneousLayerMaterial

   It extends the LayerMaterialProperties base class

   This class encapsulates the logics to build pre/post/full update material 
   for Layer structures, it hosts 5 different MaterialProperties in the following order:
   
    - fullUpdate          (crossing full Layer in both directions)
    - alongPreUpdate      (passing material to sensitive part in direction of the normal vector)
    - alongPostUpdate     (passing material after sensitive part in direction of the normal vector)
    - oppositePreUpdate   (passing material to sensitive part in opposite direction to the normal vector)
    - poopsitePostUpdate  (passing material after sensitive part in opposite direction to the normal vector)

    This is handeled by a full Lauyer information and a split factor

      
   @author Andreas.Salzburger@cern.ch 
   */

  class HomogeneousLayerMaterial : public LayerMaterialProperties {
    
    public:
      /** Default Constructor - creates empty HomogeneousLayerMaterial */
      HomogeneousLayerMaterial();
      
      /**Explizit constructor with only full MaterialProperties, 
         alongPre and oppositePre, alongPre and oppositePost are obviously swapped*/
      HomogeneousLayerMaterial(const MaterialProperties& full, double splitFactor);
      
      /**Copy Constructor */  
      HomogeneousLayerMaterial(const HomogeneousLayerMaterial& mprop);
      
      /**Destructor*/
      virtual ~HomogeneousLayerMaterial();
      
      /**Pseudo-Constructor clone()*/ 
      HomogeneousLayerMaterial* clone() const override;
      
      /** Assignment operator */
      HomogeneousLayerMaterial& operator=(const HomogeneousLayerMaterial& lmp);

      /** Scale operator */
      virtual HomogeneousLayerMaterial& operator*=(double scale) override;

      /**Return method for full material description of the Layer */
      virtual const MaterialProperties* fullMaterial(const Amg::Vector3D& gp) const override;

      /**Direct access via bins to the MaterialProperties */
      virtual const MaterialProperties* material(size_t ib0, size_t ib1) const override;
      
      /** Return the BinUtility */
      const BinUtility* binUtility() const  override { return 0; }
      
      /** Update the BinUtility if necessary - passing ownership of the utility class*/
      virtual void updateBinning(BinUtility*) const override { }
          
      /** Output Method for MsgStream, to be overloaded by child classes */
      MsgStream& dump(MsgStream& sl) const override;
      /** Output Method for std::ostream, to be overloaded by child classes */
      std::ostream& dump(std::ostream& sl) const override;      

    private:
      friend class ::HomogeneousLayerMaterialCnv_p1;

      /** The five different MaterialProperties */
      MaterialProperties*           m_fullMaterial;
                                            
  };
  
inline HomogeneousLayerMaterial* HomogeneousLayerMaterial::clone() const
  { return new HomogeneousLayerMaterial(*this); }  
  
inline const MaterialProperties* HomogeneousLayerMaterial::fullMaterial(const Amg::Vector3D&) const
  { return m_fullMaterial; }
      
inline const MaterialProperties* HomogeneousLayerMaterial::material(size_t, size_t) const
  { return m_fullMaterial; }      
    
} // end of namespace

#endif 


