/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialProperties.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALPROPERTIES_H
#define TRKGEOMETRY_MATERIALPROPERTIES_H

#include "TrkGeometry/Material.h"
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>

class MaterialPropertiesCnv_p1;

namespace Trk {

  /** 
   @class MaterialProperties

   Material with information about thickness of material
   
   the units are :
    - thickness [mm] ( only used for layer description)
    - X0  [mm]
    - L0  [mm]
    - A   [g/mole]
    - Z 
    - rho [g/mm3]

   The Trk::MaterialProperties describe material associated with a Trk::Layer
   

   @author Andreas.Salzburger@cern.ch 
   */

  class MaterialProperties {
          
    public:
        
      /** Default Constructor */
      MaterialProperties();
      
      /** Constructor - for averaged material */
      MaterialProperties(float path,
                         float Xo,
                         float Lo,
                         float averageA,
                         float averageZ,
                         float averageRho,
                         float dEdX=0.);

      /** Constructor - for full Material class */
      MaterialProperties(const Material& material, float path);

      /** Copy Constructor */
      MaterialProperties(const MaterialProperties& mprop);
                
      /** Destructor */
      virtual ~MaterialProperties(){}

      /** Pseudo-Constructor clone() */
      virtual MaterialProperties* clone() const;
       
      /** Assignment Operator */
      MaterialProperties& operator =( const MaterialProperties& mprop);
      
      /** Scale operator - scales the material thickness */
      MaterialProperties& operator *= ( float scale);

      /** Return the stored Material */
      const Material& material() const;
      
      /** Return the radiation length */
      float x0() const;

      /** Return the nuclear interaction length */
      float l0() const;

      /** Return the thickness in mm */
      float thickness() const;

      /** Return the radiationlength fraction */
      float thicknessInX0() const;

      /** Return the nuclear interaction length fraction */
      float thicknessInL0() const;
                 
      /** Returns the average Z of the material */
      float averageZ() const;
                  
      /** Return the average A of the material [gram/mole] */
      float averageA() const;
            
      /** Return the average density of the material
         - in [g/mm^3]
        */
      float averageRho() const;
      
      /** Return the @f$ Z/A * rho @f$ */ 
      float zOverAtimesRho() const;
      
      /** Return the @f$ d* Z/A * rho @f$ */ 
      float zOverAtimesRhoTimesD() const;
      
      /** Return method for @f$ dE/dX @f$*/
      float dEdX() const;

      /** Material averaging */
      void addMaterial(const Material& mp, float dInX0) const; 
      
      /** Set Material */
      void setMaterial(const Material& mp, float thickness=1.) const;
                                                                                  
    protected:
      friend class ::MaterialPropertiesCnv_p1;

      /** Set dEdX       - important for material calibarion */
      virtual void setDeDx(float dEdX) const;        
      
      mutable Material   m_material;
      
      mutable float      m_dInX0;      //!< thickness in units of radiation length
      mutable float      m_dInL0;      //!< thickness in units of nuclear interaction length
      mutable float      m_zOaTrTd;    //!< @f$ \frac{Z}{A}\cdot\rho\cdot d @f$ - in ATLAS units

  };

  /** Return method for the full material */
  inline const Material& MaterialProperties::material() const 
    { return m_material; }

  /** Return method for thicknes in units of radiation length - dimensionless */
  inline float MaterialProperties::thicknessInX0() const { return m_dInX0; }

  /** Return method for thickness in units of nuclear interaction length - dimensionless */
  inline float MaterialProperties::thicknessInL0() const { return m_dInL0; }

  /** Return method for thickness in mm */
  inline float MaterialProperties::thickness() const { return m_dInX0*m_material.X0; }

  /** Return method for radiation length - in [mm] */
  inline float MaterialProperties::x0() const
    { return m_material.X0; }
    
  /** Return method for nuclear interaction length - in [mm] */
  inline float MaterialProperties::l0() const
    { return m_material.L0; }

  /** Return method for @f$ \frac{A}{Z}\cdot\rho @f$ */
  inline float MaterialProperties::zOverAtimesRho() const
    { return m_material.zOaTr; }

  /** Return method for @f$ \frac{A}{Z}\cdot\rho\cdot d @f$ */
  inline float MaterialProperties::zOverAtimesRhoTimesD() const
    { return m_zOaTrTd; }

  /** Return method for @f$ A @f$ */
  inline float MaterialProperties::averageA() const
    { return m_material.A; }

  /** Return method for @f$ Z @f$ */
  inline float MaterialProperties::averageZ() const
    { return m_material.Z; }

  /** Return method for @f$ Z @f$ */
  inline float MaterialProperties::averageRho() const
    { return m_material.rho; }

  /** Return method for @f$ dE/dX @f$ */
  inline float MaterialProperties::dEdX() const
    { return m_material.dEdX; }

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
  MsgStream& operator<<( MsgStream& sl, const MaterialProperties& mprop);
  std::ostream& operator<<( std::ostream& sl, const MaterialProperties& mprop);

  /** Useful typedefs */

  typedef std::vector< const MaterialProperties*>   MaterialPropertiesVector;
  typedef std::vector<MaterialPropertiesVector>     MaterialPropertiesMatrix;

       
} // end of namespace

#endif // TRKGEOMETRY_MATERIALPROPERTIES_H

