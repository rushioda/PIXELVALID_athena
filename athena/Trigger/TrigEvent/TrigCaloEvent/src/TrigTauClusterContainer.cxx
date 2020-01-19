/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*******************************************************

NAME:           TrigTauClusterContainer.cxx
PACKAGE:        Trigger/TrigEvent/TrigCaloEvent
AUTHORS:        Denis O. Damazio, Carlos Osuna, Xin Wu

PURPOSE:        Keep a container for the important
		output variables from the Calorimeter
		LVL2 tau Trigger.
                Optionally seed ID Trigger and EF.
                Variables here should be RAW variables
                not corrected.
DATE:           October 17th, 2005

******************************************************/

#include "TrigCaloEvent/TrigTauClusterContainer.h"

using namespace std;

// Simple debug method
void
TrigTauClusterContainer::print ( void ) const {
	std::cout << "TrigTauClusterContainer with ";
	std::cout << size() << " TrigTauClusters" << std::endl;
	for ( TrigTauClusterContainer::const_iterator i = begin(); i!=end();i++)
		(*i)->print();
}

// Simple debug method using MsgStream
void
TrigTauClusterContainer::print ( MsgStream& log ) const {
	log <<MSG::DEBUG<< "TrigTauClusterContainer with ";
	log <<MSG::DEBUG<< size() << " TrigTauClusters" << endreq;
	for ( TrigTauClusterContainer::const_iterator i = begin(); i!=end();i++)
		(*i)->print(log);
}

// End of file TrigTauClusterContainer.cxx
