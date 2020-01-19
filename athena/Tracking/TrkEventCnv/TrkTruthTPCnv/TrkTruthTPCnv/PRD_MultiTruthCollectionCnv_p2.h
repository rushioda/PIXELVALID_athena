/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef PRD_MULTITRUTHCOLLECTIONCNV_P2_H 
#define PRD_MULTITRUTHCOLLECTIONCNV_P2_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PRD_MultiTruthCollection;
namespace Trk { class PRD_MultiTruthCollection_p2; }


class PRD_MultiTruthCollectionCnv_p2 
  : public T_AthenaPoolTPCnvBase<PRD_MultiTruthCollection,
				 Trk::PRD_MultiTruthCollection_p2>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::PRD_MultiTruthCollection
   *  from its persistent representation @c PRD_MultiTruthCollection_p2
   */
  virtual void persToTrans( const Trk::PRD_MultiTruthCollection_p2* persObj, 
                            PRD_MultiTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c PRD_MultiTruthCollection_p2
   *  from its transient representation @c Analysis::PRD_MultiTruthCollection
   */
  virtual void transToPers( const PRD_MultiTruthCollection* transObj, 
                            Trk::PRD_MultiTruthCollection_p2* persObj, 
                            MsgStream& msg );

}; 

#endif/*PRD_MULTITRUTHCOLLECTIONCNV_P2_H*/
