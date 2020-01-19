/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/SurfaceCache.h"
#include "InDetReadoutGeometry/TRT_Conditions.h"

#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "CLHEP/Geometry/Transform3D.h"

#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscSurface.h"

#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_ConditionsData/StrawDxContainer.h"


namespace InDetDD {

    TRT_EndcapElement::TRT_EndcapElement(const GeoVFullPhysVol *volume, 
                                         const TRT_EndcapDescriptor *descriptor, 
                                         bool isPositive, 
                                         unsigned int wheelIndex, 
                                         unsigned int strawLayIndex, 
                                         unsigned int phiIndex,
                                         const TRT_ID * idHelper,
                                         const TRT_Conditions * conditions) :
        TRT_BaseElement(volume,
        idHelper->layer_id((isPositive ? 2:-2), phiIndex, wheelIndex, strawLayIndex),
        idHelper, conditions),
        _code(isPositive,wheelIndex,strawLayIndex,phiIndex),
        _descriptor(descriptor),
        _nextInZ(NULL),
        _previousInZ(NULL),
        m_surface(0)

    {
        _descriptor->ref();
    }


    TRT_EndcapElement::~TRT_EndcapElement()
    {
        _descriptor->unref();
        delete m_surface;
    }

    unsigned int TRT_EndcapElement::nStraws() const
    {
        return _descriptor->nStraws();
    }

    const double & TRT_EndcapElement::strawLength() const
    {
        return _descriptor->strawLength();
    }  


    const TRT_EndcapConditions * TRT_EndcapElement::getConditionsData() const
    {
        return NULL;
    }

    const TRT_EndcapDescriptor * TRT_EndcapElement::getDescriptor() const
    {
        return _descriptor;
    }

    void TRT_EndcapElement::setNextInZ(const TRT_EndcapElement *element)
    {
        _nextInZ=element;
    }

    void TRT_EndcapElement::setPreviousInZ(const TRT_EndcapElement *element)
    {
        _previousInZ=element;
    }


    HepGeom::Transform3D TRT_EndcapElement::calculateStrawTransform(int straw) const 
    {
        // NB The tranformation to a straw is reconstructed here precisely as
        // it was ... hopefully... in the factory.  One could eliminate this 
        // requirement and make the code a little more robust in this regard but
        // at the cost of doubling the descriptors.  (One descriptor now suffices
        // for both positive and negative endcaps).
        const GeoXF::Function *f= _descriptor->getStrawTransform();

        if (f) {
            int istraw = _code.isPosZ() ? straw : _descriptor->nStraws()-1 - straw;

            size_t offsetInto = _descriptor->getStrawTransformOffset();

            return getMaterialGeom()->getAbsoluteTransform()*((*f)(istraw+offsetInto)) * calculateLocalStrawTransform(straw);
            ////return conditions()->solenoidFrame() 
            ////  * getMaterialGeom()->getAbsoluteTransform()*((*f)(istraw+offsetInto));

        } else {

            // Will not work properly with alignments.
            std::cout << "ALTERNATIVE METHOD" << std::endl;

            double phi = _descriptor->startPhi() +  _descriptor->strawPitch() * straw;
            double r = _descriptor->innerRadius() + 0.5 * _descriptor->strawLength() ;
            CLHEP::Hep3Vector pos(r*cos(phi), r*sin(phi), (getMaterialGeom()->getAbsoluteTransform()*HepGeom::Point3D<double>()).z());
            CLHEP::HepRotation rot;
            // Axis (in local (0,0,1)) points towards beam axis.
            rot.rotateY(-0.5*M_PI); // Make it point along -ve X.
            rot.rotateZ(phi);
            return HepGeom::Transform3D(rot, pos);
        }

    }

    // The L3 Alignment 
    HepGeom::Transform3D TRT_EndcapElement::calculateLocalStrawTransform(int straw) const
    {
        const TRTCond::StrawDxContainer* container = conditions()->dxContainer();
        HepGeom::Transform3D rc ;
        if (container) {

            // important note: dx1 moves the 'negative' wire endpoint end dx2
            // the 'positive' wire endpoint in the local straw frame. 
            // In the global frame, 'dx1' corresponds to the readout side and 'dx2'
            // to the side closest the beampipe.

            int bec = getCode().isPosZ() ? +2 : -2 ;
            int wheel = getCode().getWheelIndex();
            int phimodule = getCode().getPhiIndex();
            int strawlayer = getCode().getStrawLayerIndex();
            TRTCond::ExpandedIdentifier id = TRTCond::ExpandedIdentifier(bec
                ,wheel
                ,phimodule
                ,strawlayer
                ,straw
                ,TRTCond::ExpandedIdentifier::STRAW);

            double dx1 = container->getDx1(id);
            double dx2 = container->getDx2(id);
            double ang = (dx2-dx1)/strawLength();
            double dy = -1*(dx2+dx1)/2.;

            // In the local straw frame:
            //   - the z-axis is along the straw and points toward the beampipe
            //   - the x-axis is along global-z and away from the interaction point 
            //          (locX = globZ A-side / locX = -1 *gobZ C-side) 
            //   - the y-axis is along global phi_hat direction determined by the other 2. 
            //          (clockwise C-side, counter clockwise A-Side)
            rc = HepGeom::TranslateY3D(dy)*HepGeom::RotateX3D(ang) ;
        }
        return rc ;
    }

