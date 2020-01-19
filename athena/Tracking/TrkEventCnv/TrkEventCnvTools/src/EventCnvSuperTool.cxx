/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventCnvTools/EventCnvSuperTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include <typeinfo>

Trk::EventCnvSuperTool::EventCnvSuperTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_idCnvTool("InDet::InDetEventCnvTool/InDetEventCnvTool"),
    m_muonCnvTool("Muon::MuonEventCnvTool/MuonEventCnvTool"),
    m_detID(0),
    m_haveIdCnvTool(false),   // Will be set to true on retrieval
    m_haveMuonCnvTool(false), // Will be set to true on retrieval
    m_doMuons(true),
    m_doID(true),
    m_errCount(0),
    m_maxErrCount(10)
{
    declareInterface<IEventCnvSuperTool>(this);
    declareProperty("DoMuons",m_doMuons, "If true (default), attempt to retrieve Muon helper tool and convert Muon objects.");
    declareProperty("DoID",m_doID, "If true (default), attempt to retrieve Inner Detector helper tool and convert ID objects.");
    declareProperty("MaxErrorCount", m_maxErrCount, "Maximum number of errors that will be reported");
}

Trk::EventCnvSuperTool::~EventCnvSuperTool(){
  if (m_errCount>m_maxErrCount) ATH_MSG_WARNING("Suppressed "<<(m_errCount-m_maxErrCount)<<" WARNING or ERROR messages");
}

StatusCode
Trk::EventCnvSuperTool::initialize(){   
    // Try to get det store and then AtlasID
    StoreGateSvc *detStore = 0;
    StatusCode sc = service( "DetectorStore", detStore );
    if( sc.isFailure() ) {
        msg(MSG::WARNING) << "Could not get DetectorStore, nor Id Helper, and so will be unable to do anything useful." << endreq;
    } else {    
        sc = detStore->retrieve( m_detID, "AtlasID" );
        if( sc.isFailure() ) {
            msg(MSG::WARNING) << "Could not get AtlasDetectorID " << endreq;
        }
    }
        
    //Now try to get the tools
    if ( m_doID && !m_idCnvTool.empty() ) {
        if (m_idCnvTool.retrieve().isFailure() ) 
        {
            msg(MSG::DEBUG) << "Failed to retrieve InDet helper tool "<< m_idCnvTool 
                  <<". Will not be able to recreate ID Surfaces / Det Elements."<<endreq;
            m_doID=false;
        } else {
            msg(MSG::VERBOSE) << "Retrieved tool " << m_idCnvTool << endreq;
            m_haveIdCnvTool=true;
        }
    }

    if ( m_doMuons && !m_muonCnvTool.empty() ) {
        if (m_muonCnvTool.retrieve().isFailure() ) 
        {
            msg(MSG::DEBUG) << "Failed to retrieve Muon helper tool "<< m_muonCnvTool 
                  <<". Will not be able to recreate ID Surfaces / Det Elements."<<endreq;
            m_doMuons=false;
        } else { 
            msg(MSG::VERBOSE) << "Retrieved tool " << m_muonCnvTool << endreq;
            m_haveMuonCnvTool=true;
        }
    }

    // Print an extra warning if neither tool found.
    if (!m_haveIdCnvTool && !m_haveMuonCnvTool){
        msg(MSG::WARNING) << "Failed to retrieve either and InDet or a Muon tool. Will not be able to recreate surfaces / detector elements."<< endreq;
        m_maxErrCount=0; // No point in further WARNINGs
    }
    
    return StatusCode::SUCCESS;
}

StatusCode
Trk::EventCnvSuperTool::finalize(){
    msg()<< "Finalize().";
    if (m_errCount>0) msg()<<" Tried to print "<<m_errCount<<" ERROR/WARNING messages (with maximum permissable = "<<m_maxErrCount<<")";
    msg()<<endreq;
    return StatusCode::SUCCESS;
}

Trk::ITrkEventCnvTool*    
Trk::EventCnvSuperTool::getCnvTool(const Identifier& id){
    if (m_detID==0) return 0;

    if(m_detID->is_indet(id))
    {
        //std::cout<<"Trk::EventCnvSuperTool::getCnvTool = ID"<<std::endl;
        if (m_haveIdCnvTool )
        {
            return &(*m_idCnvTool);
        }else{
            if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)
                << "ID RIO_OnTrack, but have no ID cnv tool!"
                << endreq;
            return 0;
        }
    }else{
        if(m_detID->is_muon(id) )
        {
            //std::cout<<"Trk::EventCnvSuperTool::getCnvTool = Muon"<<std::endl;
            if (m_haveMuonCnvTool)
            {
                return &(*m_muonCnvTool);
            }else{
                if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)
                    << "Muon RIO_OnTrack, but have no muon cnv tool. Cannot set check RoT."
                    << endreq;
                return 0;
            }
        }
    }
    
    if ( (m_errCount++)<m_maxErrCount){
        std::string ident = m_detID->show_to_string(id);        
        msg(MSG::WARNING)
        << "Unknown Identifier: ("<< ident<<"), that is ("<<id<<")"
        << endreq;
    }
    return 0;
    
}

const Trk::Surface* 
Trk::EventCnvSuperTool::getSurface(const Identifier& id){
    const Surface* surface = 0;
    Trk::ITrkEventCnvTool* cnvTool = getCnvTool(id);
    if (cnvTool!=0) {
        const TrkDetElementBase* detEl = cnvTool->getDetectorElement( id );
        if (detEl!=0)
            surface = &(detEl->surface(id));
        else
            if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)<< "getSurface: could not get detector element from id:"<<id<<" Returning 0." << endreq;            
    } else {
        if ( (m_errCount++)<m_maxErrCount) msg(MSG::WARNING)<< "getSurface: could not get cnv tool for Identifier:"<<id<< endreq;
    }
    return surface;
}

void
Trk::EventCnvSuperTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT )
{
    using namespace std;
    Trk::ITrkEventCnvTool* cnvTool = getCnvTool(RoT->identify());
    if (cnvTool!=0) {
        cnvTool->recreateRIO_OnTrack( RoT );
    } else {
        const type_info& info = typeid(*RoT);
        if ( (m_errCount++)<m_maxErrCount) 
            msg(MSG::WARNING)<< "recreateRIO_OnTrack: could not get cnv tool. Returning without correctly filling ROT of type: "<< info.name()<< endreq;
    }
    return;
}

void
Trk::EventCnvSuperTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT )
{
    Trk::ITrkEventCnvTool* cnvTool = getCnvTool(RoT->identify());
    if (cnvTool!=0) {
        cnvTool->prepareRIO_OnTrack( RoT );
    } else {
        if ( (m_errCount++)<m_maxErrCount) msg()<< "prepareRIO_OnTrack could not find appropriate tool to prepare: "<<*RoT<<std::endl; 
    }
    return;
}
