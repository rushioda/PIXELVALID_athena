/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DigitizationModule.h, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGEVENT_PLANARDIGITIZATIONMODULE_H
#define TRKDIGEVENT_PLANARDIGITIZATIONMODULE_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDigEvent/DigitizationCell.h"
#include "TrkDigEvent/Segmentation.h"
// STL
#include <memory>
#include <vector>

namespace Trk {

    class Surface;

    /** @class DigitizationModule
    
        Class that holds the surfaces for a planar digitization detector module.
        
        It needs a descriptor to design different pixel/strixels/strip setups (with a segmentation class)
    
        The digitizaiton is done in the local frame of the surface and binning can be done
        in local x and local y direction. 
    
        The lorentz angle is assumed to be only in x-direction and constant for the module, 
        it is measured from the local z-direction.
    
        The readout direction defines the charge drift either:
        a) towards the surface at -halfThickness if readout is defined at -1 
        b) towards the surface at +halfThickness if readout is defined at +1
    
        Conventions: 
          - 3D positions are within the 3D frame of the module 
          - 2D positions are corrected to parameter surface  at the center of the module (and not the readout surface) 
    
        The lorenzShift is the correction from the readout surface to the parameter surface
        
        @author Andreas.Salzburger -at- cern.ch 
    */
    
    class DigitizationModule { 
        public :
            /** Constructor from a Segmentation descriptor */
            DigitizationModule(std::shared_ptr<const Segmentation> moduleSegmentation,
                               double halfThickness,
                               int readoutDirection,
                               double lorentzAngle);

            /** Virtual Destructor */
            virtual ~DigitizationModule(){}
            
            /** Return the internal test segmentation surfaces to test between entry and exit given by their cell id's - the boundaries are not given */ 
            const std::vector< std::shared_ptr<const Surface> > segmentationSurfaces(const DigitizationCell& entryCids, const DigitizationCell& exitCids) const;
            
            /** Get the digitization cell fropm a position */
            const DigitizationCell cell(const Amg::Vector2D& position) const;
            
            /** module thickness */
            double halfThickness() const;
            
            /** return the readout direction */
            int readoutDirection() const;
            
            /** return the lorentz Angle */
            double lorentzAngle() const;
            
            /** return the segmenation */
            const Segmentation& segmentation() const;
            
            /** return the test surfaces between these points */
            const std::vector< std::shared_ptr<const Surface> > stepSurfaces(const Amg::Vector3D& start, const Amg::Vector3D& end) const;
            
            /** Fill the associated digitsation cell from this start and end position, correct for lorentz effect if needed */
            const DigitizationStep digitizationStep(const Amg::Vector3D& start, const Amg::Vector3D& end) const;
            
            /** return the bounding surfaces at top and bottom */
            const std::vector< std::shared_ptr<const Surface> >& boundarySurfaces() const;
            
            /** return all surfaces in X - excluding the boundaries */
            const std::vector< std::shared_ptr<const Surface> >& segmentationSurfacesX() const;
            
            /** return all surfaces in Y - excluding the boundaries */
            const std::vector< std::shared_ptr<const Surface> >& segmentationSurfacesY() const;
                        
        private:

            double                                         m_halfThickness;
            int                                            m_readoutDirection;      //!< defines if the readout is along (+1) / (-1) wrt the z axis
            double                                         m_lorentzAngle;          //!< the lorentz angle
            double                                         m_tanLorentzAngle;       //!< and the tangent of it
            
            std::shared_ptr<const Segmentation>            m_segmentation;          //!< segmentation descriptor            
            std::vector< std::shared_ptr<const Surface> >  m_boundarySurfaces;      //!< boundary surfaces, they are ordered: readout, opposite, x boundaries, y boundaries
            std::vector< std::shared_ptr<const Surface> >  m_segmentationSurfacesX; //!< segmentation surfaces in X - without boundaries
            std::vector< std::shared_ptr<const Surface> >  m_segmentationSurfacesY; //!< segmentation surfaces in Y - without boundaries
    
    };
    

    inline double DigitizationModule::halfThickness() const
        { return m_halfThickness; }
    
    inline int DigitizationModule::readoutDirection() const
        { return m_readoutDirection; }
    
    inline double DigitizationModule::lorentzAngle() const
        { return m_lorentzAngle; }
    
    inline const Segmentation& DigitizationModule::segmentation() const
        { return (*(m_segmentation.get())); }
    
    inline const std::vector< std::shared_ptr<const Surface> >& DigitizationModule::boundarySurfaces() const
        { return m_boundarySurfaces; }
    
    inline const std::vector< std::shared_ptr<const Surface> >& DigitizationModule::segmentationSurfacesX() const
        { return m_segmentationSurfacesX; }
    
    inline const std::vector< std::shared_ptr<const Surface> >& DigitizationModule::segmentationSurfacesY() const
        { return m_segmentationSurfacesY; }
    
    inline const DigitizationStep DigitizationModule::digitizationStep(const Amg::Vector3D& start, const Amg::Vector3D& end) const
        { return m_segmentation->digitizationStep(start,end,m_halfThickness,m_readoutDirection,m_lorentzAngle); }

}

#endif
