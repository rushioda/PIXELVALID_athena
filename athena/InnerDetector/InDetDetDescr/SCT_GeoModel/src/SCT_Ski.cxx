/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Ski.h"
#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_Module.h"
#include "SCT_GeoModel/SCT_BaseBoard.h" // 18:00 Wed 15th Jun 2005 D.Naito added.
#include "SCT_GeoModel/SCT_Dogleg.h"
#include "SCT_GeoModel/SCT_CoolingBlock.h" // 14th Aug 2005 S.Mima added.
#include "SCT_GeoModel/SCT_CoolingPipe.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include <sstream>
#include <cmath>

SCT_Ski::SCT_Ski(const std::string & name,
			     const SCT_Module * module,
			     int stereoSign,
		             double tilt,
 		             double length)
  : SCT_UniqueComponentFactory(name), 
    m_stereoSign(stereoSign),
    m_tilt(tilt), 
    m_length(length), 
    m_module(module)
{
  getParameters();
  m_logVolume = preBuild();
}


SCT_Ski::~SCT_Ski()
{
  delete m_dogleg;
  delete m_coolingBlock;
  delete m_coolingPipe;
  delete m_env1RefPointVector;
  delete m_env2RefPointVector;
}


void
SCT_Ski::getParameters()
{

  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();

  m_safety       = generalParameters->safety();
  
  m_radialSep     = parameters->skiRadialSep();
  m_firstStagger  = parameters->skiFirstStagger();
  m_modulesPerSki = parameters->modulesPerSki();
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++){
    m_zPos.push_back(parameters->skiZPosition(iModule));
    m_id.push_back(parameters->skiModuleIdentifier(iModule));
  }

  // Various offset for the doglegs, cooling blocks and cooling pipe.
  // These are not currently needed and are all set to zero.
  m_doglegRadialOffset = 0;
  m_doglegPhiOffset = 0;
  m_coolingBlockPhiOffset = 0;
  m_coolingBlockRadialOffset = 0;
  m_coolingPipeRadialOffset = 0;
  m_coolingPipePhiOffset = 0;

  // 10th Aug 2005 S.Mima modified.
  m_coolingBlockOffsetX = parameters->coolingBlockOffsetX();
  m_coolingBlockOffsetY = parameters->coolingBlockOffsetY();
  m_coolingBlockOffsetZ = parameters->coolingBlockOffsetZ();
  m_coolingPipeOffsetX = parameters->coolingPipeOffsetX();
  m_coolingPipeOffsetY = parameters->coolingPipeOffsetY();
  // 15th Aug 2005 S.Mima modified.
  m_doglegOffsetX = parameters->doglegOffsetX();
  m_doglegOffsetY = parameters->doglegOffsetY();
}

