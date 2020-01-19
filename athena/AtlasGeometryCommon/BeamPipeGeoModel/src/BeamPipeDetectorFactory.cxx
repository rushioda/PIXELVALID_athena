/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamPipeGeoModel/BeamPipeDetectorFactory.h"


#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  

#include "StoreGate/StoreGateSvc.h"


#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <iomanip>
#include <vector>


BeamPipeDetectorFactory::BeamPipeDetectorFactory(StoreGateSvc *detStore,
				       IRDBAccessSvc *pAccess)
  :m_detectorManager(0),
   m_materialManager(0),
   m_detectorStore(detStore),
   m_access(pAccess),
   m_centralRegionZMax(1500*CLHEP::mm)
{}

BeamPipeDetectorFactory::~BeamPipeDetectorFactory()
{
}

void BeamPipeDetectorFactory::create(GeoPhysVol *world)
{
  m_detectorManager=new BeamPipeDetectorManager();

  const StoredMaterialManager * materialManagerTmp = 0;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManagerTmp, std::string("MATERIALS"))) {
    return; 
  } 
  m_materialManager = materialManagerTmp;
  
  IRDBRecordset_ptr atlasMother = m_access->getRecordsetPtr("AtlasMother",m_versionTag,m_versionNode);
  IRDBRecordset_ptr bpipeGeneral = m_access->getRecordsetPtr("BPipeGeneral",m_versionTag,m_versionNode);
  IRDBRecordset_ptr bpipeEnvelope = m_access->getRecordsetPtr("BPipeEnvelope",m_versionTag,m_versionNode);
  IRDBRecordset_ptr bpipePosition = m_access->getRecordsetPtr("BPipePosition",m_versionTag,m_versionNode);

  // Get the materials  
  const GeoMaterial *air        = m_materialManager->getMaterial("std::Air");

  // Create beam pipe top level envelopes
  // It is split into 3 sections. 
  // left, right and central. This is to allow different truth scoring in the different regions.
  
  m_centralRegionZMax = 1500 * CLHEP::mm; // For backward compatibility in DB.
  if (bpipeGeneral->size() != 0) m_centralRegionZMax = (*bpipeGeneral)[0]->getDouble("CENTRALZMAX") * CLHEP::mm;

  EnvelopeShapes envelopes;

  if (bpipeEnvelope->size() != 0) {
    envelopes = makeEnvelope(bpipeEnvelope);
  } else {
    envelopes = makeEnvelopeOld(atlasMother);
  }

  GeoLogVol* lvMotherCentral = new GeoLogVol("BeamPipeCentral",envelopes.centralShape,air);
  GeoPhysVol* pvMotherCentral = new GeoPhysVol(lvMotherCentral);


  // The treetops need to have unique physical volumes so
  // we create pvMotherFwdMinus which is identical pvMotherFwdPlus. 
  GeoLogVol* lvMotherFwd = new GeoLogVol("BeamPipeFwd",envelopes.fwdShape,air);
  GeoPhysVol* pvMotherFwdPlus = new GeoPhysVol(lvMotherFwd);
  GeoPhysVol* pvMotherFwdMinus = new GeoPhysVol(lvMotherFwd);


  // Add sections
  addSections(pvMotherCentral, 0);
  addSections(pvMotherFwdPlus, 1);
  addSections(pvMotherFwdMinus, 1);

  // We have to give them all the name "BeamPipe"
  // This is the name used by HepVis viewer
  GeoNameTag *tag = new GeoNameTag("BeamPipe");

  double beamx = 0.0;
  double beamy = 0.0;
  double beamz = 0.0;
  if (bpipePosition->size() != 0) {
    beamx = (*bpipePosition)[0]->getDouble("POSX") * CLHEP::mm;
    beamy = (*bpipePosition)[0]->getDouble("POSY") * CLHEP::mm;
    beamz = (*bpipePosition)[0]->getDouble("POSZ") * CLHEP::mm;
  }

  // Only shift the central section

  // Central
  world->add(tag);
  world->add(new GeoTransform(HepGeom::Translate3D(beamx,beamy,beamz)));
  world->add(pvMotherCentral);
  m_detectorManager->addTreeTop(pvMotherCentral);                                                                  //

  // FwdPlus
  world->add(tag);
  world->add(pvMotherFwdPlus);
  m_detectorManager->addTreeTop(pvMotherFwdPlus);

  // FwdMinus
  world->add(tag);
  world->add(new GeoTransform(HepGeom::RotateY3D(180*CLHEP::degree)));
  world->add(pvMotherFwdMinus);
  m_detectorManager->addTreeTop(pvMotherFwdMinus);


}

