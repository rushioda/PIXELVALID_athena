/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// LArCalorimeter/LArDetDescr/LArRecoSimpleGeomTool.cxx
// -------------------------------------------------------------
// Copyright (C) 2005 by ATLAS Collaboration
//****************************************************************************

#include "LArDetDescr/LArRecoSimpleGeomTool.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <iostream>
#include <iomanip>

static const InterfaceID IID_LArRecoSimpleGeomTool("LArRecoSimpleGeomTool", 1, 0);

const InterfaceID& LArRecoSimpleGeomTool::interfaceID( ) 
{ return IID_LArRecoSimpleGeomTool; }

LArRecoSimpleGeomTool::LArRecoSimpleGeomTool(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent) :
  AthAlgTool(type, name, parent)
{
  declareInterface<LArRecoSimpleGeomTool>( this );
}

StatusCode
LArRecoSimpleGeomTool::initialize()
{
  // Retrieve the Id helpers needed    
  const CaloIdManager* mgr = CaloIdManager::instance();
  m_calo_id = mgr->getCaloCell_ID();

  
  // Decode tag ( via GeoModel ) and fix it for TestBeam :
  m_tag = "ATLAS-00";
  m_node = "ATLAS";
  m_geoModelSvc  = 0;

  StatusCode status = this->service("GeoModelSvc",m_geoModelSvc);
  if (status.isFailure()) {
    ATH_MSG_ERROR ("Unable to get pointer to GeoModel service");
    return status;
  }
  else {
    DecodeVersionKey detectorKey = DecodeVersionKey(m_geoModelSvc, "LAr");
    ATH_MSG_INFO ("DecodeVersionKey found : " << detectorKey.tag()
                  << " " << detectorKey.tag());
    if ( detectorKey.tag() != "LAr-H8-00" &&  detectorKey.tag() != "LAr-H6-00"
	 && detectorKey.tag() != "LAr-G3-00")
      {
	m_tag =  detectorKey.tag();
	m_node =  detectorKey.node();
      }
  }  
  ATH_MSG_INFO ("LAr simplified geometry will use : " << m_tag << " " << m_node);
    
  // Acess the DB service :    
  status = this->service("RDBAccessSvc",m_iAccessSvc);
  if (status.isFailure()) 
    {
      ATH_MSG_ERROR ("Unable to get RDBAccessSvc.");
      return status;
    }
  else
    ATH_MSG_INFO (" did access RDBAccessSvc ");

  m_iAccessSvc->connect();

  m_recCryoCyl = m_iAccessSvc->getRecordset("CryoCylinders",m_tag,m_node);
  if ( m_recCryoCyl && m_recCryoCyl->size()==0)
    m_recCryoCyl = m_iAccessSvc->getRecordset("CryoCylinders","CryoCylinders-00");

  m_recPresGeo = m_iAccessSvc->getRecordset("PresamplerGeometry",m_tag,m_node);
  if ( m_recPresGeo && m_recPresGeo->size()==0)
    m_recPresGeo = m_iAccessSvc->getRecordset("PresamplerGeometry","PresamplerGeometry-00");

  m_recBarrGeo = m_iAccessSvc->getRecordset("BarrelGeometry",m_tag,m_node);
  if ( m_recBarrGeo && m_recBarrGeo->size()==0)
    m_recBarrGeo = m_iAccessSvc->getRecordset("BarrelGeometry","BarrelGeometry-00");

  if ( m_geometry == "ATLAS" ) {
    DecodeVersionKey detectorKeyAtl = DecodeVersionKey(m_geoModelSvc, "ATLAS");
    m_recPresPos = m_iAccessSvc->getRecordset("PresamplerPosition",detectorKeyAtl.tag(),detectorKeyAtl.node());
  }
  else {
    m_recPresPos = m_iAccessSvc->getRecordset("PresamplerPosition",m_tag,m_node);
  }

  m_EmecGeo = m_iAccessSvc->getRecordset("EmecGeometry",m_tag,m_node);

  m_HEC = m_iAccessSvc->getRecordset("HadronicEndcap",m_tag,m_node);
  if ( m_HEC && m_HEC->size()==0)
    m_HEC = m_iAccessSvc->getRecordset("HadronicEndcap","HadronicEndcap-00");

  m_iAccessSvc->disconnect();

  ATH_MSG_INFO (" LArRecoSimpleGeomTool successfully initialized ");
  return StatusCode::SUCCESS;
}

