/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Muon DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TGC_IDDetDescrCnv.cxx,v 1.8 2004/06/04 03:59:16 ketevi Exp $
//<version>	$Name:  $

//<<<<<< INCLUDES                                                       >>>>>>

#include "TGC_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "StoreGate/StoreGate.h" 
#include "IdDictDetDescr/IdDictManager.h"
#include "MuonIdHelpers/TgcIdHelper.h"


//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

//--------------------------------------------------------------------

long int   
TGC_IDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
TGC_IDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "TGC_IDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for TgcIdHelper into the detector store. However,
    // TGC_IDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for TgcIdHelper to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
TGC_IDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "TGC_IDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
TGC_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "TGC_IDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a TgcIdHelper object in the detector store" << endmsg;

    // Create a new TgcIdHelper

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string helperKey  = *( ddAddr->par() );
    if ("" == helperKey) {
	log << MSG::DEBUG << "No Helper key " << endmsg;
    }
    else {
	log << MSG::DEBUG << "Helper key is " << helperKey << endmsg;
    }
    

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {}
 
    // Get the dictionary manager from the detector store
    const DataHandle<IdDictManager> idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
    }


    // create the helper
    TgcIdHelper* tgc_id = new TgcIdHelper;
    if (idDictMgr->initializeHelper(*tgc_id)) {
	log << MSG::ERROR << "Unable to initialize TgcIdHelper" << endmsg;
	return StatusCode::FAILURE;
    } 

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(tgc_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
TGC_IDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
TGC_IDDetDescrCnv::classID() { 
    return ClassID_traits<TgcIdHelper>::ID(); 
}

//--------------------------------------------------------------------
TGC_IDDetDescrCnv::TGC_IDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<TgcIdHelper>::ID(), svcloc)
{}



