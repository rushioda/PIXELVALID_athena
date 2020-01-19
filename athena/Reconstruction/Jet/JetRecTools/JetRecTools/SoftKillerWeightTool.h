// this file is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SoftKillerWeightTool.h

#ifndef JETRECTOOLS_SOFTKILLERWEIGHTTOOL_H
#define JETRECTOOLS_SOFTKILLERWEIGHTTOOL_H



// \class SoftKillerWeightTool
// \author Jennifer Roloff
// \date November 2015
//
// This is the tool to reweight clusters according to the SoftKiller
// algorithm
// It creates a new cluster collection based on the original clusters
// and the grid spacing for SK
//
//
// Properties are as follows:
//   OutputContainer: Name of the output cluster collection
//   InputCollection: Name of the input cluster collection. 
//   GridSize: The grid size that should be applied for the SK 
//   	 algorithm. Suggested values between 0.3 and 0.6
//   SKRapMin: The minimum (absolute) rapidity over which to calculate SK
//   SKRapMax: The maximum (absolute) rapidity over which to calculate SK
//   SKRapMinApplied: The minimum (absolute) rapidity over which to apply SK
//   SKRapMaxApplied: The maximum (absolute) rapidity over which to apply SK
//   isCaloSplit: If false, SK is run the same on all clusters. If
//   	 true, SK is run separately for clusters in the ECal and the
//   	 HCal.
//   ECalGridSize: Only necessary if isCaloSplit == true. The SK grid spacing
//   	 used for the ECal.
//   HCalGridSize: Only necessary if isCaloSplit == true. The SK grid spacing
//     used for the HCal.
//
//
// This tool may be used on its own or in sequence with other pileup
// suppression methods. For central jets, it is suggested that the
// input collection be clusters with area subtraction (such as Voronoi)
// applied.
//




#include <string>
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"
#include "fastjet/contrib/SoftKiller.hh"



class SoftKillerWeightTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(SoftKillerWeightTool, IJetConstituentModifier)

  public:
  
  SoftKillerWeightTool(const std::string& name);
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::CaloClusterContainer* cont) const; // MEN: Might need to rename this process
  float calculateWeight(xAOD::CaloCluster* cl) const;
  float calculateSplitWeight(xAOD::CaloCluster* cl) const;
  void RunClusters(xAOD::CaloClusterContainer* m_clust) const;
  void RunSplitClusters(xAOD::CaloClusterContainer* m_clust) const;

  //		int execute() const;


private:

  double findMinPt(std::vector<fastjet::PseudoJet> *clustSK) const;
  std::vector<fastjet::PseudoJet> makeSKClust(std::vector<fastjet::PseudoJet> clustPJ) const;


private:

  //mutable int m_initCount;
  float m_lambdaCalDivide;

  // Properties.
  bool m_isCaloSplit; 
  float m_gridSpacing; 
  float m_eCalGrid;
  float m_hCalGrid;
  float m_rapmin;
  float m_rapmax;
  float m_rapminApplied;
  float m_rapmaxApplied;
  mutable double m_minPt;
  mutable double m_minPtECal;
  mutable double m_minPtHCal;
		
};


#endif