LArRecoSimpleGeomTool::~LArRecoSimpleGeomTool()
{
}

StatusCode
LArRecoSimpleGeomTool::finalize()
{
  return StatusCode::SUCCESS;
}
  
bool 
LArRecoSimpleGeomTool::get_cylinder_surface (CaloSubdetNames::ALIGNVOL alvol,
			     Amg::Transform3D& htrans,double& hphi,
			     std::vector<double>& radius,  
			     std::vector<double>& depth, 
			     std::vector<double>& hlength )
{

  double rad,dep,len;
  //double nb;

  StoredPhysVol* storedPV = 0;
  std::string key = map_av(alvol);
  if(detStore()->contains<StoredPhysVol>(key))
  {
    if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
    {
      return false;
    }
  }
    
  if (!storedPV) {
    return false;
  }

  GeoFullPhysVol* fullPV  = storedPV->getPhysVol();
  //const AMG::Transform3D trf_eigen=fullPV->get
  htrans =  Amg::CLHEPTransformToEigen(fullPV->getAbsoluteTransform())*   
    Amg::CLHEPTransformToEigen(fullPV->getDefAbsoluteTransform()).inverse();

  hphi = m_phi_range.twopi();

  radius.clear();
  depth.clear();
  hlength.clear();

  if ( alvol == CaloSubdetNames::SOLENOID ) {
    
    // DDDb : LAr / CryoCylinders / CryoMother nb 11 & 12 & 13
    m_lar = m_recCryoCyl;
    if ( !m_lar || m_lar->size()<14) return false;

    m_rec = (*m_lar)[11];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    len =  m_rec->getDouble("DZ")*CLHEP::cm /2.;
    dep =  m_rec->getDouble("DR")*CLHEP::cm;
    m_rec = (*m_lar)[12];
    dep =  dep +m_rec->getDouble("DR")*CLHEP::cm;
    m_rec = (*m_lar)[13];
    dep =  dep +m_rec->getDouble("DR")*CLHEP::cm;

    //rad = 124.18*CLHEP::cm;
    //dep = (.305 + 1.38 + .47 )*CLHEP::cm;
    //len = 270.*CLHEP::cm;
    
    radius.push_back( rad + dep/2.);
    depth.push_back( dep/2. );
    hlength.push_back( len );

    return true;
  }
  else if ( alvol == CaloSubdetNames::LARCRYO_B ) {
    
    // DDDb : LAr / CryoCylinders
	
    m_lar = m_recCryoCyl;
    if ( !m_lar || m_lar->size()<15) return false;

    //  CryoMother nb 10
    m_rec = (*m_lar)[10];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    len =  m_rec->getDouble("DZ")*CLHEP::cm /2.;
    dep =  m_rec->getDouble("DR")*CLHEP::cm;

    //rad = 122.9*CLHEP::cm;
    //dep = 1.28*CLHEP::cm;
    //len = 270.*CLHEP::cm;

    radius.push_back( rad + dep/2. );
    depth.push_back( dep /2.);
    hlength.push_back( len );

    //  CryoMother nb 14
    m_rec = (*m_lar)[14];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    len =  m_rec->getDouble("DZ")*CLHEP::cm /2.;
    dep =  m_rec->getDouble("DR")*CLHEP::cm;

    //rad = 126.335*CLHEP::cm;
    //dep = 1.2*CLHEP::cm;
    //len = 284.*CLHEP::cm;

    radius.push_back( rad  + dep/2.);
    depth.push_back( dep /2.);
    hlength.push_back( len );

    // CryoMother nb 0
    m_rec = (*m_lar)[0];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    len =  m_rec->getDouble("DZ")*CLHEP::cm /2.;
    dep =  m_rec->getDouble("DR")*CLHEP::cm;

    //rad = 2140*mm;
    //dep = 30*mm;
    //len = 299.6*CLHEP::cm;

    radius.push_back( rad + dep/2. );
    depth.push_back( dep /2.);
    hlength.push_back( len );

    //  CryoMother nb 5
    m_rec = (*m_lar)[5];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    len =  m_rec->getDouble("DZ")*CLHEP::cm /2.;
    dep =  m_rec->getDouble("DR")*CLHEP::cm;

    //rad = 2220*mm;
    //dep = 30*mm;
    //len = 285*CLHEP::cm;

    radius.push_back( rad + dep/2. );
    depth.push_back( dep /2.);
    hlength.push_back( len );

    return true;
  }
  else if ( alvol == CaloSubdetNames::PRESAMPLER_B_POS || 
	    alvol == CaloSubdetNames::PRESAMPLER_B_NEG ) {
    
    // DDDb : LAr / PresamplerGeometry / rmin, rmax
    //        LAr / BarrelGeometry / zmax
    m_lar = m_recPresGeo;
    if ( !m_lar || m_lar->size()==0) return false;
    
    m_rec = (*m_lar)[0];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    dep =  m_rec->getDouble("RMAX")*CLHEP::cm - rad;
	
    m_lar = m_recBarrGeo;
    if ( !m_lar || m_lar->size()==0) return false;

    m_rec = (*m_lar)[0];
    len =  m_rec->getDouble("ZMAX")*CLHEP::cm;

    //rad = 138.5*CLHEP::cm;
    //dep = (144.7 - 138.5)*CLHEP::cm;
    //len = 316.5*CLHEP::cm;

    radius.push_back( rad  + dep/2.);
    depth.push_back( dep /2.);
    hlength.push_back( len );

    return true;

  }
  else if ( alvol == CaloSubdetNames::EMB_POS || alvol == CaloSubdetNames::EMB_NEG ) {

    // DDDb : LAr / BarrelGeometry / rmin, rmax, zmax
    
    m_lar = m_recBarrGeo;
    if ( !m_lar || m_lar->size()==0) return false;

    m_rec = (*m_lar)[0];
    rad =  m_rec->getDouble("RMIN")*CLHEP::cm;
    dep =  m_rec->getDouble("RMAX")*CLHEP::cm - rad;
    len =  m_rec->getDouble("ZMAX")*CLHEP::cm;

    //rad = 1447.3*CLHEP::cm;
    //dep = (2003.35 - 1447.3)*CLHEP::cm;
    //len = 316.5*CLHEP::cm;

    radius.push_back( rad  + dep/2.);
    depth.push_back( dep /2.);
    hlength.push_back( len );

    return true;
  }
	
  return false;
}
  