const GeoLogVol * 
SCT_Ski::preBuild()
{
  const double rphiClearance = 0.5*CLHEP::mm;
  const double radialClearance = 0.5*CLHEP::mm;


  // Make components.
  // 15th Aug 2005 S.Mima modified.
  //  m_dogleg = new SCT_Dogleg(getName()+"Dogleg");
  m_dogleg = new SCT_Dogleg(getName()+"Dogleg");
  // 15th Aug 2005 S.Mima modified.
  //  m_coolingBlock = new SCT_CoolingBlock(getName()+"CoolingBlock");
  m_coolingBlock = new SCT_CoolingBlock(getName()+"CoolingBlock");
  m_coolingPipe = new SCT_CoolingPipe(getName()+"CoolingPipe",m_length);

  // We need the sign of the tilt in numerous places
  int tiltSign = (m_tilt < 0) ? -1 : +1;


  // The two modules are lined up in x in the unrotated ski. The ski then gets tilted
  // when placed in the layer.
  // So the x offset is just the raidal separation and the y offset is zero.

  double xModuleOffset = 0.5 * m_radialSep;

  // *** 10:00 Mon 30th May 2005 D.Naito modified. (12)*********************************
  // I get what "yModuleOffset" has role. "yModuleOffset" must be zero.
  // I move the calculations of corners to lower area in this code(L336 ~ L367).
  // "yModuleOffset = -0.5*(moduleYMax + moduleYMin)" is wrong.

  // *** 18:00 Fri 27th May 2005 D.Naito added.    (09)*********************************

  // *** 18:00 Fri 27th May 2005 D.Naito put some comments.
  // I need to calculate moduleYMax and moduleYMin for yModuleOffset,
  // because the modules is asyCLHEP::mmetry in y direction.

  //
  // These are coordinates of corners of module's envelopes.
  //

  //CLHEP::Hep3Vector c0(0.0, 0.5*(m_module->env1Width()), 0.5*(m_module->env1Length()));
  //CLHEP::Hep3Vector c1(0.0, -0.5*(m_module->env1Width()), 0.5*(m_module->env1Length()));
  //CLHEP::Hep3Vector c2(0.0, -0.5*(m_module->env1Width()), -0.5*(m_module->env1Length()));
  //CLHEP::Hep3Vector c3(0.0, 0.5*(m_module->env1Width()), -0.5*(m_module->env1Length()));
  //CLHEP::Hep3Vector c4(0.0,
  //		-(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
  //		-(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  //CLHEP::Hep3Vector c5(0.0,
  //		-(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
  //		-(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  //CLHEP::Hep3Vector c6(0.0,
  //		-(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
  //		-(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));
  //CLHEP::Hep3Vector c7(0.0,
  //		-(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
  //		-(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));

  //double moduleYMax = c4.y();
  //double moduleYMin = c5.y();
  //  c0.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c1.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c2.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c3.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c4.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c5.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c6.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  //  c7.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());

  //  moduleYMax = std::max(std::max(c0.y(), c4.y()), std::max(c7.y(), c3.y()));
  //  moduleYMin = std::min(std::min(c1.y(), c5.y()), std::min(c6.y(), c2.y()));
  // *** End of added lines. --------------------- (09)*********************************


  // *** 18:00 Fri 27th May 2005 D.Naito modified. (10)*********************************
  //double yModuleOffset = 0;
  // *** -->>                                      (10)*********************************
  //double yModuleOffset = -0.5*(moduleYMax + moduleYMin);
  // *** End of modified lines. ------------------ (10)*********************************
  // *** -->>                                      (12)*********************************
  double yModuleOffset = 0.0;
  // *** End of modified lines. ------------------ (12)*********************************


  //std::cout << "xPos, yPos = " << xPos << " " << yPos << std::endl;
   
  //
  // Calculate position of cooling block
  //
  // Position the cooling block m_coolingBlockPhiOffset from the lower egde 
  // of the module and m_coolingBlockRadialOffset from surface of the module.
  // double  xCoolingBlockOffset = -(m_coolingBlockRadialOffset + 
  //				  0.5 * m_module->thickness() + 
  //				  0.5 * m_coolingBlock->thickness()) + epsilon();
  // 10th Aug 2005 S.Mima modified.
  double xCoolingBlockOffset = - m_coolingBlockRadialOffset + coolingBlockOffsetX();
  // Works for both tilts.
  // *** 18:00 Wed 15th Jun 2005 D.Naito modified. (00)*********************************
  // *** 14:00 Fri 27th May 2005 D.Naito modified. (03)*********************************
  //double  yCoolingBlockOffset = -tiltSign *(0.5 * m_module->baseBoard()->width() 
  //					    - 0.5 *  m_coolingBlock->width()
  //					    - m_coolingBlockPhiOffset );
  //double  zCoolingBlockOffset = m_module->baseBoardCenter();
  // *** -->>                                      (03)*********************************
  //double  yCoolingBlockOffset = tiltSign *(m_module->baseBoard()->bb1OffsetY() - 0.5*(m_module->baseBoard()->bb1()->width()) 
  //                                         + 0.5*m_coolingBlock->width() - m_coolingBlockPhiOffset);
  //double  zCoolingBlockOffset = m_module->baseBoard()->bb1OffsetZ() - 0.5*(m_module->baseBoard()->bb3()->length());
  // *** End of modified lines. ------------------ (03)*********************************
  // *** -->>                                      (00)*********************************
  //double  yCoolingBlockOffset = tiltSign *(m_module->baseBoardOffsetY() - 0.5*(m_module->baseBoard()->width()) 
  //                                         + 0.5*m_coolingBlock->width() - m_coolingBlockPhiOffset);
  // 10th Aug 2005 S.Mima modified.
  double yCoolingBlockOffset = tiltSign *(coolingBlockOffsetY() - m_coolingBlockPhiOffset);
  // double  zCoolingBlockOffset = m_module->baseBoardOffsetZ();
  // 10th Aug 2005 S.Mima modified.
  double zCoolingBlockOffset = coolingBlockOffsetZ(); 
  // *** End of modified lines. ------------------ (03)*********************************

  //
  // Calculate position of dogleg
  //
  // *** 17:00 Fri 27th May 2005 D.Naito modified. (06)*********************************
  //double xDoglegOffset = - (m_doglegRadialOffset + 0.5 *m_dogleg->thickness()
  //			    + 0.5 * m_module->thickness())
  // *** -->>                                      (06)*********************************
  // 15th Aug 2005 S.Mima modified.
  //  double xDoglegOffset = - (m_doglegRadialOffset + 0.5 *m_dogleg->thickness()
  //		    + 0.5 * m_module->env2Thickness() + epsilon());
  double xDoglegOffset = doglegOffsetX() - m_doglegRadialOffset; 
  // *** End of modified lines. ------------------ (06)*********************************

  // NB length of dogleg runs in phi direction.
  double yDoglegOffset =  tiltSign * (doglegOffsetY() + m_doglegPhiOffset);

  // 15th Aug 2005 S.Mima modified.
  //  double zDoglegOffset = m_module->baseBoardCenter();
  double zDoglegOffset = coolingBlockOffsetZ();

  //std::cout << "Dogleg offset: " << yDoglegOffset << std::endl;

  //
  // Calculate position of cooling pipe.
  //
  // First calculate minimum block position.
  //double xCoolingBlockPosMin = -xModuleOffset + xCoolingBlockOffset;
  //double yCoolingBlockPosMin = -yModuleOffset + yCoolingBlockOffset;
  //double yCoolingBlockPosMin =  yCoolingBlockOffset;
  // 10th Aug 2005 S.Mima modified. 
  //  double xCoolingPipePos = xCoolingBlockPosMin - (m_coolingPipeRadialOffset + 
  //						  0.5 * m_coolingBlock->thickness() + m_coolingPipe->pipeRadius());
  double xCoolingPipePos = coolingPipeOffsetX() - m_coolingPipeRadialOffset;
  //double yCoolingPipePos = yCoolingBlockPosMin + m_coolingPipePhiOffset;
  double yCoolingPipePos = coolingPipeOffsetY() + m_coolingPipePhiOffset; 
  m_coolingPipePos = new GeoTransform(HepGeom::Translate3D(xCoolingPipePos, yCoolingPipePos, 0));
 
  //
  //
  //

  int stagger_sign = m_firstStagger;
  // Loop over modules in ski as we need their z positions.
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
    
    // For compatibility with AGE we need to offset the module in y by -0.5*sep*sin(tilt) so that center 
    // of module lines up with radial line. (Not sure if this is what is done in reality)

    // Not sure if this needs to be alignable
    double xModulePos = stagger_sign * xModuleOffset;
    // *** 20:00 Fri 27th May 2005 D.Naito modified. (11)*********************************
    //double yModulePos = stagger_sign * yModuleOffset;
    // *** -->>                                      (11)*********************************
    double yModulePos = yModuleOffset;
    // *** End of modified lines. ------------------ (04)*********************************
    double zModulePos = m_zPos[iModule];
    CLHEP::HepRotation rot;

    // There is only one type of module and this is rotated one way or the other
    // to get the phi-v (-ve), u-phi (+ve) orientations
    rot.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
    CLHEP::Hep3Vector pos(xModulePos, yModulePos, zModulePos);
    m_modulePos.push_back(HepGeom::Transform3D(rot, pos));

    //
    // Calculate position of cooling block
    //

    double  xCoolingBlockPos = xCoolingBlockOffset + xModulePos;
    double  yCoolingBlockPos = yCoolingBlockOffset + yModulePos;
    double  zCoolingBlockPos = zCoolingBlockOffset + zModulePos;
    m_coolingBlockPos.push_back(new GeoTransform(HepGeom::Translate3D(xCoolingBlockPos, yCoolingBlockPos, zCoolingBlockPos))); 

    
    //
    // Calculate position of dogleg
    //
    double xDoglegPos = xDoglegOffset + xModulePos; 
    double yDoglegPos = yDoglegOffset + yModulePos; 
    double zDoglegPos = zDoglegOffset + zModulePos;
    m_doglegPos.push_back(new GeoTransform(HepGeom::Translate3D(xDoglegPos, yDoglegPos, zDoglegPos))); 

    // alternate staggering
    stagger_sign = - stagger_sign;	

   
  }

  //
  // calculate envelope for ski with just modules. Not all these are used.
  // These are coordinates of corners of module's envelopes.
  //

  CLHEP::Hep3Vector c0(0.0,
		-(m_module->env1RefPointVector()->y()) + 0.5*(m_module->env1Width()),
		-(m_module->env1RefPointVector()->z()) + 0.5*(m_module->env1Length()));
  CLHEP::Hep3Vector c1(0.0,
		-(m_module->env1RefPointVector()->y()) - 0.5*(m_module->env1Width()),
		-(m_module->env1RefPointVector()->z()) + 0.5*(m_module->env1Length()));
  CLHEP::Hep3Vector c2(0.0,
		-(m_module->env1RefPointVector()->y()) - 0.5*(m_module->env1Width()),
		-(m_module->env1RefPointVector()->z()) - 0.5*(m_module->env1Length()));
  CLHEP::Hep3Vector c3(0.0,
		-(m_module->env1RefPointVector()->y()) + 0.5*(m_module->env1Width()),
		-(m_module->env1RefPointVector()->z()) - 0.5*(m_module->env1Length()));
  CLHEP::Hep3Vector c4(0.0,
		-(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
		-(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  CLHEP::Hep3Vector c5(0.0,
		-(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
		-(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  CLHEP::Hep3Vector c6(0.0,
		-(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
		-(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));
  CLHEP::Hep3Vector c7(0.0,
		-(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
		-(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));

  double moduleYMax = c4.y();
  double moduleYMin = c5.y();
  c0.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c1.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c2.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c3.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c4.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c5.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c6.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  c7.rotateX(0.5 * m_stereoSign * m_module->stereoAngle());
  
  moduleYMax = std::max(std::max(c0.y(), c4.y()), std::max(c7.y(), c3.y()));
  moduleYMin = std::min(std::min(c1.y(), c5.y()), std::min(c6.y(), c2.y()));
  
  double skiWidth = moduleYMax - moduleYMin + 2*rphiClearance;

  //
  // 17:00 Fri 27th May 2005 D.Naito put some comments.
  // m_module->thickness() corresponds to the thickness of envelope2 of SCT_Module.
  // NB. The envelope is centered on x.
  //
  double skiThickness = m_module->thickness() + 2 * std::abs(xModuleOffset) + radialClearance;
  //double skiLength =    2.0 * std::max(std::abs(m_zPos[0]), std::abs(m_zPos[m_modulesPerSki - 1])) 
  //                         + m_module->length();
            
  // Due to the cooling block and dogleg being next to the module and
  // as the modules are staggered it is no longer possible to have a
  // simple envelope around the ski.

  // Calculate envelope needed. This is a boolean of two rectangles. 
  // 1. Contains the modules and dogleg.
  // 2. Contains the cooling pipe and cooling block.

  double xmax1 = 0.5*skiThickness;
  double xmin1 = -xModuleOffset + xDoglegOffset - 0.5*m_dogleg->thickness();
  double xmax2 = xmax1;  
  double xmin2 = xCoolingPipePos - m_coolingPipe->pipeRadius();

  // Take into account possiblity that the dogleg extends past the module.

  // *** 17:00 Fri 27th May 2005 D.Naito modified. (07)*********************************
  //double maxWidth =  std::max(std::max(std::abs(-yModuleOffset + yDoglegOffset) +  0.5*m_dogleg->length(),
  //				       std::abs(+yModuleOffset + yDoglegOffset) +  0.5*m_dogleg->length()),
  //			      0.5*skiWidth);
  //double ymax1 = maxWidth;
  //double ymin1 = -maxWidth;
  // *** -->>                                      (07)*********************************
  double ymax1 = moduleYMax + rphiClearance;
  double ymin1 = moduleYMin - rphiClearance;
  // *** End of modified lines. ------------------ (07)*********************************
  double ymin2,ymax2;
  if (tiltSign > 0) {
    ymax2 = std::max(-yModuleOffset + yCoolingBlockOffset + 0.5*m_coolingBlock->width(),
		     yCoolingPipePos + m_coolingPipe->pipeRadius());
    ymin2 = ymin1;
  } else {
    ymax2 = ymax1;
    ymin2 = std::min(-yModuleOffset + yCoolingBlockOffset - 0.5*m_coolingBlock->width(),
		     yCoolingPipePos - m_coolingPipe->pipeRadius());
  }
  

  // Add some safety
  xmin1 -= m_safety;
  xmax1 += m_safety;
  ymin1 -= m_safety;
  ymax1 += m_safety;
  //  std::cout << "xmin1,xmax1,ymin1,ymax1= " << xmin1 << " " << xmax1 << " " << ymin1 << " " << ymax1 << std::endl;

  xmin2 -= m_safety;
  xmax2 += m_safety;
  ymin2 -= m_safety;
  ymax2 += m_safety;
  //  std::cout << "xmin2,xmax2,ymin2,ymax2= " << xmin2 << " " << xmax2 << " " << ymin2 << " " << ymax2 << std::endl;

  double xCenter = 0.5*(xmin1+xmax1);
  double yCenter = 0.5*(ymin1+ymax1);
  double xShift2 = 0.5*(xmin2+xmax2) - xCenter;
  double yShift2 = 0.5*(ymin2+ymax2) - yCenter;
  //double xCoolingPipeShift = xCoolingPipePos - xCenter;
  //double yCoolingPipeShift = yCoolingPipePos - yCenter;

  m_refPointTransform = new GeoTransform(HepGeom::Translate3D(-xCenter, -yCenter, 0));
  //  std::cout << "xCenter, yCenter = " << xCenter << "  " << yCenter << std::endl;
  //  std::cout << "xShift2, yShift2 = " << xShift2 << "  " << yShift2 << std::endl;
  //  std::cout << "xCoolingPipePos, yCoolingPipePos = " << xCoolingPipePos << "  " << yCoolingPipePos << std::endl;

  // *** 10:00 Tue 31st May 2005 D.Naito modified. (14)*********************************
  // *** -->>                                      (14)*********************************
  m_env1RefPointVector = new CLHEP::Hep3Vector(-xCenter, -yCenter, 0.0);
  m_env2RefPointVector = new CLHEP::Hep3Vector(-xShift2, -yShift2, 0.0);
  m_env1Thickness      = xmax1-xmin1;
  m_env1Width          = ymax1-ymin1;
  m_env2Thickness      = xmax2-xmin2;
  m_env2Width          = ymax2-ymin2;
  // *** End of modified lines. ------------------ (14)*********************************

  GeoBox * envelope1 = new GeoBox(0.5 * (xmax1-xmin1), 0.5 * (ymax1-ymin1), 0.5 * m_length);
  GeoBox * envelope2 = new GeoBox(0.5 * (xmax2-xmin2), 0.5 * (ymax2-ymin2), 0.5 * m_length);

  const GeoShape * skiEnvelopeShape = 0;
    
  const GeoShape & tmpShape = (*envelope1).
    add(*envelope2 << HepGeom::Translate3D(xShift2, yShift2, 0));
  skiEnvelopeShape = &tmpShape;
  
  
  SCT_MaterialManager materials;    
  GeoLogVol * skiLog = new GeoLogVol(getName(), skiEnvelopeShape, materials.gasMaterial());

  //GeoPhysVol * ski = new GeoPhysVol(skiLog);

   // Make names once only so we don't recreate them again again.
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
    // Add identifier to name.
    std::ostringstream name;
    name << "Module#" << m_id[iModule];
    m_nameTag.push_back(new GeoNameTag(name.str()));
  }


  // Define thickness, width, and length. This is chosen as bounding box centered on the ski rotation axis
  // which contains the modules and dogleg (ie cooling blocks and cooling pipe is ignored.)
  m_thickness = 2 * std::abs(xmin1);
  // *** 17:00 Fri 27th May 2005 D.Naito modified. (08)*********************************
  //m_width = 2 * maxWidth;
  // *** -->>                                      (08)*********************************
  m_width = skiWidth;
  // *** End of modified lines. ------------------ (08)*********************************
  // m_length defined in constructor.


  // Calculate the clearances. Module envelope1 is the thickness up to the sensors. This is used for the module to 
  // module distance

  //std::cout << "Clearance Module to Module:        " 
  //	    <<  m_radialSep - m_module->env1Thickness() << std::endl;
  //std::cout << "Clearance Module to Dogleg:        " 
  //	    << std::abs(xDoglegOffset) - 0.5*m_dogleg->thickness() - 0.5*m_module->thickness() << std::endl;
  //std::cout << "Clearance Module to Cooling Block: " 
  //	    << std::abs(xCoolingBlockOffset) - 0.5*m_coolingBlock->thickness() - 0.5 * m_module->baseBoard()->thickness() - m_safety
  //	    << std::endl;
  //std::cout << "Cooling block to pipe: " <<  std::abs(-xModuleOffset + xCoolingBlockOffset - xCoolingPipePos) 
  // - 0.5*m_coolingBlock->thickness() - m_coolingPipe->pipeRadius() 
  //	    << std::endl;

  return skiLog;

}


GeoVPhysVol * 
SCT_Ski::build(SCT_Identifier id) const
{
  GeoPhysVol * ski = new GeoPhysVol(m_logVolume);

 
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
  
    // Add modules. 
    ski->add(m_refPointTransform);
    GeoAlignableTransform * moduleTransform = new GeoAlignableTransform(m_modulePos[iModule]);
    ski->add(moduleTransform);
    ski->add(m_nameTag[iModule]);
    ski->add(new GeoIdentifierTag(m_id[iModule]));
    id.setEtaModule(m_id[iModule]); // Set identifier.
    GeoVPhysVol * modulePV = m_module->build(id);
    ski->add(modulePV);  
    
    // Store alignable transform
    detectorManager()->addAlignableTransform(1, id.getWaferId(), moduleTransform, modulePV);
 
    // Add dogleg
    ski->add(m_refPointTransform);
    ski->add(m_doglegPos[iModule]);
    ski->add(m_dogleg->getVolume());

    // and coolingblock
    ski->add(m_refPointTransform);
    ski->add(m_coolingBlockPos[iModule]);
    ski->add(m_coolingBlock->getVolume());
  }

  // Add Cooling pipe
  ski->add(m_refPointTransform);
  ski->add(m_coolingPipePos);
  ski->add(m_coolingPipe->getVolume());
  
  return ski;
}
