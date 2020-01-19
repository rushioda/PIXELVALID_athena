/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArG4Code/EnergySpot.h"
#include "LArSimEvent/LArHitContainer.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"
#include "StoreGate/StoreGateSvc.h"

using HepGeom::Point3D;
using HepGeom::Transform3D;

// Constructor:
EndcapFastSimDedicatedSD::EndcapFastSimDedicatedSD(StoreGateSvc* detStore)
  : IFastSimDedicatedSD("EndcapFastSimDedicatedSD", detStore) 
  , m_emecManager(nullptr)
  , m_innerWheelCalculatorPos(nullptr)
  , m_innerWheelCalculatorNeg(nullptr)
  , m_outerWheelCalculatorPos(nullptr)
  , m_outerWheelCalculatorNeg(nullptr)
{
  G4cout << GetName() << "::initialize()" << G4endl;
  if ( detStore->retrieve( m_emecManager ).isFailure()  ){
    throw std::runtime_error("Could not retrieve EMEC manager");
  }
  m_innerWheelCalculatorPos = new LArWheelCalculator(LArG4::InnerAbsorberWheel,+1);
  m_innerWheelCalculatorNeg = new LArWheelCalculator(LArG4::InnerAbsorberWheel,-1);
  m_outerWheelCalculatorPos = new LArWheelCalculator(LArG4::OuterAbsorberWheel,+1);
  m_outerWheelCalculatorNeg = new LArWheelCalculator(LArG4::OuterAbsorberWheel,-1);
}

// ProcessHitsMethod
void EndcapFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){

  // Fill the identifier.
  Point3D<double> globalPosition=spot.GetPosition();

  static Transform3D xfPos = m_emecManager->getDetectorRegion(1,1,0,0)->getAbsoluteTransform().inverse();
  static Transform3D xfNeg = m_emecManager->getDetectorRegion(0,1,0,0)->getAbsoluteTransform().inverse();


  Point3D<double> localPosition        = globalPosition.z()<0 ? xfNeg*globalPosition : xfPos*globalPosition;
  Point3D<double> shiftedLocalPosition = localPosition + Point3D<double>(0.0,0.0,m_emecManager->getFocalToRef()+m_emecManager->getRefToActive());
  int zIndex = globalPosition.z()<0 ? 0:1;
  double eta = shiftedLocalPosition.getEta();
  double phi = localPosition.getPhi();
  double z   = localPosition.z();

  EMECDetectorManager::DetectorRegionConstIterator e;
  for (e=m_emecManager->beginDetectorRegion();e!=m_emecManager->endDetectorRegion();  e++) {
    const EMECDetectorRegion *region = *e;
    if (region->getEndcapIndex()!=zIndex) continue;
    const EMECDetDescr       *regionDescriptor=region->getDescriptor();
    const CellBinning       & etaBinning=regionDescriptor->getEtaBinning();
    if (eta>etaBinning.getStart() && eta<etaBinning.getEnd()) {

      unsigned int etaIndex = int((eta - etaBinning.getStart())/etaBinning.getDelta()) + etaBinning.getFirstDivisionNumber();
      unsigned int phiIndex = 0;
      EMECCellConstLink cellPtr = region->getEMECCell(etaIndex,phiIndex);
      double zmax = cellPtr->getZLocal(EMECCell::BACK);
      double zmin = cellPtr->getZLocal(EMECCell::FRONT);
      if (z>zmin && z<zmax) {
        const CellBinning       & phiBinning=regionDescriptor->getPhiBinning();
        double minPhi=std::min(phiBinning.getStart(), phiBinning.getEnd());
        double maxPhi=std::max(phiBinning.getStart(), phiBinning.getEnd());
        while (phi<minPhi)   phi += 2*M_PI;
        while (phi>maxPhi)   phi -= 2*M_PI;

        unsigned int regionIndex = region->getRegionIndex();
        unsigned int samplingIndex = region->getSamplingIndex();
        //unsigned int phiIndex      =  int((phi - phiBinning.getStart())/phiBinning.getDelta()) + phiBinning.getFirstDivisionNumber();
        int wheel  = region->getRadialIndex()==0 ? 2:3;
        int sWheel = zIndex==0 ? -wheel: wheel;

        // But now we are more sophisticated...:

        LArWheelCalculator *wheelCalc=NULL;
        int nGaps= region->getRadialIndex()==0 ? 768:256;
        int nBins= phiBinning.getNumDivisions();
        int gapsPerBin = nGaps/nBins;


        if      (zIndex==0 && region->getRadialIndex()==0) {
          wheelCalc=const_cast<LArWheelCalculator *> (m_outerWheelCalculatorNeg);
        }
        else if (zIndex==0 && region->getRadialIndex()==1){
          wheelCalc=const_cast<LArWheelCalculator *> (m_innerWheelCalculatorPos);
        }
        else if (zIndex==1 && region->getRadialIndex()==0){
          wheelCalc=const_cast<LArWheelCalculator *>(m_outerWheelCalculatorNeg);
        }
        else if (zIndex==1 && region->getRadialIndex()==1){
          wheelCalc=const_cast<LArWheelCalculator *> (m_innerWheelCalculatorPos);
        }
        else {
          throw std::runtime_error("Error, unknown wheel in EndcapFastSimDedicatedSD");
        }

        int phiBin = wheelCalc->GetPhiGap(localPosition)/gapsPerBin;

        if (zIndex==0) {
          phiBin = (nBins-2)/2 - phiBin;
          if(phiBin < 0) {
            phiBin += nBins;
          }
        }

        //std::cout << didIt <<  "Compare phi gap: " << nGaps << ' ' << nBins << ' ' << gapsPerBin << ':' << phiBin <<  '/' << phiIndex << std::endl;

        //static LArG4Identifier id;
        m_larID.clear();
        m_larID        << 4          // LArCalorimeter
                       << 1          // LArEM
                       << sWheel
                       << samplingIndex
                       << regionIndex
                       << etaIndex
                       << phiBin;
        // call process to add this to the collection 
        SimpleHit(m_larID, spot.GetTime(), spot.GetEnergy());
        return;
      }
    }
  }


}