    HepGeom::Transform3D TRT_EndcapElement::defStrawTransform(int straw) const 
    {
        // Same as calculateStrawTransform, except we use getDefAbsoluteTransform()
        // rather than  getAbsoluteTransform()
        
        // NB The tranformation to a straw is reconstructed here precisely as
        // it was ... hopefully... in the factory.  One could eliminate this 
        // requirement and make the code a little more robust in this regard but
        // at the cost of doubling the descriptors.  (One descriptor now suffices
        // for both positive and negative endcaps).

        const GeoXF::Function *f= _descriptor->getStrawTransform();

        if (f) {

            int istraw = _code.isPosZ() ? straw : _descriptor->nStraws()-1 - straw;

            size_t offsetInto = _descriptor->getStrawTransformOffset();
            return getMaterialGeom()->getDefAbsoluteTransform()*((*f)(istraw+offsetInto));

        } else {

            double phi = _descriptor->startPhi() +  _descriptor->strawPitch() * straw;
            double r = _descriptor->innerRadius() + 0.5 * _descriptor->strawLength() ;
            CLHEP::Hep3Vector pos(r*cos(phi), r*sin(phi), (getMaterialGeom()->getDefAbsoluteTransform()*HepGeom::Point3D<double>()).z());
            CLHEP::HepRotation rot;
            rot.rotateY(-0.5*M_PI); // Make it point along -ve X.
            rot.rotateZ(phi);
            return HepGeom::Transform3D(rot, pos);
        }

    }

    const Trk::SurfaceBounds& TRT_EndcapElement::strawBounds() const
    {
        return _descriptor->strawBounds();
    }

    const Trk::Surface& TRT_EndcapElement::elementSurface() const 
    {
        if (!m_surface) m_surface = new Trk::DiscSurface(*this);
        return *m_surface;
    }

    void TRT_EndcapElement::createSurfaceCache() const
    {
        // Calculate the surface 
        double phiCenter = _descriptor->startPhi() +  _descriptor->strawPitch() * 0.5*(nStraws()-1);
        double phiHalfWidth = 0.5 * _descriptor->strawPitch() * nStraws();
        double rMin = _descriptor->innerRadius();
        double rMax = rMin +_descriptor->strawLength();

        // The transform of the endcap is a translation in z for no
        // misalignement. For the -ve endcap there is also a 180deg rotation
        // around the y axis. getAbsoluteTransform() will also include the
        // misalignment.  
        //
        // To get the transform of the element we have to first rotate
        // around z to the phi center of the element.  We want the local z
        // to point in the same direction for both endcaps (approximately
        // global z axis). For the negative endcap we therefore have to
        // rotate 180 CLHEP::deg around the Y axis.
        
        // We need to rotate to phi center before we apply the
        // misalignment. However, in the negative endcap the the phi
        // location is inverted (due to 180 rotation around y axis). This is
        // taken care of by the extra 180 CLHEP::deg rotation around Y that we do
        // to get the z axis pointing in the correct direction.

        Amg::Transform3D * transform = 0;
        if  (_code.isPosZ())
            transform = new Amg::Transform3D(Amg::CLHEPTransformToEigen( (getMaterialGeom()->getAbsoluteTransform() * HepGeom::RotateZ3D(phiCenter))));
        else
            transform = new Amg::Transform3D(Amg::CLHEPTransformToEigen( (getMaterialGeom()->getAbsoluteTransform() * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateZ3D(phiCenter))));

        // create the igredients and the cache
        Trk::DiscBounds* bounds = new Trk::DiscBounds(rMin, rMax, phiHalfWidth);
        Amg::Vector3D*  center = new Amg::Vector3D(transform->translation());
        Amg::Vector3D*  normal = new Amg::Vector3D(transform->rotation().col(2));
        m_surfaceCache = new SurfaceCache(transform, center, normal, bounds);
        // create the surface if needed 
        if (!m_surface) m_surface = new Trk::DiscSurface(*this);

    }



    int TRT_EndcapElement::strawDirection() const
    {
  // Return +1 if the straw local axis is in the same direction as increasing eta direction,
  //  -1 otherwise.
  // The straw axis by convention goes in the direction away from the readout. This is
  // towards the beam pipe. For +ve endcap it is what we want. For -ve endcap it is oppposite.
  // 
  // 
        return (_code.isPosZ()) ? +1 : -1;
    }

} // end namespace