bool 
LArRecoSimpleGeomTool::get_disk_surface (CaloSubdetNames::ALIGNVOL alvol,
			 Amg::Transform3D& htrans,double& hphi, 
			 std::vector<double>& z, 
			 std::vector<double>& depth,
			 std::vector<double>& rmin, std::vector<double>& rmax )
{

  // Tracking geometry convention : z is the center of the layer, it is signed 
  //                                and "depth" is actually the half depth
  StoredPhysVol* storedPV = 0;
  std::string key = map_av(alvol);
  if(detStore()->contains<StoredPhysVol>(key))
  {
    if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
    {
      return false;
    }
  }
    
  if (!storedPV) {
    return false;
  }

  GeoFullPhysVol* fullPV  = storedPV->getPhysVol();
  //htrans = fullPV->getAbsoluteTransform()*fullPV->getDefAbsoluteTransform().inverse();
  htrans =  Amg::CLHEPTransformToEigen(fullPV->getAbsoluteTransform())*
    Amg::CLHEPTransformToEigen(fullPV->getDefAbsoluteTransform()).inverse();


  hphi = m_phi_range.twopi();
  z.clear();
  depth.clear();
  rmin.clear();
  rmax.clear();
  double nb;
  double ri,ra,dep,zcent;

  if ( alvol == CaloSubdetNames::LARCRYO_EC_POS || 
       alvol == CaloSubdetNames::LARCRYO_EC_NEG ) {
     
   // DDDb : LAr / CryoCylinders / Endcap nb 11

    m_lar = m_recCryoCyl;
    if ( !m_lar || m_lar->size()<60) return false;

    m_rec = (*m_lar)[49];

    ri = m_rec->getDouble("RMIN")*CLHEP::cm;
    ra = ri + m_rec->getDouble("DR")*CLHEP::cm;
    dep = m_rec->getDouble("DZ")*CLHEP::cm;
    zcent = m_rec->getDouble("ZMIN")*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::LARCRYO_EC_NEG) zcent = -1. * zcent;

    //ri = 22.1*CLHEP::cm;
    //ra = (22.1 + 194.4)*CLHEP::cm;
    //dep = 6.5*CLHEP::cm;
    //zcent = (356.1 + dep/2.)*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );
    
    // DDDb : LAr / CryoCylinders / Endcap nb 6
    m_rec = (*m_lar)[44];

    ri = m_rec->getDouble("RMIN")*CLHEP::cm;
    ra = ri + m_rec->getDouble("DR")*CLHEP::cm;
    dep = m_rec->getDouble("DZ")*CLHEP::cm;
    zcent = m_rec->getDouble("ZMIN")*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::LARCRYO_EC_NEG) zcent = -1. * zcent;

    //ri = 79.*CLHEP::cm;
    //ra = (ri + 173.)*CLHEP::cm;
    //dep = 6.*CLHEP::cm;
    //zcent = (660.5 + dep/2.)*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );

    return true;
  }

  if ( alvol == CaloSubdetNames::PRESAMPLER_EC_POS || 
       alvol == CaloSubdetNames::PRESAMPLER_EC_NEG ) {
    
    // DDDb Cryostat / PresamplerPosition
    m_lar = m_recPresPos;
    if (!m_lar) return false;

    m_rec = (*m_lar)[0];

    ri = m_rec->getDouble("RMIN")*CLHEP::cm;
    ra = m_rec->getDouble("RMAX")*CLHEP::cm;
    dep = m_rec->getDouble("TCK")*CLHEP::cm;
    zcent = m_rec->getDouble("ZPOS")*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::PRESAMPLER_EC_NEG) zcent = -1. * zcent;

    //ri = 123.174*CLHEP::cm;
    //ra = 170.2*CLHEP::cm;
    //dep = 0.4*CLHEP::cm;
    //zcent = (362.4 + dep/2.)*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );

    return true;
  }

  if ( alvol == CaloSubdetNames::EMEC_POS || 
       alvol == CaloSubdetNames::EMEC_NEG ) {
    
    // handcoded copy of dddb : LAr / EmecGeometry / z1, etot, rmin, rmax
    m_lar = m_EmecGeo;
    if ( !m_lar ) return false;

    m_rec = (*m_lar)[0];

    ri = m_rec->getDouble("RMIN")*CLHEP::cm;
    ra = m_rec->getDouble("RMAX")*CLHEP::cm;
    dep = m_rec->getDouble("ETOT")*CLHEP::cm;
    zcent = m_rec->getDouble("Z1")*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::EMEC_NEG) zcent = -1. * zcent;

    //ri = 29.*CLHEP::cm;
    //ra = 210.*CLHEP::cm;
    //dep = 53.6*CLHEP::cm;
    //zcent = (369.1 + dep/2.)*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );

    return true;
  }

  if ( alvol == CaloSubdetNames::HEC1_POS || alvol == CaloSubdetNames::HEC1_NEG ) {
    
    // DDDb numbers : LAr / HadronicEndcap / zstart, roorig, rend
    //                + for depth, see LArNumberHelper
    m_lar = m_HEC;
    if ( !m_lar || m_lar->size()==0) return false;

    m_rec = (*m_lar)[0];

    ri = m_rec->getDouble("ROORIG")*CLHEP::cm;
    ra = m_rec->getDouble("REND")*CLHEP::cm;
    // Block0+Block1+Block2
    dep = m_rec->getDouble("PLATE_0")/2. 
      + 3*8*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_0"));
    dep = dep*CLHEP::cm;
    zcent = m_rec->getDouble("ZSTART")*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::HEC1_NEG) zcent = -1. * zcent;

    //ri = 37.2*CLHEP::cm;
    //ra = 213.0*CLHEP::cm;
    //dep = (513.4 - 4.05 - 427.7)*CLHEP::cm;
    //zcent = 427.7*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );

    return true;
  }

  if ( alvol == CaloSubdetNames::HEC2_POS || alvol == CaloSubdetNames::HEC2_NEG ) {
    
    // DDDb numbers : LAr / HadronicEndcap / zstart, roorig, rend
    //                + for depth, see LArNumberHelper

    m_lar = m_HEC;
    if ( !m_lar || m_lar->size()==0) return false;

    m_rec = (*m_lar)[0];

    ri = m_rec->getDouble("ROORIG")*CLHEP::cm;
    ra = m_rec->getDouble("REND")*CLHEP::cm;
    // Block 3 + Block 4 + Block 5 + Block 6
    dep =  m_rec->getDouble("PLATE_1")/2. 
      + 4*4*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_1"));
    dep = dep*CLHEP::cm;
    // start+depth of HEC1 + gap
    zcent =  m_rec->getDouble("ZSTART") 
      + m_rec->getDouble("PLATE_0")/2. 
      + 3*8*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_0"))
      +  m_rec->getDouble("GAPWHL") ;
    zcent = zcent*CLHEP::cm + dep/2.;
    if (alvol == CaloSubdetNames::HEC2_NEG) zcent = -1. * zcent;

    //ri = 37.2*CLHEP::cm;
    //ra = 213.0*CLHEP::cm;
    //dep = (609.5 - 513.4)*CLHEP::cm;
    //zcent = (513.4 + dep/2.)*CLHEP::cm;

    rmin.push_back( ri );
    rmax.push_back( ra );
    depth.push_back( dep/2. );
    z.push_back( zcent );

    return true;
  }

  if ( alvol == CaloSubdetNames::FCAL1_POS || alvol == CaloSubdetNames::FCAL1_NEG ) {
    
    // see LArNumberHelper

    nb = 8.6*CLHEP::cm;
    rmin.push_back( nb );
    nb = 47.5*CLHEP::cm;
    rmax.push_back( nb );

    nb = 45.*CLHEP::cm;
    depth.push_back( nb/2. );

    nb = (466.85 + nb/2. )*CLHEP::cm;
    if (alvol == CaloSubdetNames::FCAL1_NEG) nb = -1. * nb;
    z.push_back( nb );

    return true;
  }

  if ( alvol == CaloSubdetNames::FCAL2_POS || alvol == CaloSubdetNames::FCAL2_NEG ) {
    
    // see LArNumberHelper

    nb = 8.6*CLHEP::cm;
    rmin.push_back( nb );
    nb = 47.5*CLHEP::cm;
    rmax.push_back( nb );

    nb = 45.*CLHEP::cm;
    depth.push_back( nb/2. );

    nb = (512.3 + nb/2. )*CLHEP::cm;
    if (alvol == CaloSubdetNames::FCAL2_NEG) nb = -1. * nb;
    z.push_back( nb );

    return true;
  }

  if ( alvol == CaloSubdetNames::FCAL3_POS || alvol == CaloSubdetNames::FCAL3_NEG ) {
    
    // see LArNumberHelper

    nb = 8.6*CLHEP::cm;
    rmin.push_back( nb );
    nb = 47.5*CLHEP::cm;
    rmax.push_back( nb );

    nb = 45.*CLHEP::cm;
    depth.push_back( nb/2. );

    nb = (559.75 + nb/2. )*CLHEP::cm;
    if (alvol == CaloSubdetNames::FCAL3_NEG) nb = -1. * nb;
    z.push_back( nb );

    return true;
  }


  return false;
} 
   
