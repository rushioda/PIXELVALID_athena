/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDepthTool.h
 *
 * @brief Declaration of CaloDepthTool. Created by Claire Bourdarios, 25.10.2004
 *
 * $Id: CaloDepthTool.h,v 1.9 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODETDESCR_CALODEPTHTOOL_H
#define CALODETDESCR_CALODEPTHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

class CaloDetDescrManager;
class IMessageSvc;

#include "GeoPrimitives/GeoPrimitives.h"


/** 
 * @class CaloDepthTool

   @brief Implementation of the ICaloDepthTool interface.Given a Track direction, checks if it is 
   in the Calorimeter acceptance and provides various radii to be compared/used in the extrapolation.

   This tool extends the functionality of the old egammaUtils/egammaqdepth class 

   In all the methods, radius stands for r in the (cylindric) barrel, for z in the (disk) endcap.   

   The generic method radius() uses the default choosen by joboption, and if the result is 0 because
   the eta/phi direction is outside the Sample acceptance, returns the egparametrised depth as backup
   solution.

 */

class CaloDepthTool : public AthAlgTool

{
public:
 
  /** @brief Standard constructor
   */
  CaloDepthTool(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 
  /** @brief Destructor
   */
  ~CaloDepthTool();

  /** @brief Query interface
   */
  static const InterfaceID& interfaceID( ) ; 

  /** @brief Tool Initialize
   */
  StatusCode initialize();

  /** @brief Tool Finalize
   */
  StatusCode finalize();

public:

  /** @brief get radius

      Generic method to be used in client code : 
      the default choosen by joboption, and if the result is 0 because
      the eta/phi direction is outside the Sample acceptance, returns the 
      egparametrised depth as backup solution.

   */
  double radius(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module,  const bool barrel,
		const double eta, const double phi ) const;
  /** @brief get radius

      Generic method to be used in client code : 
      the default choosen by joboption, and if the result is 0 because
      the eta/phi direction is outside the Sample acceptance, returns the 
      egparametrised depth as backup solution.

   */
  double radius(const CaloCell_ID::CaloSample sample,const double eta, const double phi ) const;

  /** @brief How far are you (in eta) from a given sample or - if you are in - how far are you from the edge ? 

      This method will return the distance to the closest edge.
      If you are outside the sample volume, it will always be a positive number.
      If you are inside the sample volume, the distance will be multiplied by -1 : i.e.always a negative number.
  */
  double deta (const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module,  const bool barrel,
		const double eta) const;
  /** @brief How far are you (in eta) from a given sample or - if you are in - how far are you from the edge ? 

      This method will return the distance to the closest edge.
      If you are outside the sample volume, it will always be a positive number.
      If you are inside the sample volume, the distance will be multiplied by -1 : i.e.always a negative number.
  */
  double deta (const CaloCell_ID::CaloSample sample,const double eta ) const;

  /** @brief Methods taken from the old egammaqdepth class : 
      return the parametrised depth used in DC1 and DC2. Only LAREM is foreseen 
   */
  double egparametrized(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module,  const bool barrel,
			const double eta, const double phi ) const;
  /** @brief Methods taken from the old egammaqdepth class : 
      return the parametrised depth used in DC1 and DC2. Only LAREM is foreseen 
   */
  double egparametrized(const CaloCell_ID::CaloSample sample,const double eta, const double phi ) const;

  /** @brief Parametrisation taken from the old egammaqdepth class, but adapted for TestBeam. 
      Only LAREM is foreseen 
   */
  double TBparametrized(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module,  const bool barrel,
				const double eta, const double phi ) const;
  /** @brief Parametrisation taken from the old egammaqdepth class, but adapted for TestBeam. 
      Only LAREM is foreseen 
   */
  double TBparametrized(const CaloCell_ID::CaloSample sample,const double eta, const double phi ) const;

  /** @brief Layers 1 and 2 depths set to give the best eta resolution.
   From CSC12 E=100 GeV photons.
   */
  double cscopt_parametrized(const CaloCell_ID::CaloSample sample,
                             const double eta, const double phi ) const;

  /** @brief Same as cscopt with the outermost EMB2 point excluded.
   This improves the fit.
   From CSC12 E=100 GeV photons.
   */
  double cscopt2_parametrized(const CaloCell_ID::CaloSample sample,
                             const double eta, const double phi ) const;

  /** @brief For a given direction, return the entrance of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double entrance(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module, const bool barrel, 
			  const double eta, const double phi ) const;
  /** @brief For a given direction, return the entrance of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double entrance(const CaloCell_ID::CaloSample sample, const double eta, const double phi ) const;

  /** @brief For a given direction, return the middle of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double middle(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module, const bool barrel,
			const double eta, const double phi ) const;
  /** @brief For a given direction, return the middle of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double middle(const CaloCell_ID::CaloSample sample, const double eta, const double phi ) const;

  /** @brief For a given direction, return the exit of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double exit(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module, const bool barrel,
			const double eta, const double phi ) const;
  /** @brief For a given direction, return the exit of the given CaloCell_ID::CaloSample.
      If the direction does not cross any CaloCell, the returned bool will be set to False.
   */
  double exit(const CaloCell_ID::CaloSample sample, const double eta, const double phi ) const;

  /**
      @brief If the direction is not specified, returns a "flat" aproximation (truely valid for strips only).
   */
  double flat(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module, const bool barrel,
		      const int side ) const;
  /**
      @brief If the direction is not specified, returns a "flat" aproximation (truely valid for strips only).
   */
  double flat(const CaloCell_ID::CaloSample sample, const int side ) const;

  /**
      @brief If the direction is not specified, returns a "flat" aproximation (truely valid for strips only).
    */
  double depth(const CaloCell_ID::SUBCALO subcalo,const int sampling_or_module, const bool barrel,
		      const int side ) const;
  /**
      @brief If the direction is not specified, returns a "flat" aproximation (truely valid for strips only).
    */
  double depth(const CaloCell_ID::CaloSample sample, const int side ) const;

  //      End of the general public methods : these are used behind the sceene and distinguish barrel/ec
  
  /** 
      @brief will look for the corresponding CaloDetDescrElement and return its entrance radius
    */
  double get_entrance_radius(CaloCell_ID::CaloSample sample,
			     double eta, double phi) const;
  
  /** @brief will look for the corresponding CaloDetDescrElement and return its entrance z
    */    
  double get_entrance_z(CaloCell_ID::CaloSample sample,
			double eta, double phi) const;
  
  /** @brief will look for the corresponding CaloDetDescrElement and return its middle radius
    */    
  double get_middle_radius(CaloCell_ID::CaloSample sample,
			   double eta, double phi) const;
  
  /** @brief will look for the corresponding CaloDetDescrElement and return its middle z
    */    
  double get_middle_z(CaloCell_ID::CaloSample sample,
		      double eta, double phi) const;
  
  /** @brief will look for the corresponding CaloDetDescrElement and return its middle radius
    */    
  double get_exit_radius(CaloCell_ID::CaloSample sample,
			 double eta, double phi) const;
  
  /** @brief will look for the corresponding CaloDetDescrElement and return its middle z
    */    
  double get_exit_z(CaloCell_ID::CaloSample sample,
		    double eta, double phi) const;
 protected:
   CaloDepthTool(); //> not implemented
   CaloDepthTool (const CaloDepthTool&); //> not implemented
   CaloDepthTool& operator= (const CaloDepthTool&); //> not implemented

private:

  std::string m_depth_choice;

  /** ID Helper
   */
  const CaloCell_ID* m_calo_id;

  /** DD manager
   */
  const CaloDetDescrManager* m_calo_dd;

  /** Phi range helper object
   */
  CaloPhiRange m_range;

  double m_default;

  /** Tool Service
   */
  IToolSvc* m_toolsvc;

  /** Mesaage Service
   */
  IMessageSvc*  m_msgSvc;
};

 
#endif // CALODETDESCR_CALODEPTHTOOL_H