void BeamPipeDetectorFactory::addSections(GeoPhysVol* parent, int region)
{

  IRDBRecordset_ptr bpipeSections = m_access->getRecordsetPtr("BPipeSections",m_versionTag,m_versionNode);  
  
  bool central = (region == 0); 

  // Sections 2 & 3 are placed in section 1. 
  // pvMotherSection will point to section 1.
  GeoPhysVol* pvMotherSection = 0;
  bool addToFirstSection = true;
  double rminSec1 = 0;
  double rmaxSec1 = 0;
  double zminSec1 = 0;
  double zmaxSec1 = 0;
  
  for (unsigned int itemp=0; itemp<bpipeSections->size(); itemp++)
  {
    std::string material = (*bpipeSections)[itemp]->getString("MATERIAL");
    double rMin1 = (*bpipeSections)[itemp]->getDouble("RMIN1") * CLHEP::mm;
    double rMax1 = (*bpipeSections)[itemp]->getDouble("RMAX1") * CLHEP::mm;
    double rMin2 = (*bpipeSections)[itemp]->getDouble("RMIN2") * CLHEP::mm;
    double rMax2 = (*bpipeSections)[itemp]->getDouble("RMAX2") * CLHEP::mm;
    double z = (*bpipeSections)[itemp]->getDouble("Z") * CLHEP::mm;
    double dZ = (*bpipeSections)[itemp]->getDouble("DZ") * CLHEP::mm;
    int secNum = (*bpipeSections)[itemp]->getInt("SECNUM");
      
    double zmin = z - dZ;
    double zmax = z + dZ;

    
    // Check if in central or fwd region or if it is split.
    // We assume it is left/right symmetric. 
    if (central) { 
      if (zmin > m_centralRegionZMax) continue;
      if (zmax > m_centralRegionZMax) zmax = m_centralRegionZMax;
      if (zmax < -m_centralRegionZMax) continue;
      if (zmin < -m_centralRegionZMax) zmin = -m_centralRegionZMax;
    } else {
      if (zmax < m_centralRegionZMax) continue; 
      if (zmin < m_centralRegionZMax) zmin = m_centralRegionZMax;
    }


    double znew  =  0.5 * (zmin + zmax);
    double dZnew  = 0.5 * (zmax - zmin);
      
    std::ostringstream os; 
    if (central) {
      os << "SectionC"; 
    } else {
      os << "SectionF"; 
    }
    os << std::setw(2) << std::setfill('0') << secNum;
    std::string name = os.str(); 

    //std::cout << "Adding section: " << name 
    //	      << " rmin1,rmin2,rmax1,rmax2,z,dz = "
    //	      << rMin1 << ", " 
    //	      << rMin2 << ", " 
    //	      << rMax1 << ", " 
    //	      << rMax2 << ", " 
    //	      << znew << ", " 
    //	      << dZnew << ", "
    //	      << material << std::endl;

    bool isTube = true;
    GeoShape* shape;
    if((rMin1==rMin2)&&(rMax1==rMax2)) {
      shape = new GeoTube(rMin1,rMax1,dZnew);
      isTube = true;
    } else {
      shape = new GeoCons(rMin1,rMin2,
			  rMax1,rMax2,
			  dZnew,
			  0*CLHEP::deg,360*CLHEP::deg);
      isTube = false;
    }

    const GeoMaterial* mat = m_materialManager->getMaterial(material);
    if (mat == 0) {
      // For backward compatibility - older geometry versions didn't specify the
      // material namespace
      // std::cout << "Material """ << material << """ not found. Trying  std::" << material << std::endl;
      mat = m_materialManager->getMaterial("std::"+material);
    }

    GeoLogVol* lvSection = new GeoLogVol(name,shape,mat);
    GeoPhysVol* pvSection = new GeoPhysVol(lvSection);
    
    // Determine if this is a geometry where the first section can act as the mother of the following
    // sections. The following sections are only added to this if their ave CLHEP::radius is within the CLHEP::radial
    // extent and their ave z is within the z extent. 
    // As soon as one section fails to meet this the latter sections are not considered.
    if(secNum==1) {
      pvMotherSection = pvSection;
      rminSec1 = rMin1;
      rmaxSec1 = rMax1;
      zminSec1 = zmin;
      zmaxSec1 = zmax;
    }

    if (addToFirstSection && secNum != 1) {
      double rAve = 0.5*(rMin1+rMax1);
      addToFirstSection = (znew > zminSec1 && znew < zmaxSec1 &&
			   rAve > rminSec1 && rAve < rmaxSec1);
    }
	     

    GeoTransform* tfSection = 0;
    if (znew != 0 && (secNum==1 || !addToFirstSection)) tfSection = new GeoTransform(HepGeom::TranslateZ3D(znew));
    GeoNameTag* ntSection = new GeoNameTag(name);

    if (addToFirstSection && secNum!=1) {
      if (!pvMotherSection) {
	std::cout << "Unexpected condition when building beam pipe." << std::endl;
      }
      //std::cout << "Placing section " << secNum << " in Section1" << std::endl;
      pvMotherSection->add(ntSection);
      pvMotherSection->add(pvSection);
    } else {
      //std::cout << "Placing section " << secNum << " in mother envelope" << std::endl;
      parent->add(ntSection);
      if (tfSection) parent->add(tfSection);
      parent->add(pvSection);
    }

    // Not needed, but just in case in the future we have +/- sections in central region
    if(central && z!=0.) {
      // add rotated section as well
      GeoTransform* tfSectionRot = 0;
      if (isTube) { 
	// No need for rotation.
	tfSectionRot = new GeoTransform(HepGeom::TranslateZ3D(-znew));
      } else {
	// For cone we need to rotate around Y axis.
	tfSectionRot = new GeoTransform(HepGeom::TranslateZ3D(-znew)*HepGeom::RotateY3D(180*CLHEP::deg));
      }
      parent->add(ntSection);
      parent->add(tfSectionRot);
      parent->add(pvSection);
    }
  }  
}

