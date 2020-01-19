/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMdtRdoToPrepDataTool_H
#define MUONMdtRdoToPrepDataTool_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <string>
class AtlasDetectorID;
class Identifier;
class MdtIdHelper;
class MdtDigit;
class MdtCalibrationSvc;
class MuonMDT_CablingSvc;
class MdtCalibrationSvcSettings;
class MdtCalibHit;
//class MuonMDT_CablingSvc;
//class MdtRDO_Decoder;
class MdtCsm;
class StoreGateSvc;
//class MdtCalibrationDbSvc; //TWIN TUBES


namespace MuonGM
{    
    class MuonDetectorManager;
    class MdtReadoutElement;
}


namespace Muon 
{

  class IMuonRawDataProviderTool;
  class IMDT_RDO_Decoder;
  class MuonIdHelperTool;

  /** @class MdtRdoToPrepDataTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  

  class MdtRdoToPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
    {
    public:
      MdtRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~MdtRdoToPrepDataTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
      StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect );
      //new decode methods for Rob based readout
      StatusCode decode( const std::vector<uint32_t>& robIds, const std::vector<IdentifierHash>& chamberHashInRobs );
      StatusCode decode( const std::vector<uint32_t>& robIds );
     
      StatusCode processCsm(const MdtCsm *rdoColl, std::vector<IdentifierHash>& idWithDataVect);

      Muon::MdtDriftCircleStatus getMdtDriftRadius(const MdtDigit * digit, double& radius, double& errRadius, const MuonGM::MdtReadoutElement * descriptor);
 
      // + TWIN TUBE
      StatusCode processCsmTwin(const MdtCsm *rdoColll, std::vector<IdentifierHash>& idWithDataVect);
      // method to get the twin tube 2nd coordinate
      Muon::MdtDriftCircleStatus getMdtTwinPosition(const MdtDigit * prompt_digit, const MdtDigit * twin_digit, 
                    double& radius, double& errRadius, 
                    double& zTwin, double& errZTwin, bool& twinIsPrompt);
            
      // - TWIN TUBE

      // dump methods for debugging
      void printInputRdo();
      void printPrepData();
      
    private:
            
  
        /// store gate transactions
        StoreGateSvc * m_sgSvc;
        
        /// Muon Detector Descriptor
        const MuonGM::MuonDetectorManager * m_muonMgr;
        
        /// MDT identifier helper
        const MdtIdHelper * m_mdtHelper;
        
        /// MDT calibration service
        MdtCalibrationSvc* m_calibrationSvc;
        MdtCalibrationSvcSettings* m_mdtCalibSvcSettings; 
        MdtCalibHit* m_calibHit;
        double m_invSpeed;

        /// MdtPrepRawData containers
        Muon::MdtPrepDataContainer * m_mdtPrepDataContainer;
        std::string m_outputCollectionLocation;        
        
        /** member variables for algorithm properties: */
        /**If true, then 'fix' Identifiers from RDOs*/
        bool m_doIdentifierCorrection;
        bool m_calibratePrepData; //!< toggle on/off calibration of MdtPrepData
        bool m_decodeData; //!< toggle on/off the decoding of MDT RDO into MdtPrepData

        bool m_useBStoRdoTool;

        /** class member version of retrieving MsgStream */
    //        MsgStream* m_log;
        //bool m_debug;
        //bool m_verbose;

//         // Rob Data Provider handle
//         ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;
        // handle tp the MDT_RawDataProviderTool
        ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool;
        ToolHandle<Muon::IMDT_RDO_Decoder> m_mdtDecoder;
	ToolHandle<Muon::MuonIdHelperTool> m_idHelper;

	//retreive the cabling svc
      	MuonMDT_CablingSvc* m_mdtCabling;

        //keepTrackOfFullEventDecoding
        bool m_fullEventDone;

        bool m_isTestBeam; //!< Support for test beam data (twin tubes)

        bool m_BMEpresent;
        bool m_BMGpresent;
        int m_BMEid;
        int m_BMGid;

        // + TWIN TUBE
        bool   m_useTwin;
        bool   m_useAllBOLTwin;
        bool   m_use1DPrepDataTwin;
        bool   m_twinCorrectSlewing;
        bool   m_discardSecondaryHitTwin;
        //  const MdtCalibrationDbSvc* m_dbSvc;
        int twin_chamber[2][3][36];
        int secondaryHit_twin_chamber[2][3][36];
        // - TWIN TUBE

        std::map<Identifier, std::vector<Identifier> > m_DeadChannels;
        void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

    }; 
} // end of namespace

#endif 
