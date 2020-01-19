/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdint.h>

#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigNavStructure/ComboIterator.h"
#include "testutils.h"
using namespace HLT;
StandaloneNavigation tns;
namespace ID {
  const te_id_type trk=22;
  //const te_id_type mu=33;
  const te_id_type trk2=44;
  
  const te_id_type topoCalo=7000;
  const te_id_type topoMu=8000;
  const te_id_type topoMuCalo=9000;
}
TriggerElement* makeTopoConnections(TriggerElement* t1, TriggerElement* t2, unsigned id) {
  std::vector<TriggerElement*> seeds;
  seeds.push_back(t1);
  seeds.push_back(t2);
  return tns.addNode(seeds, id);    
}

void prepareNavigation() {
  tns.reset();
  // build RoIs (calo)
  TriggerElement* roi1 = tns.addRoINode(tns.getInitialNode());
  TriggerElement* roi2 = tns.addRoINode(tns.getInitialNode());
  TriggerElement* roi3 = tns.addRoINode(tns.getInitialNode());
  TriggerElement* roi4 = tns.addRoINode(tns.getInitialNode());


  // make a fist TE seeding from this RoIs 
  TriggerElement* calo1 =  tns.addNode(roi1, 1101);
  TriggerElement* calo2 =  tns.addNode(roi2, 1102);
  TriggerElement* calo3 =  tns.addNode(roi3, 1103);
  TriggerElement* calo4 =  tns.addNode(roi4, 1104);

  // make topo connections (1-2, and 1-3, no 2-3)
  makeTopoConnections(calo1, calo2, ID::topoCalo);
  makeTopoConnections(calo1, calo3, ID::topoCalo); 
  makeTopoConnections(calo2, calo4, ID::topoCalo)->setActiveState(false);  // topo run but failed
  
  tns.addNode(calo1, ID::trk);
  tns.addNode(calo2, ID::trk);
  tns.addNode(calo3, ID::trk);
  tns.addNode(calo4, ID::trk);
  

  // build RoIs (mu)
  TriggerElement* roi5 = tns.addRoINode(tns.getInitialNode());
  TriggerElement* roi6 = tns.addRoINode(tns.getInitialNode());
  TriggerElement* roi7 = tns.addRoINode(tns.getInitialNode());

  TriggerElement* ms1 =  tns.addNode(roi5, 3301);
  TriggerElement* ms2 =  tns.addNode(roi6, 3302);
  TriggerElement* ms3 =  tns.addNode(roi7, 3303);

  // make topo connections
  makeTopoConnections(ms1, ms2, ID::topoMu)->setActiveState(false);
  makeTopoConnections(ms1, ms3, ID::topoMu);
  makeTopoConnections(ms2, ms3, ID::topoMu)->setActiveState(false);
  
  tns.addNode(ms1, ID::trk2);
  tns.addNode(ms2, ID::trk2);
  tns.addNode(ms3, ID::trk2);

  // make mu+electron combinations
  makeTopoConnections(ms1, calo1, ID::topoMuCalo);
  makeTopoConnections(ms2, calo1, ID::topoMuCalo);
  makeTopoConnections(ms3, calo2, ID::topoMuCalo)->setActiveState(false); 
}

void printCombination(size_t n,  const TEVec& comb) {
  const TriggerElement* te1 = tns.getDirectPredecessors(comb[0])[0];
  const TriggerElement* te2 = tns.getDirectPredecessors(comb[1])[0];
  MSG("INFO", "Combination " << n <<" of TEs seeded of " << te1->getId() << " " << te2->getId());  
}

size_t iterateAndPrint(ComboIteratorBase& it) {
  size_t counter = 0;
  while( it.isValid() ) {

    TEVec comb;
    comb = it.combination();
    printCombination(counter, comb);
    counter++;
    it++;    
  }
  return counter;
}


bool testPlainIteratorSymmetric() {
  BEGIN_TEST;
  TEVec trks;
  tns.getAllOfType(ID::trk, trks);
  std::vector<TEVec> zipped{trks, trks};
  
  ComboIterator it(zipped,  &tns);
  it.rewind();
  if ( not it.isValid() ) 
    REPORT_AND_STOP("Newly made ComboIterator is invalid");
  PROGRESS;
  size_t counter = iterateAndPrint(it);
  if ( counter != 6 ) 
    REPORT_AND_STOP("To few combinations");

  MSG("INFO", "Plain iterator test passed ok on symmetric test");  
  return true;  
}


bool testPlainIteratorAsymmetric() {
  BEGIN_TEST;
  TEVec trksCalo;
  tns.getAllOfType(ID::trk, trksCalo);
  TEVec trksMu;
  tns.getAllOfType(ID::trk2, trksMu);
  std::vector<TEVec> zipped{trksCalo, trksMu};

  ComboIterator it(zipped,  &tns);
  if ( not it.isValid() ) 
    REPORT_AND_STOP("Newly made ComboIterator is invalid");

  size_t counter = iterateAndPrint(it);
  if ( counter !=  trksCalo.size()*trksMu.size()) 
    REPORT_AND_STOP("Wrong number of combinations " << counter);

  return true;
}




bool testTopoIterator() {
  BEGIN_TEST;
  TEVec trksCalo;
  tns.getAllOfType(ID::trk, trksCalo);

  TEVec trksMu;
  tns.getAllOfType(ID::trk2, trksMu);

  {
    PROGRESS;
    std::vector<TEVec> zipped{trksCalo, trksCalo};    
    ComboIteratorTopo it(zipped, &tns, ID::topoCalo);  
    it.rewind();
    size_t counter = iterateAndPrint(it);
    if ( counter != 2 ) 
      REPORT_AND_STOP("Topo iterator provides wrong number of combinations (2)" << counter );
  }
  {
    PROGRESS;
    std::vector<TEVec> zipped{trksMu, trksMu};
    ComboIteratorTopo it(zipped, &tns, ID::topoMu);  
    it.rewind();
    size_t counter = iterateAndPrint(it);
    if ( counter != 1 ) 
      REPORT_AND_STOP("Topo iterator provides wrong number of combinations (1)" << counter );    
  }
  {
    PROGRESS;
    std::vector<TEVec> zipped{trksMu, trksCalo};
    ComboIteratorTopo it(zipped, &tns, ID::topoMu);   // this is on purpose there should be 0 combs as topoMu never spans caloTEs
    it.rewind();
    size_t counter = iterateAndPrint(it);
    if ( counter != 0 ) 
      REPORT_AND_STOP("Topo iterator provides wrong number of combinations (0)" << counter );    
  }
  {
    PROGRESS;
    std::vector<TEVec> zipped{trksMu, trksCalo};
    ComboIteratorTopo it(zipped, &tns, ID::topoMuCalo);   
    it.rewind();
    size_t counter = iterateAndPrint(it);
    if ( counter != 2 ) 
      REPORT_AND_STOP("Topo iterator provides wrong number of combinations (2)" << counter );    
  }
  return true;    
}


int main() {

  prepareNavigation();
  
  if ( not testPlainIteratorSymmetric() ) 
    ABORT("testPlainIteratorAsymmetric() test failed");

  if ( not testPlainIteratorAsymmetric() ) 
    ABORT("testPlainIteratorAsymmetric() test failed");

  if (not testTopoIterator())
    ABORT("testTopoIterator() test failed");
    
  MSG("OK", "Test passed");
  return 0;
}
