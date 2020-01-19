/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLoss.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKMATERIALONTRACK_ENERGYLOSS_H
#define TRKMATERIALONTRACK_ENERGYLOSS_H

#include <iostream>
#include <math.h>
class MsgStream;
class TrackCollectionCnv;

#include <cassert>

namespace Trk {

/** @brief This class describes energy loss material effects in the ATLAS
    tracking EDM.

    Energy loss through ionisation and/or radiation leads to a change
    (reduction) of the momentum. It uncertainty can be asymmetric in this
    class. The quantity is energy since the calculation from energy to
    momentum can be done better inside the MEFupdators (which know the
    particle hypothesis) than the DetDescr tools.
   
   @author Common tracking software group     
   
*/
class EnergyLoss {
  friend class ::TrackCollectionCnv;
 public:
  
  //! default constructor for POOL
  EnergyLoss();
  //! Constructor with @f$\Delta E@f$, @f$\sigma(\Delta E)@f$ and asym. errors
  EnergyLoss (double deltaE,
              double sigmaDeltaE,
              double sigmaMinusDeltaE=0.0,
              double sigmaPlusDeltaE=0.0); 

  //! Constructor with @f$\Delta E@f$, @f$\sigma(\Delta E)@f$ and component info
  EnergyLoss (double deltaE,
              double sigmaDeltaE,
              double mean_ioni,
              double sigma_ioni,
              double mean_rad,
              double sigma_rad); 

  //! Constructor with @f$\Delta E@f$, @f$\sigma(\Delta E)@f$ and component info
  EnergyLoss (double deltaE,
              double sigmaDeltaE,
              double sigmaMinusDeltaE,
              double sigmaPlusDeltaE, 
              double mean_ioni,
              double sigma_ioni,
              double mean_rad,
              double sigma_rad,
              double length); 

  //! Destructor 
  virtual ~EnergyLoss() {};
  
  //! Virtual constructor 
  virtual EnergyLoss* clone() const;
    
  //! returns the @f$ \Delta E @f$ 
  double deltaE() const;
  
  //! returns the symmatric error @f$ \sigma(\Delta E) @f$ 
  double sigmaDeltaE() const;

  //! returns the negative side @f$ \sigma(\Delta E) @f$ 
  double sigmaMinusDeltaE() const;

  //! returns the positive side @f$ \sigma(\Delta E) @f$ 
  double sigmaPlusDeltaE() const;

  // access to eloss components
  double meanIoni() const;
  double sigmaIoni() const;
  double meanRad() const;
  double sigmaRad() const;
  double length() const;

  //update from mean values
  void update(double ioni, double sigi, double rad, double sigr, bool mpv=false) const; 
  
  //update 
  void update( EnergyLoss&, bool mpv=false ) const; 
  
  //set
  void set(double eLoss, double sigde, double ioni, double sigi, double rad, double sigr) const; 
  
  //! Interface method for output, can be overloaded by child classes
  virtual MsgStream&    dump( MsgStream& sl ) const;
  //! Interface method for output, can be overloaded by child classes
  virtual std::ostream& dump( std::ostream& sl ) const;


 private:
  //! @f$ \Delta E @f$        - the estimated or measured energy loss
  mutable double  m_deltaE;           
  //!< @f$ \sigma(\Delta E) @f$ - error on the energy loss
  mutable double  m_sigmaDeltaE;
  //!< @f$ \sigma(\Delta E) @f$ - negative error on the energy loss
  double  m_sigmaMinusDeltaE;
  //!< @f$ \sigma(\Delta E) @f$ - positive error on the energy loss
  double  m_sigmaPlusDeltaE;
  // additional information about components (cache only, not persistified)
  mutable double  m_mean_ioni;          // mean value for ionization 
  mutable double  m_sig_ioni;          // sigma for ionization 
  mutable double  m_mean_rad;          // mean value for radiation 
  mutable double  m_sig_rad;           // sigma for radiation 
  mutable double  m_length;           // 3D length of material 

};

 inline EnergyLoss* EnergyLoss::clone() const
 { return new EnergyLoss(*this); }
 
 inline double EnergyLoss::deltaE() const
 { return m_deltaE; }
 
 inline double EnergyLoss::sigmaDeltaE() const
 { return m_sigmaDeltaE; }
 
 inline double EnergyLoss::sigmaMinusDeltaE() const
 { return m_sigmaMinusDeltaE; }
 
 inline double EnergyLoss::sigmaPlusDeltaE() const
 { return m_sigmaPlusDeltaE; }
 
 inline double EnergyLoss::meanIoni() const
 { return m_mean_ioni; }
 
 inline double EnergyLoss::sigmaIoni() const
 { return m_sig_ioni; }
 
 inline double EnergyLoss::meanRad() const
 { return m_mean_rad; }
 
 inline double EnergyLoss::sigmaRad() const
 { return m_sig_rad; }
 
 inline double EnergyLoss::length() const
 { return m_length;  } // length can be positive and negative like Eloss depending on (back)tracking
 
 inline void EnergyLoss::update(double ioni, double sigi, double rad, double sigr, bool mpv) const
 { m_mean_ioni += ioni;
   m_mean_rad += rad;
   m_sig_ioni += sigi; 
   m_sig_rad  += sigr; 
   m_deltaE += mpv ? 0.9*ioni+0.15*rad : ioni+rad; 
   m_sigmaDeltaE = sqrt( m_sig_ioni*m_sig_ioni + m_sig_rad*m_sig_rad);  
 }

 inline void EnergyLoss::update(EnergyLoss& eloss, bool mpv) const
 { m_mean_ioni += eloss.meanIoni();
   m_mean_rad += eloss.meanRad();
   m_sig_ioni += eloss.sigmaIoni(); 
   m_sig_rad  += eloss.sigmaRad(); 
   m_deltaE += mpv ? 0.9*eloss.meanIoni()+0.15*eloss.meanRad() : eloss.meanIoni()+eloss.meanRad(); 
   m_sigmaDeltaE = sqrt( m_sig_ioni*m_sig_ioni + m_sig_rad*m_sig_rad);  
 }

 inline void EnergyLoss::set(double eloss, double sigde, double ioni, double sigi, double rad, double sigr) const
 { m_mean_ioni = ioni;
   m_mean_rad = rad;
   m_sig_ioni = sigi; 
   m_sig_rad  = sigr; 
   m_deltaE   = ioni + rad  + 0*eloss; 
   m_sigmaDeltaE = sqrt( m_sig_ioni*m_sig_ioni + m_sig_rad*m_sig_rad + 0*sigde*sigde);
 }

//! Overload of << operator for MsgStream for debug output
inline MsgStream& operator << ( MsgStream& sl, const EnergyLoss& eloss)
  { return eloss.dump(sl); }
//! Overload of << operator for std::ostream for debug output
inline std::ostream& operator << ( std::ostream& sl, const EnergyLoss& eloss)
  { return eloss.dump(sl); }


} //end ns

#endif // TRKMATERIALONTRACK_MATERIALEFFECTSONTRACK_H