void 
LArRecoSimpleGeomTool::print()
{}
  
bool  
LArRecoSimpleGeomTool::ScanBarrelCryo(CaloSubdetNames::ALIGNVOL alvol,
				      Amg::Transform3D& htrans,double& hphi,
				      std::vector<double>& radius,  
				      std::vector<double>& depth, 
				      std::vector<double>& hlength )
{
  StoredPhysVol* storedPV = 0;
  std::string key = map_av(alvol);
  if(detStore()->contains<StoredPhysVol>(key))
  {
    if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
    {
      return false;
    }
  }
    
  if (!storedPV) {
    return false;
  }

  GeoFullPhysVol* fullPV  = storedPV->getPhysVol();
  htrans =  Amg::CLHEPTransformToEigen(fullPV->getAbsoluteTransform())*
    Amg::CLHEPTransformToEigen(fullPV->getDefAbsoluteTransform()).inverse();

  //htrans = fullPV->getAbsoluteTransform()*fullPV->getDefAbsoluteTransform().inverse();

  hphi = m_phi_range.twopi();
  radius.clear();
  depth.clear();
  hlength.clear();

  return true;
}
  
bool  
LArRecoSimpleGeomTool::ScanEMB(CaloSubdetNames::ALIGNVOL alvol,
			       Amg::Transform3D& htrans,double& hphi,
			       std::vector<double>& radius,  
			       std::vector<double>& depth, 
			       std::vector<double>& hlength )
{
  StoredPhysVol* storedPV = 0;
  std::string key = map_av(alvol);
  if(detStore()->contains<StoredPhysVol>(key))
  {
    if(detStore()->retrieve(storedPV,key)==StatusCode::FAILURE)
    {
      return false;
    }
  }
    
  if (!storedPV) {
    return false;
  }

  GeoFullPhysVol* fullPV  = storedPV->getPhysVol();
  //htrans = fullPV->getAbsoluteTransform()*fullPV->getDefAbsoluteTransform().inverse();
  htrans =  Amg::CLHEPTransformToEigen(fullPV->getAbsoluteTransform())*
    Amg::CLHEPTransformToEigen(fullPV->getDefAbsoluteTransform()).inverse();

  hphi = m_phi_range.twopi();
  radius.clear();
  depth.clear();
  hlength.clear();

  return true;
}

