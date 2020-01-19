/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDictDetDescrCnv package
 -----------------------------------------
 Copyright (C) 2002 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>   $Id: IdDictDetDescrCnv.cxx,v 1.21 2009-02-15 13:08:19 schaffer Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDictDetDescrCnv.h"

#include "IdDictParser/IdDictParser.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"

#include "StoreGate/StoreGate.h" 
#include "SGTools/StorableConversions.h"

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
IdDictDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(messageService(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in initialize" << endreq;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endreq;
        return sc;
    }
    
    // Must set indet tag to EMPTY
    m_inDetIDTag = "EMPTY";
    
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::finalize()
{
    MsgStream log(messageService(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in finalize" << endreq;

    // Remove parser
    if(m_parser) delete m_parser;
    m_parser = 0;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
IdDictDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //
    // Here we create an IdDictManager and provide it with an
    // IdDictMgr which has been filled by an IdDictParser. This mgr
    // is used by each IdHelper to initialize itself. 
    //
    //   Lifetime management:
    //
    //     IdDictDetDescrCnv holds onto ONE IdDictParser, which in
    //     turn holds the same IdDictMgr. 
    //
    //     Multiple initializations are possible. parseXMLDescription
    //     will look for a new set of xml files, clear any
    //     pre-existing IdDictMgr help by the parser and then parse
    //     the new xml files, filling the IdDictMgr.
    //
    //     Since the parser "refills" the same IdDictMgr, one has the
    //     option to delete and recreate the IdDictManager, or just
    //     keep the same one which will be refreshed with the new
    //     description. 
    //
    MsgStream log(messageService(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a IdDictManager object in the detector store" << endreq;

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
        log << MSG::ERROR << "Could not cast to DetDescrAddress." << endreq;
        return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    if ("" == mgrKey) {
        log << MSG::DEBUG << "No Manager key " << endreq;
    }
    else {
        log << MSG::DEBUG << "Manager key is " << mgrKey << endreq;
    }
    
    StatusCode sc = parseXMLDescription();
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR << " Cannot parse the XML description " << endreq; 
        return sc ;
    }


    // Create the manager - only once
    IdDictManager* dictMgr = new IdDictManager(m_parser->m_idd); 

    log << MSG::DEBUG << "Created IdDictManager " 
        << endreq;
    

    // Print out the dictionary names
    printDicts(dictMgr);

    // Pass a pointer to the container to the Persistency service
    // by reference.
    pObj = SG::asStorable(dictMgr);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

StatusCode
IdDictDetDescrCnv::parseXMLDescription() 
{
    MsgStream log(messageService(), "IdDictDetDescrCnv");

    log << MSG::DEBUG << "in getManager" << endreq;

    // Parse the xml files to obtain the iddict dictionaries
    //
    //   Parsing of the xml files may not be needed. So we check. The
    //   conditions to reparse are:
    //
    //     - first pass, i.e. creating a new IdDictParser
    //     - a change in an input xml file
    //     - a change in one of the "options" such as doIdChecks,
    //       doInitNeighbors, etc.
    //
    
    m_doParsing = false; // Preset to no parsing

    if (!m_parser) {

        // Create parser
        m_parser    = new IdDictParser;
        m_doParsing = true; 
        
    }
    
    // We access the properties on each pass

    // Get the access to DetDescrCnvSvc for access to properties
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("DetDescrCnvSvc", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR 
            << " Cannot get DetDescrCnvSvc " 
            << endreq; 
        return sc ;
    }

    // Set flag for doing checks of ids
    BooleanProperty     boolProperty("DoIdChecks", false);
    sc = propertyServer->getProperty(&boolProperty);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get DoIdChecks flag: found " 
            << boolProperty.value()
            << endreq;
        return sc;
    }
    else {
        bool doChecks = boolProperty.value();
        if (doChecks != m_doChecks)m_doParsing = true; // Changed flag
        m_doChecks = doChecks;
        log << MSG::DEBUG << "Flag DoIdChecks is:  " 
            << m_doChecks
            << endreq;
    }

    // Set flag for initializing neighbours
    BooleanProperty     boolProperty1("DoInitNeighbours", true);
    sc = propertyServer->getProperty(&boolProperty1);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get DoInitNeighbours flag: found " 
            << boolProperty1.value()
            << endreq;
        return sc;
    }
    else {
        bool doNeighbours = boolProperty1.value();
        if (doNeighbours != m_doNeighbours)m_doParsing = true; // Changed flag
        m_doNeighbours = doNeighbours;
        log << MSG::DEBUG << "Flag DoInitNeighbours is:  " 
            << m_doNeighbours
            << endreq;
    }

    // Name of IdDict file
    StringProperty property("IdDictName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        std::string idDictName = property.value();
        if (idDictName != m_idDictName)m_doParsing = true; // Changed flag
        m_idDictName = idDictName;
        log << MSG::INFO << "IdDictName:  " 
            << m_idDictName
            << endreq;
    }


    // Get the file names: two options - either from jobOpt
    // properties of the DetDescrCnvSvc or from RDB tags
        
    // Determine if Dict filename comes from DD database or
    // if properties from JobOptions should be used.
    BooleanProperty  boolProperty2("IdDictFromRDB", false);
    sc = propertyServer->getProperty(&boolProperty2);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictFromRDB: found " 
            << boolProperty2.value()
            << endreq;
        return sc;
    } else {
        m_idDictFromRDB =  boolProperty2.value();           
        log << MSG::DEBUG << "IdDictFromRDB:  " 
            << (boolProperty2.value() ? "True" : "False")
            << endreq;
    }
        
    if (m_idDictFromRDB) {
        log << MSG::DEBUG << "Dictonary file name from DD database" 
            << endreq;
    } else {
        log << MSG::DEBUG << "Dictonary file name from job options or using defaults." 
            << endreq;
    }
        
    // Get the file name to parse:
    //
    //   1) From Relational DB
    //   2) Via jobOptions
    //   3) default name in the xml files
    //
    //  Currently the logic is 1) or 2) and 3) covers the case where
    //  no file name is found.
    //
    if (m_idDictFromRDB) {

        // Get file names from RDB
        StatusCode sc = getFileNamesFromTags();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to get Id file names from RDB " 
                << endreq;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Looked for ID file name from RDB " 
                << endreq;
        }

    }
    else {
        // Get file names from properties
        sc = getFileNamesFromProperties(propertyServer);
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to get Id file names from properties " 
                << endreq;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Looked for ID file name from properties " 
                << endreq;
        }
    }
        
            
//          if (!idDictFromRDB || inDetCustom) {
//              // We get it from properties
//              if (inDetCustom) {
//                  log << MSG::WARNING << "InnerDetector dictionary from custom settings." << endreq; 
//              }
//          } 


    // Only parse if necessary
    if (m_doParsing) {
        

        // Register the requested files with the xml parser
        sc = registerFilesWithParser();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to register file names " 
                << endreq;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Registered file names " 
                << endreq;
        }
        
        // Check whether a tag is needed for dictionary initialization

        // NOTE: the internal tag for IdDict is global, but is only
        // used for InDet and thus is defined by InDet
        std::string tag;
        if (m_inDetIDTag == "EMPTY") {
            sc = getTag(tag);
            if (!sc.isSuccess()) {
                log << MSG::DEBUG << " no tag found " 
                    << endreq;
                tag = "";  // force empty tag
            } 
        } else {
            tag = m_inDetIDTag;
        }

        // Parse the dictionaries
        m_parser->parse(m_idDictName.c_str(), tag);
        
        if (tag == "") tag = "default";
        log << MSG::DEBUG << "Read dict:  " 
            << m_idDictName << " with tag " << tag
            << endreq;

        // Set flag to check ids
        IdDictMgr& mgr = m_parser->m_idd;
        if(m_doChecks) {
            mgr.set_do_checks(true);
            log << MSG::DEBUG << "Set IdDictManager doChecks flag to true " 
                << endreq;
        }
        else {
            mgr.set_do_checks(false);
            log << MSG::DEBUG << "Set IdDictManager doChecks flag to false  " 
                << endreq;
        }

        // Set flag to initialize neighbours
        if(m_doNeighbours) {
            mgr.set_do_neighbours(true);
            log << MSG::DEBUG << "Set IdDictManager doNeighbours flag to true " 
                << endreq;
        }
        else {
            mgr.set_do_neighbours(false);
            log << MSG::DEBUG << "Set IdDictManager doNeighbours flag to false  " 
                << endreq;
        }

        // Do some checks
        const IdDictMgr::dictionary_map& dm = mgr.get_dictionary_map (); 
        if (dm.begin () == dm.end ()) {
            // No dicts found
            log << MSG::ERROR << "No dictionaries found!" << endreq;
            return StatusCode::FAILURE;
        }
        else {
            log << MSG::DEBUG << "Found " << dm.size() << " dictionaries." << endreq;
        }

        // Register the requested files and tags with the id dicts
        sc = registerInfoWithDicts();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to register info with dicts " 
                << endreq;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Registered info with id dicts " 
                << endreq;
        }
    }
    else {
        log << MSG::WARNING << "NOTE:  ** parseXMLDescription called, but parsing was deemed unnecessary ** " 
            << endreq;
    }
    

    log << MSG::DEBUG << "parseXMLDescription: Finished parsing and setting options " 
        << endreq;

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::initTheManager()
{
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

long int
IdDictDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
IdDictDetDescrCnv::classID() { 
    return ClassID_traits<IdDictManager>::ID(); 
}

//--------------------------------------------------------------------
IdDictDetDescrCnv::IdDictDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<IdDictManager>::ID(), svcloc),
    m_dictMgr(0),
    m_parser(0),
    m_doChecks(false),
    m_doNeighbours(true),
    m_idDictFromRDB(false),
    m_doParsing(false)
{}

