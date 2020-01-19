/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZdcRec.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ZDCRECV3_H
#define ZDCRECV3_H

#include <string>
#include <map>



//which one ???
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"



//#include "ZdcRec/ZdcRecNoiseTool.h"
//#include "ZdcRec/ZdcRecChannelTool.h"

class ISvcLocator;
class StatusCode;
class Identifier;
class StoreGateSvc;

class ZdcRecChannelToolV2;
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "ZdcAnalysis/ZdcAnalysisTool.h"

/** @class ZdcRec

    Class definition for the ZDC Reconstruction class

    @author  Brian Cole and Peter Steinberg, bcole@cern.ch, peter.steinberg@bnl.gov
             and M. Leite leite@cern.ch
*/


class ZdcRecV3 : public AthAlgorithm
{

public:

	ZdcRecV3(const std::string& name, ISvcLocator* pSvcLocator);
	~ZdcRecV3();

	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

private:

  /** class member version of retrieving StoreGate */
	//StoreGateSvc* m_storeGate;
	 ServiceHandle<StoreGateSvc> m_storeGate;


	/** Does the collection own it's objects ? **/
	int m_ownPolicy;


	/** Digits data container name */
	std::string m_ttContainerName;

	/** Raw data object name */
	std::string m_zdcModuleContainerName;
	std::string m_zdcModuleAuxContainerName;


	/** Pointer to Zdc input "digits" data */
	const xAOD::TriggerTowerContainer* m_ttContainer;

	int m_eventCount;
	bool m_complainContain;
	bool m_complainRetrieve;

	//Include here all tools to do the job. They will be called by the algorithm execute method
	//Another option is to use ToolHandleArray<IZdcRecTool>, where IZdcRecTool is the factory for
	//the tools
	ToolHandle<ZdcRecChannelToolV2> m_ChannelTool;
	ToolHandle<ZDC::IZdcAnalysisTool> m_zdcTool;

};

#endif
