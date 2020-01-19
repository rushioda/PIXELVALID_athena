/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr DetDescrCnv package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: LArOnline_SuperCellIDDetDescrCnv.cxx,v 1.7 2009-02-11 04:50:26 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "LArIdCnv/LArOnline_SuperCellIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "StoreGate/StoreGate.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

long int
LArOnline_SuperCellIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
LArOnline_SuperCellIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(messageService(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endreq;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endreq;
	return sc;
    }

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
LArOnline_SuperCellIDDetDescrCnv::finalize()
{
    MsgStream log(messageService(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endreq;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
LArOnline_SuperCellIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(messageService(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a LArOnline_SuperCellID helper object in the detector store" << endreq;

    // Create a new LArOnline_SuperCellID

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
    //const DataHandle<IdDictManager> idDictMgr;
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
    LArOnline_SuperCellID* online_id = new LArOnline_SuperCellID;
    // pass a pointer to IMessageSvc to the helper
    online_id->setMessageSvc(messageService());

    if (idDictMgr->initializeHelper(*online_id)) {
	log << MSG::ERROR << "Unable to initialize LArOnline_SuperCellID" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized LArOnline_SuperCellID. " << endreq;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(online_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
LArOnline_SuperCellIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
LArOnline_SuperCellIDDetDescrCnv::classID() { 
    return ClassID_traits<LArOnline_SuperCellID>::ID(); 
}

//--------------------------------------------------------------------
LArOnline_SuperCellIDDetDescrCnv::LArOnline_SuperCellIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<LArOnline_SuperCellID>::ID(), svcloc)
{}



