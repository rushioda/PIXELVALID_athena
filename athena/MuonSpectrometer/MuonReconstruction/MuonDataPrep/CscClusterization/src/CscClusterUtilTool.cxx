/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscClusterUtilTool.cxx

#include "CscClusterUtilTool.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscClusterization/ICscClusterFitter.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

using std::vector;
using std::string;
using Muon::CscPrepData;
using Muon::CscStripPrepDataContainer;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepData;

//**********************************************************************

CscClusterUtilTool::
CscClusterUtilTool(string type, string aname, const IInterface* parent)
  : AthAlgTool(type, aname, parent),
    m_pmuon_detmgr(0), m_phelper(0),
    m_stripFitter("CalibCscStripFitter/CalibCscStripFitter"),
    m_precClusterFitter("QratCscClusterFitter/QratCscClusterFitter")
{
  declareInterface<ICscClusterUtilTool>(this);
  declareProperty("strip_fitter", m_stripFitter);
  declareProperty("precision_fitter", m_precClusterFitter);
  declareProperty("CscStripPrepDataLocation",  m_cscStripLocation = "CSC_Measurements");
}

//**********************************************************************

CscClusterUtilTool::~CscClusterUtilTool() { }

//**********************************************************************

StatusCode CscClusterUtilTool::initialize() {

  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_MSG_DEBUG ( "  Strip fitter is " << m_stripFitter.typeAndName() );
  ATH_MSG_DEBUG ( "  CscStripPrepDataLocation is " << m_cscStripLocation );

  // Retrieve the strip fitting tool.
  if ( m_stripFitter.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve strip fitting tool " << m_stripFitter );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_DEBUG ( "Retrieved strip fitting tool " << m_stripFitter );

  // Retrieve the precision cluster fitting tool.
  if ( m_precClusterFitter.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve CSC precision cluster fitting tool "
                    << m_precClusterFitter->name() );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved CSC precision cluster fitting tool" );

  // Retrieve the detector descriptor.
  if ( detStore()->retrieve(m_pmuon_detmgr).isFailure() ) {
    ATH_MSG_ERROR ( " Cannot retrieve MuonGeoModel " );
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ( "Retrieved geometry." );
  m_phelper = m_pmuon_detmgr->cscIdHelper();


  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode CscClusterUtilTool::finalize() {
  ATH_MSG_VERBOSE ( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}

//**********************************************************************
void CscClusterUtilTool::getStripFits(const Trk::RIO_OnTrack* rot,
                                      ICscClusterFitter::StripFitList& sfits) const {

  if ( !rot ) {
    ATH_MSG_WARNING ( " Trk::RIO_OnTrack* rot is empty !" );
    return;
  }
  const Muon::CscClusterOnTrack* pclu = dynamic_cast<const Muon::CscClusterOnTrack*>(rot);
  if ( !pclu ) {
    ATH_MSG_WARNING ( " Muon::CscClusterOnTrack* pclu is empty !" );
    return;
  }
  getStripFits(pclu, sfits);

  return;
}

void CscClusterUtilTool::getStripFits(const Muon::CscClusterOnTrack* pclu,
                                      ICscClusterFitter::StripFitList& sfits) const {

  if ( !pclu ) {
    ATH_MSG_WARNING ( " Muon::CscClusterOnTrack* pclu is empty !" );
    return;
  }
  const Muon::CscPrepData* pprd = pclu->prepRawData();
  if ( !pprd ) {
    ATH_MSG_WARNING ( " Muon::CscPrepData* pprd is empty !" );
    return;
  }

  getStripFits(pprd, sfits);
  return;

}
////////////////////////////////////////////////////////////
void CscClusterUtilTool::getStripFits(const CscPrepData* MClus,
                                      ICscClusterFitter::StripFitList& sfits) const {

  if ( !MClus ) {
    ATH_MSG_WARNING ( " Muon::CscPrepData* pprd is empty !" );
    return;
  }

  sfits.clear();

  vector<const CscStripPrepData*> strPrepDatas =getStrips(MClus);
  for (unsigned int i=0; i<strPrepDatas.size(); ++i) {
    ICscClusterFitter::StripFit sfit;
    sfit = m_stripFitter->fit(*strPrepDatas[i]);
    ATH_MSG_VERBOSE ( " in loop charge " << sfit.charge );
    //    if (sfit.charge == 0.) // It's the case of narrow strips...in QratStripFitter
    //      sfit.charge = 40.;  // 0 charge is set to minimal non zero to do fit e.g. dead channel case
    sfits.push_back(sfit);
  }
  return;
}
///////////////////////////////////////////////////////////
ICscClusterFitter::Results CscClusterUtilTool::getRefitCluster(const CscPrepData* MClus,
                                                               double tantheta) const {

  ICscClusterFitter::Results results;
  if ( !MClus ) {
    ATH_MSG_WARNING ( " Muon::CscPrepData* pprd is empty !" );
    return results;
  }

  ICscClusterFitter::StripFitList sfits;
  sfits.clear();

  getStripFits(MClus, sfits);
  results = m_precClusterFitter->fit(sfits,tantheta);

  return results;
}
/////////////////////////////////////////////////////////////////////////////

vector<const CscStripPrepData*> CscClusterUtilTool::getStrips(const CscPrepData* MClus) const {

  vector<const CscStripPrepData*> strips;
  if ( !MClus ) {
    ATH_MSG_WARNING ( " Muon::CscPrepData* pprd is empty !" );
    return strips;
  }

  vector<Identifier> prd_digit_ids = MClus->rdoList();

  const DataHandle<Muon::CscStripPrepDataContainer> pdigcont;

  if ( evtStore()->retrieve(pdigcont, m_cscStripLocation).isFailure() ) {
    ATH_MSG_WARNING ( "Strip container " << m_cscStripLocation << " not found in StoreGate!" );
    return strips;
  }
  ATH_MSG_DEBUG ( "Retrieved " << m_cscStripLocation << " successfully. " );


  IdentifierHash elhash=MClus->collectionHash();
  CscStripPrepDataContainer::const_iterator it = pdigcont->indexFind(elhash);

  ATH_MSG_VERBOSE ( "Hash " << elhash << " converted to iterator of container successfully");
  
  if (it != pdigcont->end()) {
    ATH_MSG_VERBOSE ( " it == pdigcont.end() passed");
    for ( unsigned int istrip=0; istrip<prd_digit_ids.size(); ++istrip ) {
      const CscStripPrepDataCollection& col = **it;
      // Loop over digits and fill these arrays.
      for ( CscStripPrepDataCollection::const_iterator idig=col.begin();
            idig!=col.end(); ++idig ) {
        const CscStripPrepData& dig = **idig;
        Identifier stid = dig.identify();
        if (stid!=prd_digit_ids[istrip]) continue;
        const CscStripPrepData* pstrip = &dig;
        ATH_MSG_VERBOSE ( "strip " << pstrip->timeOfFirstSample() );
        
        strips.push_back(pstrip);
        break;
      }
    }
  }
  return strips;
}



