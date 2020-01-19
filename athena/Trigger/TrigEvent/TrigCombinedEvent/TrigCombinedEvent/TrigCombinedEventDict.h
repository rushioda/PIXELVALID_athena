/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_COMBINEDEVENT_DICT
#define TRIG_COMBINEDEVENT_DICT

#include "TrigCombinedEvent/TrigComposite.h"
#include "TrigCombinedEvent/TrigCompositeContainer.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TrigFeatureLink.h"


//the function triggers loading dictionaries for typedefs
void dummyTriggerForTrigCombinedEventTypedefs(TrigCompositeContainer a,
					      DataVector<TrigCompositeContainer> b
					      ){
  
  //to avoid warnings about unused variables
  TrigCompositeContainer   aa=a; 
  DataVector<TrigCompositeContainer>   bb = b;
	
  std::vector<const TrigComposite*> aaa; aaa.size();

}

#endif // TRIG_COMBINEDEVENT_DICT
