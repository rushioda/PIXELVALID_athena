/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_CELLFINDER_H
#define DITAUREC_CELLFINDER_H

#include "DiTauToolBase.h"

#include "GaudiKernel/ToolHandle.h"


class CellFinder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 CellFinder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~CellFinder();

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);


 virtual void cleanup(DiTauCandidateData *) { }
 

private:
  bool m_bWriteJetCells;
  bool m_bWriteSubjetCells;
  std::string m_ClusterContainerName;
  std::string m_CellContainerName;
  float m_Rsubjet;


};

#endif  /* CELLFINDER_H */

