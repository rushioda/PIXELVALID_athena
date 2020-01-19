/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloClusterLinkTemplate.h"

#include "CaloEvent/CaloClusterNavigable.h"
#include "CaloEvent/CaloCellLink.h"
#include "CaloEvent/CaloCellLinkContainer.h"

#include <algorithm>
//#include <iostream>

CaloClusterNavigable::CaloClusterNavigable()
  : m_ownLinkStore(false)
{ }

CaloClusterNavigable::CaloClusterNavigable(CaloCellLinkContainer* pLinkColl,
					   CaloCellLink* pLink)
  : m_ownLinkStore(false)
{
  CaloClusterLinkTemplate<CaloCellLinkContainer>::setLink(pLinkColl,
							  pLink,
							  m_cellLink);
}

CaloClusterNavigable::CaloClusterNavigable(CaloCellLink* pLink)
  : m_ownLinkStore(true)
{
  m_cellLink.setElement(pLink);
}

CaloClusterNavigable::~CaloClusterNavigable()
{ 
  if ( m_ownLinkStore && *m_cellLink != 0 ) delete *m_cellLink;
}

CaloCellLink* CaloClusterNavigable::getCellLink()
{
  if ( m_ownLinkStore )
    {
      if ( *m_cellLink == 0 ) m_cellLink.setElement(new CaloCellLink());
      return const_cast<CaloCellLink*>(*m_cellLink);
    }
  else
    {
      return (CaloCellLink*)0;
    }

  // check if local pointer to store
  //  if ( m_cellLinkPointer != 0 ) { return m_cellLinkPointer;}
  // check if linked pointer to store
  //  if ( m_cellLink.isValid() && *m_cellLink != 0 ) 
  //    { 
      // FIXME
  //     CaloCellLink* pLink = const_cast<CaloCellLink*>(*m_cellLink);
  //      return pLink; 
  //    }
  // create a new local store/
  //  m_cellLinkPointer = new CaloCellLink();
  //  return m_cellLinkPointer;
}

const CaloCellLink* 
CaloClusterNavigable::getCellLink() const
{
  // return *m_cellLink;

  //  if ( m_cellLinkPointer != 0 )
  //  {
      //      std::cout << "[CaloClusterNavigable@" << this << "] return "
      //		<< "cell link @" << m_cellLinkPointer << std::endl;
      //      return m_cellLinkPointer;
      //    } 
  //  if ( *m_cellLink == 0 )
  //    {
  //      std::cout << "[CaloClusterNavigable@" << this << "] problem with "
  //		<< "cell link store, unexpected pointer = "
  //		<< *m_cellLink << std::endl;
  //    }
  return m_cellLink.isValid() ? *m_cellLink : 0;
}



bool CaloClusterNavigable::replaceCellContainer(const CaloCellContainer* newCont) const {
  //return (*m_cellLink)->replace(newCont);
  CaloCellLink* lnk=const_cast<CaloCellLink*>(*m_cellLink);
  return lnk->replace(newCont);
}
