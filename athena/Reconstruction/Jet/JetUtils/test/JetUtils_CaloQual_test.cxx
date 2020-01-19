/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "JetUtils/JetCaloQualityUtils.h"

#include "ClusterData.h"

#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
#else
// Done like this so that this will compile in both releases 20 and 21.
# include "AthLinks/ElementLink.h"
# include "SGTools/CLASS_DEF.h"
# ifdef SGTOOLS_CURRENTEVENTSTORE_H
#  include "SGTools/TestStore.h"
# endif
#endif

template< class T>
struct CalcTester {
  
  T m_calc;
};

#define TEST_MSG( msg ) std::cout << msg << std::endl

using namespace jet;


int main() {

#ifdef ROOTCORE
  // Create a TEvent object:
  // xAOD::TEvent eventstore(xAOD::TEvent::kClassAccess);
#else
# ifdef SGTOOLS_CURRENTEVENTSTORE_H
  SGTest::initTestStore();
# endif
#endif


  JetCaloCalculations calculators(false);
#define ADDCALC( klass, vname) klass vname; vname.setName( #vname ); calculators.addCalculator(&vname)


  // Create some calculators add them to the JetCaloCalculations instance.
  ADDCALC( JetCalcnLeadingCells ,nLeadCell);
  ADDCALC( JetCalcOutOfTimeEnergyFraction,  ootFrac);
  ADDCALC( JetCalcAverageLArQualityF , avgLArQF);
  ADDCALC( JetCalcQualityHEC , hecQual);
  ADDCALC( JetCalcNegativeEnergy , negE);

  JetCalcQualityHEC hec2;

  /// Read data from reference AOD files
  const char* refdir_s = getenv("D3PDTESTDATA");

  std::string refdir_mc = refdir_s ? refdir_s : "/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/AOD";
  std::string refdir_data = refdir_s ? refdir_s : "/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/AOD";

  ClusterData::fillContainer(refdir_mc + "/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.AOD.e1728_s1581.19_1_1_5.039772.pool.root.1");
  ClusterData::fillContainer(refdir_data + "/data12_8TeV.00209109.physics_JetTauEtmiss.merge.AOD.19_1_1_5._lb0186._SFO-1._0001.pool.root.1");
  //vector<JetCaloCalculator*> &vcalc = calculators.calculators();


  // loop over jet and test calculations
#define TESTCALC( calc) valuesDirect.push_back(calc( jet )); TEST_MSG(calc.name() <<" = " <<  valuesDirect.back() ) 
  for(xAOD::Jet* jet : ClusterData::jetCont){

    // calculate from JetCalculations    
    std::vector<double>  valuesFromC =calculators.process( jet );

    // calculate directly the values from the ::operator() of the calculators.
    std::vector<double> valuesDirect;
    TESTCALC(nLeadCell);
    TESTCALC(ootFrac);
    TESTCALC(avgLArQF);
    TESTCALC(hecQual);
    TESTCALC(negE);

    size_t nval = valuesDirect.size();
    assert( nval == valuesFromC.size() );

    // test equality 
    for(size_t v=0;v<nval;v++){
      if( valuesFromC[v] != valuesDirect[v] ){
        TEST_MSG( "FAILURE at calculator "<<calculators.at(v)->name() );
        TEST_MSG( "direct value = "<< valuesDirect[v] <<  "  from C ="<< valuesFromC[v] ) ;
        return 1;
      }
    }

  } // end loop on jets
  
  
} // main()

// #else // XAOD_STANDALONE
// int main(){return 0;}
// #endif
