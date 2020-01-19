/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile DetDescrCnv package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TileHWIDDetDescrCnv.cxx,v 1.7 2009-02-12 10:11:09 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "TileIdCnv/TileHWIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "StoreGate/StoreGate.h" 

#include "IdDictDetDescr/IdDictManager.h"
//#include "Identifier/IdentifierHash.h"
#include "TileIdentifier/TileHWID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

StatusCode 
TileHWIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(messageService(), "TileHWIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endreq;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endreq;
	return sc;
    }

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
TileHWIDDetDescrCnv::finalize()
{
    MsgStream log(messageService(), "TileHWIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endreq;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
TileHWIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(messageService(), "TileHWIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a TileHWID helper object in the detector store" << endreq;

    // Create a new TileHWID

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endreq;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string helperKey  = *( ddAddr->par() );
    if ("" == helperKey) {
	log << MSG::DEBUG << "No Helper key " << endreq;
    }
    else {
	log << MSG::DEBUG << "Helper key is " << helperKey << endreq;
    }
    

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endreq;
	return StatusCode::FAILURE;
    } else {}
 
    // Get the dictionary manager from the detector store
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get IdDictManager !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the IdDictManager. " << endreq;
    }


    // create the helper
    TileHWID* tilehw_id = new TileHWID;

    // pass a pointer to IMessageSvc to the helper
    tilehw_id->setMessageSvc(messageService());

    if (idDictMgr->initializeHelper(*tilehw_id)) {
	log << MSG::ERROR << "Unable to initialize TileHWID" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized TileHWID. " << endreq;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(tilehw_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
TileHWIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

long
TileHWIDDetDescrCnv::repSvcType() const
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
TileHWIDDetDescrCnv::classID() { 
    return ClassID_traits<TileHWID>::ID(); 
}

//--------------------------------------------------------------------
TileHWIDDetDescrCnv::TileHWIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<TileHWID>::ID(), svcloc)
{}

