/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Local
#include "InDetGlobalErrorMonTool.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "TH2F.h"

InDetGlobalErrorMonTool::InDetGlobalErrorMonTool( const std::string & type,
						  const std::string & name,
						  const IInterface* parent):
  ManagedMonitorToolBase(type, name, parent),
  m_pixID( 0 ),
  m_pixManager( 0 ),
  m_sctManager( 0 ),
  m_pixCond("PixelConditionsSummarySvc", name),
  m_ConfigurationSvc("InDetSCT_ConfigurationConditionsSvc", name),
  m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
  m_ErrorSvc("PixelByteStreamErrorsSvc",name),
  m_errorGeoPixel(),
  m_disabledGeoPixel(),
  m_errorGeoSCT(),
  m_disabledGeoSCT(),
  m_disabledModulesMapPixel( 0 ),
  m_disabledModulesMapSCT( 0 ),
  m_badModulesMapPixel(nullptr),
  m_errorModulesMapPixel(nullptr),
  m_errorModulesMapSCT(nullptr),
  m_totalModulesMapPixel(nullptr),
  m_totalModulesMapSCT(nullptr),
  m_totalModulesMapSi(nullptr),
  c_nBinsEta( 200 ),
  c_rangeEta( 2.5 ),
  c_nBinsPhi( 200 )
{
}

StatusCode InDetGlobalErrorMonTool::initialize() {

  if (detStore()->retrieve(m_pixID, "PixelID").isFailure()){
    msg(MSG::ERROR) << "Could not retrieve Pixel ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (detStore()->retrieve(m_pixManager, "Pixel").isFailure()){
    msg(MSG::ERROR) << "Could not retrieve Pixel Detector Manager" << endreq;
    return StatusCode::FAILURE;
  }
  if (detStore()->retrieve(m_sctManager, "SCT").isFailure()){
    msg(MSG::ERROR) << "Could not retrieve SCT Detector Manager" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_pixCond.retrieve().isFailure()){
    msg(MSG::ERROR) << "Could not retrieve Pixel conditions service!" << endreq;
    return StatusCode::FAILURE;
  }
  
  return ManagedMonitorToolBase::initialize();
}


