/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Generated by Together */

/* author : Johann Collot */

/* date of creation : 30/01/2001 */

/* date of last modification :                                                   */
/*  23/05/2001 :                                                                 */
/*  13/09/2002 : corrections of comments + include file moved to definition file */

#ifndef LARHITMAKER_H
#define LARHITMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

/**
 * Specialized ATHENA algorithm which can be used to store <br>
 * liquid argon hit collections in the Athena TES. <p>
 * 
 * LArHitMaker owns a sub-algorithm ( LArHitProducer) , which creates <br>
 *  and subsequently fills a LArHitContainer.  <p>
 * 
 * The  LArHit collections are produced by a LArHitProducer <br>
 * are then stored in the Transient Event Store by LArHitMaker <br>
 * using StoreGate . <p>
 * @author Johann Collot
 * @version 00-00-05
 *
 */
class LArHitMaker : public AthAlgorithm {

public:

    /** Constructor */
    LArHitMaker(const std::string & name, ISvcLocator * pSvcLocator);

    /** Destructor */
    virtual ~LArHitMaker() { }

public:

    /** Initialize method , executed once  at the beginning <br> of the job execution by the control framework . <br> */
    virtual StatusCode initialize();

    /** execute method executed for each event  <br> by the control framework . <br> */
    virtual StatusCode execute();

    /** finalize method executed at the end of  <br> the job execution by the control framework . <br> */
    virtual StatusCode finalize();

private:


    /** Instance Name and Type of the hit producer sub-algorithm. <br> This is a property of LArHitMaker <br><br>
     * it is set by the Athena string property HitRetrieverNameAndType <br> 
     * default value = LArHitProducerTest/producer1 <br>*/
    std::string m_HitRetrieverNameAndType;

    /**
     * Location in the TES where the hit collection, once retrieved or produced, will be stored .<br>
     * This is a property of LArHitMaker <br><br>
     * It is set by the Athena string property HitContainerLocation <br>
     * default value = LArHits <br>
     */
    std::string m_HitContainerLocation;

    /**
     * Pointer to the sub-algorithm which produces the hits .<p>
     * 
     * This sub-algorithm is a derived class of LArHitProducer <br>
     * Example : LArHitProducerTest in LArSim <br>
     * @clientCardinality 1
     * @supplierCardinality 1
     * @supplierRole hit retrieval/production
     * @link aggregation
     */
    Algorithm * m_HitRetrieverAlgorithm;

    /**
     * debug Flag <br><br>
     * Property of LArHitMaker <br>
     * Avoids printout of the container content if set to 0 <br>
     * It is set by the Athena integer proterty DebugFlag <br>
     * default value = 0 ( no printout )<br>
     */
    int m_DebugFlag;

    /** find Algorithm type in string property */
    std::string findType(std::string l_property);

    /** find instance name of Algorithm in string property */
    std::string findName(std::string l_property);

    /**
     * @link dependency
     * @clientRole stores in TDS
     */

    /*#  LArHitContainer lnkLArHitContainer; */

    /**
     * @link aggregation
     * @clientCardinality 1
     * @supplierCardinality 1
     * @supplierRole production/retrieval of hits
     */

    /*#  LArHitProducer lnkLArHitProducer; */
};
#endif     //LARHITMAKER_H



