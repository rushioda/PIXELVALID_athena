/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Takashi Kubota
/// Tokyo, May 2008
///
/// algorithm to decode RDO into PrepRawData

#include "MuonRdoToPrepData/TgcRdoToTgcPrepData.h"
#include "Identifier/IdentifierHash.h"

TgcRdoToTgcPrepData::TgcRdoToTgcPrepData(const std::string& name, ISvcLocator* pSvcLocator) 
  :
  AthAlgorithm(name, pSvcLocator),
  m_tool( "Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"), // 'this' as 2nd arg would make it private tool
  m_print_inputRdo(false),
  m_print_prepData(false),
  m_setting(0)
{
  declareProperty("DecodingTool",       m_tool,       "tgc rdo to prep data conversion tool" );
  declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
  declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
  declareProperty("Setting",            m_setting,        "0 is default unseeded decoding"); 
  // m_setting=314321 means 
  // Execution #0 is mode=1 unseeded mode
  // Execution #1 is mode=2 seeded mode (1578 hash vectors with just one hash looping over 0 to 1577)
  // Execution #2 is mode=3 seeded mode (just one hash vector with hashes of 0 to 1577)
  // Execution #3 is mode=4 seeded mode (just one hash vector with hashes of 0 to 1577 and 0 to 1577, intentional duplication!)
  // Execution #4 is mode=1 unseeded mode
  // Execution #5 is mode=3 seeded mode (just one hash vector with hashes of 0 to 1577)
}  

StatusCode TgcRdoToTgcPrepData::finalize() {
  ATH_MSG_DEBUG( "in finalize()"  );
  return StatusCode::SUCCESS;
}

StatusCode TgcRdoToTgcPrepData::initialize(){
  ATH_MSG_DEBUG( " in initialize()"  );
    
  // verify that our tool handle is pointing to an accessible tool
  ATH_CHECK( m_tool.retrieve() );
  ATH_MSG_INFO( "Retrieved " << m_tool  );

  // Debug setting 
  int tmpSetting = m_setting;
  while(tmpSetting>0) {
    int mode = tmpSetting%10; 
    // 1: unseeded mode
    // 2: seeded mode (1578 hash vectors with just one hash looping over 0 to 1577)
    // 3: seeded mode (just one hash vector with hashes of 0 to 1577)
    // 4: seeded mode (just one hash vector with hashes of 0 to 1577 and 0 to 1577, intentional duplication!)
    // (BigWheel:1488, SmallWheel:90)
    if(mode==1 || mode==2 || mode==3 || mode==4) m_settingVector.push_back(mode);
    tmpSetting /= 10;
  }
  if(!m_settingVector.empty()) {
    ATH_MSG_INFO( "Debug setting is as follows (" << m_setting << ") :"  );
    for(unsigned int iExec=0; iExec<m_settingVector.size(); iExec++) {
      if(m_settingVector.at(iExec)==1) {
	ATH_MSG_INFO( "Execution #" << iExec << ": unseeded mode (mode=1)"  );
      } else if(m_settingVector.at(iExec)==2) { 
	ATH_MSG_INFO( "Execution #" << iExec << 
                      ": seeded mode (1578 hash vectors with just one hash looping over 0 to 1577) (mode=2)"  );
      } else if(m_settingVector.at(iExec)==3) { 
	ATH_MSG_INFO( "Execution #" << iExec << 
                      ": seeded mode (just one hash vector with hashes of 0 to 1577) (mode=3)"  );
      } else if(m_settingVector.at(iExec)==4) {
	ATH_MSG_INFO( "Execution #" << iExec << 
                      ": seeded mode (just one hash vector with hashes of 0 to 1577 and 0 to 1577, intentional duplication!) (mode=4)"  );
      }
    }
  }
    
  return StatusCode::SUCCESS;
}