/*---------------------------------------------------------*/
StatusCode InDetGlobalErrorMonTool::bookHistogramsRecurrent()
{
    MonGroup monGr_shift( this, "InDetGlobal/Track/DeadModules", run, ATTRIB_UNMANAGED );
  
    bool status = true;
    if ( newRunFlag() )
    {
      m_disabledModulesMapPixel = new TH2F( "disabledModulesMapPixel", "Map of disabled modules for Pixel", 
					    c_nBinsEta, -c_rangeEta, c_rangeEta, 
					    c_nBinsPhi, -M_PI, M_PI );
      
      m_disabledModulesMapSCT   = new TH2F( "disabledModulesMapSCT", "Map of disabled modules for SCT", 
					    c_nBinsEta, -c_rangeEta, c_rangeEta, 
					    c_nBinsPhi, -M_PI, M_PI );
       
      m_badModulesMapPixel = new TH2F( "badModulesMapPixel", "Map of bad modules for Pixel", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );
      
      m_errorModulesMapPixel = new TH2F( "errorModulesMapPixel", "Map of modules in error for Pixel", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );
      
     m_errorModulesMapSCT   = new TH2F( "errorModulesMapSCT", "Map of bad modules for SCT", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );
      
      m_totalModulesMapPixel = new TH2F( "totalModulesMapPixel", "Map of all bad and disabled modules for Pixel", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );
      
      m_totalModulesMapSCT   = new TH2F( "totalModulesMapSCT", "Map of all bad and disabled modules for SCT", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );

      m_totalModulesMapSi   = new TH2F( "totalModulesMapSi", "Map of all bad and disabled modules for Pixel and SCT", 
					 c_nBinsEta, -c_rangeEta, c_rangeEta, 
					 c_nBinsPhi, -M_PI, M_PI );
      
      status &= monGr_shift.regHist( m_disabledModulesMapPixel ).isSuccess();
      status &= monGr_shift.regHist( m_errorModulesMapPixel ).isSuccess();
      status &= monGr_shift.regHist( m_disabledModulesMapSCT ).isSuccess();
      status &= monGr_shift.regHist( m_errorModulesMapSCT ).isSuccess();
      status &= monGr_shift.regHist( m_totalModulesMapPixel ).isSuccess();
      status &= monGr_shift.regHist( m_totalModulesMapSCT ).isSuccess();
      status &= monGr_shift.regHist( m_totalModulesMapSi ).isSuccess();
    }
  
  return ( status ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/*---------------------------------------------------------*/
StatusCode InDetGlobalErrorMonTool::fillHistograms() 
{   
  return StatusCode::SUCCESS;
}

StatusCode InDetGlobalErrorMonTool::procHistograms()
{
    if ( ( endOfLumiBlockFlag() || endOfRunFlag() ) && m_manager->lumiBlockNumber() % 1 == 0 )
    {	
      m_disabledModulesMapPixel->Reset("ICE");
      m_errorModulesMapPixel->Reset("ICE");
      m_badModulesMapPixel->Reset("ICE");
      m_totalModulesMapPixel->Reset("ICE");
      m_disabledModulesMapSCT->Reset("ICE");
      m_errorModulesMapSCT->Reset("ICE");
      m_totalModulesMapSCT->Reset("ICE");
      m_totalModulesMapSi->Reset("ICE");
      
      SyncPixel();
      SyncErrorSCT();
      SyncDisabledSCT();

      {
	geoContainer_t::iterator currIt = m_disabledGeoPixel.begin();
	geoContainer_t::iterator currEnd = m_disabledGeoPixel.end();
	while (currIt != currEnd) {
	  FillModule( (*currIt).second, m_disabledModulesMapPixel );
	  ++currIt;
	}
      }
      
      {
	geoContainer_t::iterator currIt = m_errorGeoPixel.begin();
	geoContainer_t::iterator currEnd = m_errorGeoPixel.end();
	while (currIt != currEnd) {
	  FillModule( (*currIt).second, m_badModulesMapPixel );
	  ++currIt;
	}
      }

      m_totalModulesMapPixel->Add( m_disabledModulesMapPixel );
      m_totalModulesMapPixel->Add( m_errorModulesMapPixel );
      
      {
	geoContainerPure_t::iterator currIt = m_disabledGeoSCT.begin();
	geoContainerPure_t::iterator currEnd = m_disabledGeoSCT.end();
	while (currIt != currEnd) {
	  FillModule( (*currIt).second, m_disabledModulesMapSCT );
	  ++currIt;
	}
      }
      
      {
	geoContainer_t::iterator currIt = m_errorGeoSCT.begin();
	geoContainer_t::iterator currEnd = m_errorGeoSCT.end();
	while (currIt != currEnd) {
	  FillModule( (*currIt).second, m_errorModulesMapSCT );
	  ++currIt;
	}
      }
      
      m_totalModulesMapSCT->Add( m_disabledModulesMapSCT );
      m_totalModulesMapSCT->Add( m_errorModulesMapSCT );
      
      m_totalModulesMapSi->Add( m_totalModulesMapSCT );
      m_totalModulesMapSi->Add( m_totalModulesMapPixel );
    }
  return StatusCode::SUCCESS;
}

void InDetGlobalErrorMonTool::FillModule( moduleGeo_t module, TH2F * histo )
{
  unsigned int lowX  = 0;
  unsigned int highX = 0;
  unsigned int lowY  = 0;
  unsigned int highY = 0;
  double area = 0.;
  
  double widthEta = 2. * c_rangeEta / c_nBinsEta;
  double widthPhi = 2. * M_PI / c_nBinsPhi;
  double edgesEta[200], centerEta[200], 
    edgesPhi[200], centerPhi[200];
  
  histo->GetXaxis()->GetLowEdge(edgesEta);
  histo->GetXaxis()->GetCenter(centerEta);
  histo->GetYaxis()->GetLowEdge(edgesPhi);
  histo->GetYaxis()->GetCenter(centerPhi);
  
  for ( unsigned int i = 0; i < c_nBinsEta; i++ )
    if ( edgesEta[i] + widthEta > module.first.first )
      {
	lowX = i;
	break;
      }
  for ( unsigned int i = lowX; i < c_nBinsEta; i++ )
    if ( edgesEta[i] > module.first.second )
      {
	highX = i;
	break;
      }
  for ( unsigned int i = 0; i < c_nBinsPhi; i++ )
    if ( edgesPhi[i] + widthPhi > module.second.first )
      {
	lowY = i;
	break;
      }
  for ( unsigned int i = lowY; i < c_nBinsPhi; i++ )
    if ( edgesPhi[i] > module.second.second )
      {
	highY = i;
	break;
      }
  for ( unsigned int i = lowX; i < highX; i++ )
    for ( unsigned int j = lowY; j < highY; j++ )
      {
	area = (
		((( module.first.second < edgesEta[i] + widthEta ) ? module.first.second : (edgesEta[i] + widthEta) )  - 
		 ( ( module.first.first > edgesEta[i] ) ? module.first.first : edgesEta[i] ) ) *
		((( module.second.second < edgesPhi[j] + widthPhi ) ? module.second.second : (edgesPhi[j] + widthPhi) )  - 
		 ( ( module.second.first > edgesPhi[j] ) ? module.second.first : edgesPhi[j] ) ) 
		) /  ( widthEta * widthPhi ); 
	histo->Fill( centerEta[i], centerPhi[j], area );
      }
  return;
}

bool InDetGlobalErrorMonTool::SyncErrorSCT()
{
  double rz = 0;
  double deltaZ = 0;
  
  m_errorGeoSCT.clear();
  
  for ( unsigned int i = 0; i < SCT_ByteStreamErrors::NUM_ERROR_TYPES; i++ )
    {
      std::set<IdentifierHash> * sctErrors = m_byteStreamErrSvc->getErrorSet( i );
      std::set<IdentifierHash>::iterator fit = sctErrors->begin();
      std::set<IdentifierHash>::iterator fitEnd = sctErrors->end();
      
      // Check that all modules are registered
      for (; fit != fitEnd; ++fit) {
	// The module is already registered, no need to do something
	if ( m_errorGeoSCT.count( (*fit) ) )
	  continue;
	else
	  {
	    moduleGeo_t moduleGeo;
	    
	    InDetDD::SiDetectorElement * newElement = m_sctManager->getDetectorElement( (*fit) );
	    newElement->getEtaPhiRegion( deltaZ,
					 moduleGeo.first.first,  moduleGeo.first.second,
					 moduleGeo.second.first, moduleGeo.second.second,
					 rz );
	    
	    m_errorGeoSCT.insert( std::pair<IdentifierHash, moduleGeo_t>( (*fit), moduleGeo ) );
	  }
      }
    }
  
  return true;
}

bool InDetGlobalErrorMonTool::SyncPixel()
{
  double rz = 0;
  double deltaZ = 0;
  
  m_errorGeoPixel.clear();
  m_disabledGeoPixel.clear();
  
  PixelID::const_id_iterator fit = m_pixID->wafer_begin();
  PixelID::const_id_iterator fitEnd = m_pixID->wafer_end();
  
  // Check that all modules are registered
  for (; fit != fitEnd; ++fit) 
    {
      IdentifierHash waferHash = m_pixID->wafer_hash((*fit));
      
      // Inactive module, flagging time!
      if ( m_pixCond->isActive( waferHash ) == false )
	{
	  moduleGeo_t moduleGeo;
	  
	  InDetDD::SiDetectorElement * newElement = m_pixManager->getDetectorElement( (*fit) );
	  newElement->getEtaPhiRegion( deltaZ,
				       moduleGeo.first.first,  moduleGeo.first.second,
				       moduleGeo.second.first, moduleGeo.second.second,
				       rz );
	  
	  m_disabledGeoPixel.insert( std::pair<IdentifierHash, moduleGeo_t>( waferHash, moduleGeo ) );
	  
	}
      // Bad module, flagging time!
      if ( m_pixCond->isActive( waferHash ) == true && m_pixCond->isGood( waferHash ) == false )
	{
	  moduleGeo_t moduleGeo;
	  
	  InDetDD::SiDetectorElement * newElement = m_pixManager->getDetectorElement( (*fit) );
	  newElement->getEtaPhiRegion( deltaZ,
				       moduleGeo.first.first,  moduleGeo.first.second,
				       moduleGeo.second.first, moduleGeo.second.second,
				       rz );
	  
	  m_errorGeoPixel.insert( std::pair<IdentifierHash, moduleGeo_t>( waferHash, moduleGeo ) );
	}
    }
  
  return true;
}

bool InDetGlobalErrorMonTool::SyncDisabledSCT()
{
  bool altered = false;
  double rz = 0;
  double deltaZ = 0;
  
  m_disabledGeoSCT.clear();
  std::set<Identifier>* badModules = m_ConfigurationSvc->badModules();
  std::set<Identifier>::iterator fit = badModules->begin();
  std::set<Identifier>::iterator fitEnd = badModules->end();
  
  // Check that all modules are registered
  for (; fit != fitEnd; ++fit)
    {
      // The module is already registered, no need to do something
      if ( m_disabledGeoSCT.count( (*fit) ) )
	continue;
      else
	{
	  altered = true;
	  moduleGeo_t moduleGeo;
	  
	  InDetDD::SiDetectorElement * newElement = m_sctManager->getDetectorElement( (*fit) );
	  newElement->getEtaPhiRegion( deltaZ,
				       moduleGeo.first.first,  moduleGeo.first.second,
				       moduleGeo.second.first, moduleGeo.second.second,
				       rz );
	
	  m_disabledGeoSCT.insert( std::pair<Identifier, moduleGeo_t>( (*fit), moduleGeo ) );
	}
    }
  
  return altered;
}