std::string 
LArRecoSimpleGeomTool::map_av(CaloSubdetNames::ALIGNVOL alvol)
{
  switch(alvol)
  {
  case CaloSubdetNames::LARCRYO_B:
    return std::string("LARCRYO_B");
  case CaloSubdetNames::LARCRYO_EC_POS:
    return std::string("LARCRYO_EC_POS");
  case CaloSubdetNames::LARCRYO_EC_NEG:
    return std::string("LARCRYO_EC_NEG");
  case CaloSubdetNames::SOLENOID:
    return std::string("SOLENOID");
  case CaloSubdetNames::PRESAMPLER_B_POS:
    return std::string("PRESAMPLER_B_POS");
  case CaloSubdetNames::PRESAMPLER_B_NEG:
    return std::string("PRESAMPLER_B_NEG");
  case CaloSubdetNames::EMB_POS:
    return std::string("EMB_POS");
  case CaloSubdetNames::EMB_NEG:
    return std::string("EMB_NEG");
  case CaloSubdetNames::PRESAMPLER_EC_POS:
    return std::string("PRESAMPLER_EC_POS");
  case CaloSubdetNames::PRESAMPLER_EC_NEG:
    return std::string("PRESAMPLER_EC_NEG");
  case CaloSubdetNames::EMEC_POS:
    return std::string("EMEC_POS");
  case CaloSubdetNames::EMEC_NEG:
    return std::string("EMEC_NEG");
  case CaloSubdetNames::HEC1_POS:
    return std::string("HEC1_POS");
  case CaloSubdetNames::HEC1_NEG:
    return std::string("HEC1_NEG");
  case CaloSubdetNames::HEC2_POS:
    return std::string("HEC2_POS");
  case CaloSubdetNames::HEC2_NEG:
    return std::string("HEC2_NEG");
  case CaloSubdetNames::FCAL1_POS:
    return std::string("FCAL1_POS");
  case CaloSubdetNames::FCAL1_NEG:
    return std::string("FCAL1_NEG");
  case CaloSubdetNames::FCAL2_POS:
    return std::string("FCAL2_POS");
  case CaloSubdetNames::FCAL2_NEG:
    return std::string("FCAL2_NEG");
  case CaloSubdetNames::FCAL3_POS:
    return std::string("FCAL3_POS");
  case CaloSubdetNames::FCAL3_NEG:
    return std::string("FCAL3_NEG");
  default:
    return std::string("");
  }
}