const BeamPipeDetectorManager * BeamPipeDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

void BeamPipeDetectorFactory::setTagNode(std::string tag, std::string node)
{
  m_versionTag = tag;
  m_versionNode = node;
}


BeamPipeDetectorFactory::EnvelopeShapes
BeamPipeDetectorFactory::makeEnvelope(IRDBRecordset_ptr bpipeEnvelope)
{
  EnvelopeShapes envelopes;

  std::vector<EnvelopeEntry> centralEntry;
  std::vector<EnvelopeEntry> fwdEntry;

  for (unsigned int i=0; i<bpipeEnvelope->size(); i++) {
    double z = (*bpipeEnvelope)[i]->getDouble("Z") * CLHEP::mm;
    double r = (*bpipeEnvelope)[i]->getDouble("R") * CLHEP::mm;
    EnvelopeEntry entry(z,r);
    if (z < m_centralRegionZMax) {
      centralEntry.push_back(entry);
    } else {
      fwdEntry.push_back(entry);
    }
  }

  double rFwd = 0;
  if (fwdEntry.size()) {
    rFwd = fwdEntry[0].r();    
  } else if (centralEntry.size()) {
    rFwd = centralEntry[0].r();    
  } else {
    std::cout << "Unexpected condition when building beam pipe." << std::endl;
  }
 
  // central
  if (centralEntry.size() == 0) {
    envelopes.centralShape  = new GeoTube(0, rFwd, m_centralRegionZMax);
  } else {
    // This case probably will never get used and is untested.
    GeoPcon* pcone = new GeoPcon(0, 360*CLHEP::deg);

    pcone->addPlane(-m_centralRegionZMax,0,rFwd);
    for (int i=centralEntry.size()-1; i>=0; i--) {  
      double z = centralEntry[i].z();
      double r = centralEntry[i].r();
      pcone->addPlane(-z,0,r);
    }
    for (unsigned int i=0; i<centralEntry.size(); i++) {  
      double z = centralEntry[i].z();
      double r = centralEntry[i].r();
      pcone->addPlane(z,0,r);
    }
    pcone->addPlane(m_centralRegionZMax,0,rFwd);
    envelopes.centralShape = pcone;
  }

  
  // forward
  {
    GeoPcon* pcone = new GeoPcon(0, 360*CLHEP::deg);
    pcone->addPlane(m_centralRegionZMax,0,rFwd);
    if (fwdEntry.size() == 0) { 
      // Unlikely case but for completeness
      // we make small fwd region if everything is in central region.
      pcone->addPlane(m_centralRegionZMax+0.1*CLHEP::mm,0,rFwd);
    }
    for (unsigned int i=0; i<fwdEntry.size(); i++) {  
      double z = fwdEntry[i].z();
      double r = fwdEntry[i].r();
      pcone->addPlane(z,0,r);
    }
    envelopes.fwdShape = pcone;
  }

  return envelopes;

}


BeamPipeDetectorFactory::EnvelopeShapes
BeamPipeDetectorFactory::makeEnvelopeOld(IRDBRecordset_ptr atlasMother)
{

  double iir = (*atlasMother)[0]->getDouble("IDETIR")*CLHEP::cm;
  double cir = (*atlasMother)[0]->getDouble("CALOIR")*CLHEP::cm;
  double mir = (*atlasMother)[0]->getDouble("MUONIR")*CLHEP::cm;
  double totlen = (*atlasMother)[0]->getDouble("ZMAX")*CLHEP::cm;
  double ilen = (*atlasMother)[0]->getDouble("IDETZMX")*CLHEP::cm;
  double clen = (*atlasMother)[0]->getDouble("CALOZMX")*CLHEP::cm;

  // Central Section.
  GeoTube * bpipeCentralShape = new GeoTube(0, iir, m_centralRegionZMax);

  // Left/Right section. We create this once (as the +ve z section) and
  // place th -ve section by doing a rotation.

  // Right section (+ve z)

  GeoPcon* bpipeEnvPcone = new GeoPcon(0, 360*CLHEP::deg);
  bpipeEnvPcone->addPlane(m_centralRegionZMax,0,iir);
  bpipeEnvPcone->addPlane(ilen,0,iir);
  bpipeEnvPcone->addPlane(ilen,0,cir); 
  bpipeEnvPcone->addPlane(clen,0,cir);
  bpipeEnvPcone->addPlane(clen,0,mir);
  bpipeEnvPcone->addPlane(totlen,0,mir);

  // These get returned.
  EnvelopeShapes envelopes;
  envelopes.centralShape = bpipeCentralShape;
  envelopes.fwdShape =  bpipeEnvPcone;
  
  return envelopes;
}    