StatusCode TgcRdoToTgcPrepData::execute() {

  ATH_MSG_DEBUG( "**************** in TgcRdoToTgcPrepData::execute() ***********************************************"  );
  ATH_MSG_DEBUG( "in execute()"  );
    
  if(m_settingVector.empty()) { // Default
    // for the test of "unseeded mode" in MuonTGC_CnvTools/src/TgcRdoToTgcPrepDataTool
    std::vector<IdentifierHash> myVector(0); // empty vector
    std::vector<IdentifierHash> myVectorWithData;
    ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );

    if (m_print_inputRdo) m_tool->printInputRdo(); // print input TGC RDO
    if (m_print_prepData) m_tool->printPrepData(); // print output TGC PRD

    return StatusCode::SUCCESS;;
  } 

  // Debugging of TgcRdoToPrepDataTool
  ATH_MSG_WARNING( "!!! DEBUGGING of TgcRdoToPrepDataTool by TgcRdoToTgcPrepData.cxx !!!" 
                   << " m_setting=" << m_setting << " m_settingVector.size()=" << m_settingVector.size() 
                   );

  unsigned int maxHashId = 1578; // TGC IdHash max = 1578 (BigWheel:1488, SmallWheel:90)
  std::vector<IdentifierHash> myFoundHashVector;
  std::vector<std::vector<IdentifierHash> > myFoundHashVectors; 
  for(unsigned int iExec=0; iExec<m_settingVector.size(); iExec++) {
    int mode = m_settingVector.at(iExec);
    myFoundHashVector.clear();
    if(mode==1) { // 1: unseeded mode
      std::vector<IdentifierHash> myRequestedHashVector(0);
      ATH_CHECK( m_tool->decode(myRequestedHashVector, myFoundHashVector) );
    } else if(mode==2) { // 2: seeded mode (1578 hash vectors with just one hash looping over 0 to 1577)
      std::vector<IdentifierHash> myRequestedHashVector(1);
      std::vector<IdentifierHash> tmpFoundHashVector;
      for(unsigned int hashId=0; hashId<maxHashId; hashId++) {
	myRequestedHashVector.at(0) = IdentifierHash(hashId);
	ATH_CHECK( m_tool->decode(myRequestedHashVector, tmpFoundHashVector) );
	if(!tmpFoundHashVector.empty()) {
	  for(unsigned int iHash=0; iHash<tmpFoundHashVector.size(); iHash++) {
	    myFoundHashVector.push_back(tmpFoundHashVector.at(iHash));
	  }
	}
      }
    } else if(mode==3) { // 3: seeded mode (just one hash vector with hashes of 0 to 1577)
      std::vector<IdentifierHash> myRequestedHashVector(maxHashId);
      for(unsigned int hashId=0; hashId<maxHashId; hashId++) {
	myRequestedHashVector.at(hashId) = IdentifierHash(hashId);
      }
      ATH_CHECK( m_tool->decode(myRequestedHashVector, myFoundHashVector) );
    } else if(mode==4) { // 4: seeded mode (just one hash vector with hashes of 0 to 1577 and 0 to 1577, intentional duplication!)
      std::vector<IdentifierHash> myRequestedHashVector(maxHashId);
      for(unsigned int hashId=0; hashId<maxHashId; hashId++) {
	myRequestedHashVector.at(hashId) = IdentifierHash(hashId);
      }
      for(unsigned int hashId=0; hashId<maxHashId; hashId++) { 
	myRequestedHashVector.at(hashId) = IdentifierHash(hashId); // For intentional duplication!
      }
      ATH_CHECK( m_tool->decode(myRequestedHashVector, myFoundHashVector) );
    }
    myFoundHashVectors.push_back(myFoundHashVector); 
  }

  // Check if the numbers of found hashes are consistent 
  for(unsigned int iExec=1; iExec<m_settingVector.size(); iExec++) {
    if(myFoundHashVectors.at(0).size()!=myFoundHashVectors.at(iExec).size()) {
      ATH_MSG_WARNING( "The numbers of found hashes are different: " 
                       << "Execution #0 (mode=" << m_settingVector.at(0) << ") : " << myFoundHashVectors.at(0).size() << " "
                       << "Execution #" << iExec << " (mode=" << m_settingVector.at(iExec) << ") : " 
                       << myFoundHashVectors.at(iExec).size()  );
    }
  }

  // Check if found hashes are consistent 
  for(unsigned int iExec=1; iExec<m_settingVector.size(); iExec++) {
    for(unsigned int iHash=0; iHash<myFoundHashVectors.at(0).size(); iHash++) {
      bool found = false;
      for(unsigned int jHash=0; jHash<myFoundHashVectors.at(iExec).size(); jHash++) {
	if(myFoundHashVectors.at(0).at(iHash)==myFoundHashVectors.at(iExec).at(jHash)) {
	  found = true;
	  break;
	}
      }
      if(!found) {
	ATH_MSG_WARNING( "Hash " << static_cast<unsigned int>(myFoundHashVectors.at(0).at(iHash)) 
                         << " is found by Execution #0 (mode=" << m_settingVector.at(0) 
                         << ") but is not found by Execution #" << iExec << " (mode=" 
                         << m_settingVector.at(iExec) << ")"  );
      }
    }
  }

  return StatusCode::SUCCESS;

}

