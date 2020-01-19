/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MuonSort.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  algorithm to create sorted lists for muons, et order applied
//
#include "L1TopoAlgorithms/MuonSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/MuonTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(MuonSort)

bool SortByEtLargestM(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}


// constructor
TCS::MuonSort::MuonSort(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 32 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 6);
   defineParameter( "MinEta", 0);
   defineParameter( "MaxEta", 7);
}


// destructor
TCS::MuonSort::~MuonSort() {}


TCS::StatusCode
TCS::MuonSort::initialize() {
   m_numberOfMuons = parameter("OutputWidth").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::MuonSort::sort(const InputTOBArray & input, TOBArray & output) {

   const MuonTOBArray & muons = dynamic_cast<const MuonTOBArray&>(input);

   // fill output array with GenericTOB buildt from muons
   for(MuonTOBArray::const_iterator cl = muons.begin(); cl!= muons.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      if (parType_t(std::abs((*cl)-> eta())) < m_minEta) continue; 
      if (parType_t(std::abs((*cl)-> eta())) > m_maxEta) continue;
      
      output.push_back( gtob );
   }

   // sort
   output.sort(SortByEtLargestM);
   

   // keep only max number of muons
   int par = m_numberOfMuons;
   unsigned int maxNumberOfMuons = (unsigned int)(par<0?0:par);
   if(maxNumberOfMuons>0) {
      while( output.size()> maxNumberOfMuons ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

