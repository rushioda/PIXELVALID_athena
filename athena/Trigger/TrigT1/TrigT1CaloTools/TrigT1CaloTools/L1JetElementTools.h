/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JetElementTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JETELEMENTTOOLS_H
#define LVL1L1JETELEMENTTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1JetElementTools

      This is a tool to reconstruct the L1 Jet trigger sums
      for a particular RoI location from the stored JetInputs.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1JetElementTools->mapTowers() // set up event for analysis
              L1JetElementTools->formSums(eta, phi) // for each RoI
              L1JetElementTools->ET8x8() // etc. Return values
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JetElementTools : virtual public IL1JetElementTools, public AthAlgTool
    {
    public:
      
      L1JetElementTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1JetElementTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /** Fill DataVector of JetElements from user-supplied TriggerTowers */
      virtual void makeJetElements(const xAOD::TriggerTowerContainer* tts, xAOD::JetElementContainer* jetElements);
      virtual void mapJetElements(const xAOD::JetElementContainer* jetElements, std::map<int, xAOD::JetElement*>* map);

      // @DEPRICATED(amazurov)
      virtual void makeJetElements(const DataVector<TriggerTower>* tts, DataVector<JetElement>* jetElements);
            
    private:

     /** Trigger Tower and Jet Element properties */
     static const int m_ttSat = 0xFF;
     static const int m_layerSat = 0x1FF;
             
    }; 
} // end of namespace

#endif 
