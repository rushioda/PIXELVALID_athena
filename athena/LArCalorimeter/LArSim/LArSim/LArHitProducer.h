/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Generated by Together */

/* author : Johann Collot */

/* date of creation : 31/01/2001 */
/* modifications :                                                                              */
/*  10/06/2001 :                                                                                */
/*  13/09/2002 : corrections of comments                                                        */
/*  04/10/2002 : corrections of comments                                                        */

#ifndef LARHITPRODUCER_H
#define LARHITPRODUCER_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArSimEvent/LArHitContainer.h"

/**
 * ATHENA specialized sub-algorithm which is used <br>
 * to produce liquid argon hits (LArHits) . <br><br>
 * In fact, a user has to write a doughter of this class <br>
 * in which only the hitconstruction() method <br>
 * needs to be implemented. <br><br> 
 * 
 * A LArHitContainer instance is built for each execution <br> 
 * of the execute() method. <br><br> 
 * 
 * Access to the new LArHitContainer is provided through hitcontainer() . <br><br>
 *
 * LArFakeClusterProducer in LArSim is an example of a user version of this class  . <br>
 * @author Johann Collot
 * @version 01-00-00 , remove methods to create specialized hits
 *
 */
class LArHitProducer : public AthAlgorithm {

public:

    /** usual ATHENA constructor of an algorithm*/
    LArHitProducer(const std::string & name, ISvcLocator * pSvcLocator);

    /** Destructor */
    virtual ~LArHitProducer() { }

    /** Initialize method , executed once at the beginning <br> of the job execution by the control framework */
    virtual StatusCode initialize();

    /** execute() method executed for each event <br> by the LArHitMaker instance which owns <br> this producer */
    virtual StatusCode execute();

    /** finalize() method executed at the end <br> of the job execution by the control framework */
    virtual StatusCode finalize();

    /** return a pointer to the LArHitContainer <br> in which the hits are produced */
    virtual LArHitContainer * hitContainer() const { return m_hitcontainer; }

    /**
     * Method in which the hits are produced <br><br> This method must be redefined in the doughter <br>
     * specializations of this class, to accommodate the <br> specific code
     */
    virtual StatusCode hitConstruction() { return StatusCode::SUCCESS; }

    

    /**
     * creates a new LArHit <br>
     */
    virtual LArHit * newLArHit(Identifier l_cell, double l_energy, double l_time);

private:

    /** LArHitContainer in which the produced hits will be stored
     * @supplierCardinality 1
     * @clientCardinality 1
     * @clientRole creates */
    LArHitContainer * m_hitcontainer;

};
#endif    //LARHITPRODUCER_H