//--------------------------------------------------------------------
StatusCode
IdDictDetDescrCnv::getTag(std::string& tag)
{

    MsgStream log(messageService(), "IdDictDetDescrCnv");


    // Allow the internal tag for dictionaries to be set by
    // jobOptions. Note that this is not used as far as I know. RDS
    // 10/2007. 
    log << MSG::DEBUG << "in getTag: chenk if tag is set in jobOpts" << endreq;

    // Check whether we initialize from Zebra or ROOT
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("DetDescrCnvSvc", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR 
            << " Cannot get DetDescrCnvSvc " 
            << endreq; 
        return sc ;
    }

    // Finally get tag from DetDescrCnvSvc property

    StringProperty tagProperty("IdDictGlobalTag", "");
    sc = propertyServer->getProperty(&tagProperty);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictGlobalTag: found " 
            << tagProperty.value()
            << endreq;
    }
    else {
        tag = tagProperty.value();
        log << MSG::DEBUG << "Found IdDictGlobalTag:  " 
            << tag
            << endreq;
    }

    return sc;
}
    
//--------------------------------------------------------------------
void    
IdDictDetDescrCnv::printDicts(const IdDictManager* dictMgr)
{
    MsgStream  log(msgSvc(),"IdDictDetDescrCnv");

    log << MSG::INFO 
        << "Found id dicts:" 
        << endreq;

    if(!dictMgr) return;
    
    std::string tag = dictMgr->manager()->tag();
    if (!tag.size()) tag = "<no tag>";
    log << MSG::INFO << "Using dictionary tag: " << tag << endreq;
    
    const IdDictMgr::dictionary_map& dm = dictMgr->manager()->get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
        const IdDictDictionary& dictionary = *((*it).second); 
        std::string version = ("" != dictionary.m_version) ? dictionary.m_version : "default";
        log << MSG::INFO 
            << "Dictionary " << dictionary.m_name;
        if (dictionary.m_name.size() < 20) {
            std::string space(20-dictionary.m_name.size(),' ');
            log << MSG::INFO << space;
        }
        log << MSG::INFO 
            << " version " << version;
        if (version.size() < 20) {
            std::string space(20-version.size(),' ');
            log << MSG::INFO << space;
        }
        if (dictionary.dict_tag().size()) {
            log << MSG::INFO 
                << " DetDescr tag " << dictionary.dict_tag();
            if (dictionary.dict_tag().size() < 20) {
                std::string space(25-dictionary.dict_tag().size(),' ');
                log << MSG::INFO << space;
            }
        }
        else {
            log << MSG::INFO 
                << " DetDescr tag (using default)";
        }
        log << MSG::INFO 
            << " file " << dictionary.file_name();
        log << MSG::INFO << endreq;
    }
}

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::getFileNamesFromProperties(IProperty* propertyServer)
{

    // Check whether non-default names have been specified for the
    // IdDict files of the subsystems

    MsgStream log(messageService(), "IdDictDetDescrCnv");

    // Atlas IDs
    StringProperty property = StringProperty("AtlasIDFileName", "");
    StatusCode sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get AtlasIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_atlasIDFileName = property.value();
        std::string fileName = m_atlasIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "AtlasIDFileName:  " 
            << fileName
            << endreq;
    }

    // InDet Ids
    property = StringProperty("InDetIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get InDetIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_inDetIDFileName = property.value();
        std::string fileName = m_inDetIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "InDetIDFileName:  " 
            << fileName
            << endreq;
    }

    // LAr ids
    property = StringProperty("LArIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get LArIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_larIDFileName = property.value();
        std::string fileName = m_larIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "LArIDFileName:  " 
            << fileName
            << endreq;
    }

    // Tile ids 
    property = StringProperty("TileIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get TileIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_tileIDFileName = property.value();
        std::string fileName = m_tileIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "TileIDFileName:  " 
            << fileName
            << endreq;
    }

    // Calo ids
    property = StringProperty("CaloIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get CaloIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_caloIDFileName = property.value();
        std::string fileName = m_caloIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "CaloIDFileName:  " 
            << fileName
            << endreq;
    }

    // Calo neighbor files
    property = StringProperty("FullAtlasNeighborsFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get FullAtlasNeighborsFileName: found " 
            << property.value() << endreq;
        return sc;
    }
    else {
        m_fullAtlasNeighborsName = property.value();
        std::string fileName = m_fullAtlasNeighborsName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "FullAtlasNeighborsFileName:  " << fileName << endreq;
    }
    property = StringProperty("FCAL2DNeighborsFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get FCAL2DNeighborsFileName: found " 
            << property.value() << endreq;
        return sc;
    }
    else {
        m_fcal2dNeighborsName = property.value();
        std::string fileName = m_fcal2dNeighborsName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "FCAL2DNeighborsFileName:  " << fileName << endreq;
    }
    property = StringProperty("FCAL3DNeighborsNextFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get FCAL3DNeighborsNextFileName: found " 
            << property.value() << endreq;
        return sc;
    }
    else {
        m_fcal3dNeighborsNextName = property.value();
        std::string fileName = m_fcal3dNeighborsNextName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "FCAL3DNeighborsNextFileName:  " << fileName << endreq;
    }
    property = StringProperty("FCAL3DNeighborsPrevFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get FCAL3DNeighborsPrevFileName: found " 
            << property.value() << endreq;
        return sc;
    }
    else {
        m_fcal3dNeighborsPrevName = property.value();
        std::string fileName = m_fcal3dNeighborsPrevName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "FCAL3DNeighborsPrevFileName:  " << fileName << endreq;
    }
    property = StringProperty("TileNeighborsFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get TileNeighborsFileName: found " 
            << property.value() << endreq;
        return sc;
    }
    else {
        m_tileNeighborsName = property.value();
        std::string fileName = m_tileNeighborsName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "TileNeighborsFileName:  " << fileName << endreq;
    }

    // Muon ids
    property = StringProperty("MuonIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get MuonIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_muonIDFileName = property.value();
        std::string fileName = m_muonIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "MuonIDFileName:  " 
            << fileName
            << endreq;
    }

    // ForwardDetectors ids
    property = StringProperty("ForwardIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get ForwardIDFileName: found " 
            << property.value()
            << endreq;
        return sc;
    }
    else {
        m_forwardIDFileName = property.value();
        std::string fileName = m_forwardIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "ForwardIDFileName:  " 
            << fileName
            << endreq;
    }

    return StatusCode::SUCCESS ;
}

    

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::getFileNamesFromTags()
{

    // Fetch file names and tags from the RDB 

    MsgStream log(messageService(), "IdDictDetDescrCnv");

    const IGeoModelSvc* geoModelSvc  = 0;
    IRDBAccessSvc*      rdbAccessSvc = 0;

    // Get GeoModelSvc and RDBAccessSvc in order to get the XML filenames and tags from the 
    // database
    StatusCode sc = service ("GeoModelSvc",geoModelSvc);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "Unable to get GeoModelSvc." << endreq;
        return sc;
    }
    else log << MSG::DEBUG << "Accessed GeoModelSvc." << endreq;
          
    sc = service("RDBAccessSvc",rdbAccessSvc);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "Unable to get RDBAccessSvc." << endreq;
        return sc;
    }
    else log << MSG::DEBUG << "Accessed RDBAccessSvc." << endreq;


    // RDS: not clear what a custom tag means.
    //     if (detectorKey.custom()) {
    //  // Get from properties - below. 
    //  inDetCustom = true;
    //} else {

    // Must be sure a connection is open
    if(rdbAccessSvc->connect()) {
        // Get InDet
        DecodeVersionKey detectorKey(geoModelSvc, "InnerDetector");
        log << MSG::DEBUG << "From Version Tag: " 
            << detectorKey.tag()  << " at Node: " << detectorKey.node() << endreq;
        IRDBRecordset_ptr idDictSet   = rdbAccessSvc->getRecordsetPtr("InDetIdentifier",
								      detectorKey.tag(), 
								      detectorKey.node());
        std::string        dictName;
        const IRDBRecord*  idDictTable = 0;
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable       = (*idDictSet)[0];
            dictName          = idDictTable->getString("DICT_NAME");
            m_inDetIDFileName = idDictTable->getString("DICT_FILENAME");
            // NOTE: the internal tag for IdDict is global, but is
            // only used for InDet and thus is defined by InDet
            if(!idDictTable->isFieldNull("DICT_TAG")) {
                m_inDetIDTag      = idDictTable->getString("DICT_TAG");
            }
            m_inDetIdDictTag  = idDictSet->tagName();
            log << MSG::DEBUG << " using dictionary:  " << dictName 
                << ", file: " <<  m_inDetIDFileName  
                << ", with internal tag: " << m_inDetIDTag
                << ", dictionary tag: " << m_inDetIdDictTag
                << endreq;
        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;
        
        // Get LAr
        detectorKey = DecodeVersionKey(geoModelSvc, "LAr");
        log << MSG::DEBUG << "From Version Tag: " 
            << detectorKey.tag()  << " at Node: " << detectorKey.node() << endreq;
        idDictSet = rdbAccessSvc->getRecordsetPtr("LArIdentifier", 
                                               detectorKey.tag(), 
                                               detectorKey.node());
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable      = (*idDictSet)[0];
            dictName         = idDictTable->getString("DICT_NAME");
            m_larIDFileName  = idDictTable->getString("DICT_FILENAME");
            m_larIdDictTag   = idDictSet->tagName();
            log << MSG::DEBUG << " using Dictionary:  " << dictName 
                << ", file: " <<  m_larIDFileName
                << ", dictionary tag: " << m_larIdDictTag
                << endreq;
        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;
                        

        // Get Tile
        detectorKey = DecodeVersionKey(geoModelSvc, "TileCal");
        log << MSG::DEBUG << "From Version Tag: " 
            << detectorKey.tag()  << " at Node: " << detectorKey.node() << endreq;
        idDictSet = rdbAccessSvc->getRecordsetPtr("TileIdentifier",
                                               detectorKey.tag(), 
                                               detectorKey.node());
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable       = (*idDictSet)[0];
            dictName          = idDictTable->getString("DICT_NAME");
            m_tileIDFileName  = idDictTable->getString("DICT_FILENAME");
            m_tileIdDictTag   = idDictSet->tagName();
            log << MSG::DEBUG << " using Dictionary:  " << dictName  
                << ", file: " <<  m_tileIDFileName 
                << ", dictionary tag: " << m_tileIdDictTag
                << endreq;
        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;

        // Get Calo
        detectorKey = DecodeVersionKey(geoModelSvc, "Calorimeter");
        log << MSG::DEBUG << "From Version Tag: " << detectorKey.tag()  
            << " at Node: " << detectorKey.node() << endreq;
        idDictSet = rdbAccessSvc->getRecordsetPtr("CaloIdentifier",
                                               detectorKey.tag(), 
                                               detectorKey.node());
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable       = (*idDictSet)[0];
            dictName          = idDictTable->getString("DICT_NAME");
            m_caloIDFileName  = idDictTable->getString("DICT_FILENAME");
            m_caloIdDictTag   = idDictSet->tagName();
            log << MSG::DEBUG << " using Dictionary:  " << dictName  
                << ", file: " <<  m_caloIDFileName 
                << ", dictionary tag: " << m_caloIdDictTag
                << endreq;
        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;

        // Calo neighbor files:
        IRDBRecordset_ptr caloNeighborTable = rdbAccessSvc->getRecordsetPtr("CaloNeighborTable",
                                                                            detectorKey.tag(), 
                                                                            detectorKey.node());
        if (caloNeighborTable->size()==0) {
            caloNeighborTable = rdbAccessSvc->getRecordsetPtr("CaloNeighborTable","CaloNeighborTable-00");
        }
        // Size == 0 if not found
        if (caloNeighborTable->size()) {
            const IRDBRecord* neighborTable =  (*caloNeighborTable)[0];
            m_fullAtlasNeighborsName        = neighborTable->getString("FULLATLASNEIGHBORS");
            m_fcal2dNeighborsName           = neighborTable->getString("FCAL2DNEIGHBORS");
            m_fcal3dNeighborsNextName       = neighborTable->getString("FCAL3DNEIGHBORSNEXT");
            m_fcal3dNeighborsPrevName       = neighborTable->getString("FCAL3DNEIGHBORSPREV");
            m_tileNeighborsName             = neighborTable->getString("TILENEIGHBORS");
            log << MSG::DEBUG << " using neighbor files:  " << endreq;
            log << MSG::DEBUG << "   FullAtlasNeighborsFileName:  " << m_fullAtlasNeighborsName
                << endreq;
            log << MSG::DEBUG << "   FCAL2DNeighborsFileName:     " << m_fcal2dNeighborsName      
                << endreq;
            log << MSG::DEBUG << "   FCAL3DNeighborsNextFileName: " << m_fcal3dNeighborsNextName
                << endreq;
            log << MSG::DEBUG << "   FCAL3DNeighborsPrevFileName: " << m_fcal3dNeighborsPrevName  
                << endreq;
            log << MSG::DEBUG << "   TileNeighborsFileName:       " << m_tileNeighborsName        
                << endreq;
        }
        else log << MSG::ERROR << " no record set found neighbor file " << endreq;



        // Get Muon
        detectorKey = DecodeVersionKey(geoModelSvc, "MuonSpectrometer");
        log << MSG::DEBUG << "From Version Tag: " 
            << detectorKey.tag()  << " at Node: " << detectorKey.node() << endreq;
        idDictSet = rdbAccessSvc->getRecordsetPtr("MuonIdentifier",
                                               detectorKey.tag(), 
                                               detectorKey.node());
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable       = (*idDictSet)[0];
            dictName          = idDictTable->getString("DICT_NAME");
            m_muonIDFileName  = idDictTable->getString("DICT_FILENAME");
            m_muonIdDictTag   = idDictSet->tagName();
            log << MSG::DEBUG << " using Dictionary:  " << dictName  
                << ", file: " <<  m_muonIDFileName 
                << ", dictionary tag: " << m_muonIdDictTag
                << endreq;
            if (m_muonIDFileName == "") {
                log << MSG::ERROR << "MuonIDFileName CANNOT be empty " << endreq;
                return StatusCode::FAILURE;
            }
            else if (m_muonIDFileName == "IdDictMuonSpectrometer.xml") {
                log << MSG::ERROR << "MuonIDFileName must NOT be IdDictMuonSpectrometer.xml " 
                    << endreq;
                return StatusCode::FAILURE;
            }

        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;

        // Get Forward
        detectorKey = DecodeVersionKey(geoModelSvc, "ForwardDetectors");
        log << MSG::DEBUG << "From Version Tag: " << detectorKey.tag()
            << " at Node: " << detectorKey.node() << endreq;
        idDictSet = rdbAccessSvc->getRecordsetPtr("ForDetIdentifier",
                                               detectorKey.tag(), 
                                               detectorKey.node());
        // For older datasets use ForDetIdentifier-00 as fallback
        if (0 == idDictSet->size()) {
            idDictSet = rdbAccessSvc->getRecordsetPtr("ForDetIdentifier",
                                                   "ForDetIdentifier-00");
            log << MSG::DEBUG << " explicitly requesting ForDetIdentifier-00 tag for pre-forward detector data  " 
                << endreq;
        }
        // Size == 0 if not found
        if (idDictSet->size()) {
            idDictTable          = (*idDictSet)[0];
            dictName             = idDictTable->getString("DICT_NAME");
            m_forwardIDFileName  = idDictTable->getString("DICT_FILENAME");
            m_forwardIdDictTag   = idDictSet->tagName();
            log << MSG::DEBUG << " using Dictionary:  " << dictName  
                << ", file: " <<  m_forwardIDFileName 
                << ", dictionary tag: " << m_forwardIdDictTag
                << endreq;
        }
        else log << MSG::WARNING << " no record set found - using default dictionary " << endreq;
                        
        // Close request for this connection
        rdbAccessSvc->disconnect();
    }
    else {
        log << MSG::ERROR 
            << "unable to get connect to database for to get Identifier records" 
            << endreq;
        return (StatusCode::FAILURE);
    }

    log << MSG::DEBUG << "End access to RDB for id dictionary info " << endreq;

    return StatusCode::SUCCESS ;
}

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::registerFilesWithParser()
{
    // If files names were found, register them with the parser to be read 

    MsgStream log(messageService(), "IdDictDetDescrCnv");

    if ("" != m_atlasIDFileName) {
        m_parser->register_external_entity("ATLAS", m_atlasIDFileName);
        log << MSG::INFO << "Reading ATLAS            IdDict file "
            << m_atlasIDFileName
            << endreq;
    }
    if ("" != m_inDetIDFileName) {
        m_parser->register_external_entity("InnerDetector", m_inDetIDFileName);
        log << MSG::INFO << "Reading InnerDetector    IdDict file "
            << m_inDetIDFileName
            << endreq;
    }
    if ("" != m_larIDFileName) {
        m_parser->register_external_entity("LArCalorimeter", m_larIDFileName);
        log << MSG::INFO << "Reading LArCalorimeter   IdDict file "
            << m_larIDFileName
            << endreq;
    }
    if ("" != m_tileIDFileName) {
        m_parser->register_external_entity("TileCalorimeter", m_tileIDFileName);
        log << MSG::INFO << "Reading TileCalorimeter  IdDict file "
            << m_tileIDFileName
            << endreq;
    }
    if ("" != m_caloIDFileName) {
        m_parser->register_external_entity("Calorimeter", m_caloIDFileName);
        log << MSG::INFO << "Reading Calorimeter      IdDict file "
            << m_caloIDFileName
            << endreq;
    }
    if ("" != m_muonIDFileName) {
        m_parser->register_external_entity("MuonSpectrometer", m_muonIDFileName);
        log << MSG::INFO << "Reading MuonSpectrometer IdDict file "
            << m_muonIDFileName
            << endreq;
    }
    if ("" != m_forwardIDFileName) {
        m_parser->register_external_entity("ForwardDetectors", m_forwardIDFileName);
        log << MSG::INFO << "Reading ForwardDetectors IdDict file "
            << m_forwardIDFileName
            << endreq;
    }

    return StatusCode::SUCCESS ;
}

//--------------------------------------------------------------------
StatusCode
IdDictDetDescrCnv::registerInfoWithDicts()
{
    // Save the file name and tag in each of the dictionaries
    
    MsgStream log(messageService(), "IdDictDetDescrCnv");

    IdDictMgr& mgr = m_parser->m_idd;

    if ("" != m_atlasIDFileName) {
        // Find Atlas:
        IdDictDictionary* dict = mgr.find_dictionary("ATLAS");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for ATLAS" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_atlasIDFileName);
        dict->set_dict_tag (m_atlasIdDictTag);
        log << MSG::DEBUG << "For ATLAS idDict, setting file/tag: "
            << m_atlasIDFileName << " " << m_atlasIdDictTag
            << endreq;
    }
    if ("" != m_inDetIDFileName) {
        // Find Indet:
        IdDictDictionary* dict = mgr.find_dictionary("InnerDetector");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for InnerDetector" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_inDetIDFileName);
        dict->set_dict_tag (m_inDetIdDictTag);
        log << MSG::DEBUG << "For InnerDetector idDict, setting file/tag: "
            << m_inDetIDFileName << " " << m_inDetIdDictTag
            << endreq;
    }
    if ("" != m_larIDFileName) {
        // Find LAr:
        IdDictDictionary* dict = mgr.find_dictionary("LArCalorimeter");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for LArCalorimeter" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_larIDFileName);
        dict->set_dict_tag (m_larIdDictTag);
        log << MSG::DEBUG << "For LArCalorimeter idDict, setting file/tag: "
            << m_larIDFileName << " " << m_larIdDictTag
            << endreq;
    }
    if ("" != m_tileIDFileName) {
        // Find Tile:
        IdDictDictionary* dict = mgr.find_dictionary("TileCalorimeter");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for TileCalorimeter" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_tileIDFileName);
        dict->set_dict_tag (m_tileIdDictTag);
        log << MSG::DEBUG << "For TileCalorimeter idDict, setting file/tag: "
            << m_tileIDFileName << " " << m_tileIdDictTag
            << endreq;
    }
    if ("" != m_caloIDFileName) {
        // Find Calo:
        IdDictDictionary* dict = mgr.find_dictionary("Calorimeter");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for Calorimeter" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_caloIDFileName);
        dict->set_dict_tag (m_caloIdDictTag);
        log << MSG::DEBUG << "For Calorimeter idDict, setting file/tag: "
            << m_caloIDFileName << " " << m_caloIdDictTag
            << endreq;
    }
    if ("" != m_fullAtlasNeighborsName) {
        // Set neighbor file name:
        mgr.add_metadata("FULLATLASNEIGHBORS", m_fullAtlasNeighborsName);  
        log << MSG::DEBUG << "Added to dict mgr meta data: <FULLATLASNEIGHBORS, " 
            << m_fullAtlasNeighborsName << endreq;
    }
    if ("" != m_fcal2dNeighborsName) {
        // Set neighbor file name:
        mgr.add_metadata("FCAL2DNEIGHBORS", m_fcal2dNeighborsName);  
        log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL2DNEIGHBORS, " 
            << m_fcal2dNeighborsName << endreq;
    }
    if ("" != m_fcal3dNeighborsNextName) {
        // Set neighbor file name:
        mgr.add_metadata("FCAL3DNEIGHBORSNEXT", m_fcal3dNeighborsNextName);  
        log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL3DNEIGHBORSNEXT, " 
            << m_fcal3dNeighborsNextName << endreq;
    }
    if ("" != m_fcal3dNeighborsPrevName) {
        // Set neighbor file name:
        mgr.add_metadata("FCAL3DNEIGHBORSPREV", m_fcal3dNeighborsPrevName);  
        log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL3DNEIGHBORSPREV, " 
            << m_fcal3dNeighborsPrevName << endreq;
    }
    if ("" != m_tileNeighborsName) {
        // Set neighbor file name:
        mgr.add_metadata("TILENEIGHBORS", m_tileNeighborsName);  
        log << MSG::DEBUG << "Added to dict mgr meta data: <TILENEIGHBORS, " 
            << m_tileNeighborsName << endreq;
    }
    if ("" != m_muonIDFileName) {
        // Find Muon:
        IdDictDictionary* dict = mgr.find_dictionary("MuonSpectrometer");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for MuonSpectrometer" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_muonIDFileName);
        dict->set_dict_tag (m_muonIdDictTag);
        log << MSG::DEBUG << "For MuonSpectrometer idDict, setting file/tag: "
            << m_muonIDFileName << " " << m_muonIdDictTag
            << endreq;
    }
    if ("" != m_forwardIDFileName) {
        // Find Forward:
        IdDictDictionary* dict = mgr.find_dictionary("ForwardDetectors");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for ForwardDetectors" 
                << endreq;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_forwardIDFileName);
        dict->set_dict_tag (m_forwardIdDictTag);
        log << MSG::DEBUG << "For ForwardDetectors idDict, setting file/tag: "
            << m_forwardIDFileName << " " << m_forwardIdDictTag
            << endreq;
    }

    return StatusCode::SUCCESS ;
}

