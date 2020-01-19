/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETComponentMap_v1.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODCaloEvent/CaloCluster.h"

#include "xAODTracking/TrackParticle.h"

#include "xAODBase/IParticleContainer.h"
#include "xAODBase/ObjectType.h"

#include <iterator>
#include <cstdio>

using namespace xAOD;

///////////////////////////////
// Internally used constants //
///////////////////////////////

size_t MissingETComponentMap_v1::m_clusterLinkReserve = 10000; 
size_t MissingETComponentMap_v1::m_trackLinkReserve   = 10000;
size_t MissingETComponentMap_v1::m_maxClusterSize     = 5000;
size_t MissingETComponentMap_v1::m_maxTrackSize       = 5000;
size_t MissingETComponentMap_v1::m_maxSignalSize      = fmax(m_maxTrackSize,m_maxClusterSize);

/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MissingETComponentMap_v1::MissingETComponentMap_v1(SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETComponent_v1>(own,trackIndices)
  , m_lastMETObject((const MissingET*)0)
  , m_lastContribIndex(MissingETBase::Numerical::invalidIndex())
{
  this->f_setupLookupCache(m_clusterLinks,m_clusterLinkReserve); 
  this->f_setupLookupCache(m_trackLinks,m_trackLinkReserve); 
}

MissingETComponentMap_v1::MissingETComponentMap_v1(MissingETComponentMap_v1::iterator first, MissingETComponentMap_v1::iterator last,
						   SG::OwnershipPolicy own, SG::IndexTrackingPolicy trackIndices)
  : DataVector<MissingETComponent_v1>(first,last,own,trackIndices)
  , m_lastMETObject((const MissingET*)0)
  , m_lastContribIndex(MissingETBase::Numerical::invalidIndex())
{
  this->f_setupLookupCache(m_clusterLinks,m_clusterLinkReserve); 
  this->f_setupLookupCache(m_trackLinks,m_trackLinkReserve); 
}

MissingETComponentMap_v1::~MissingETComponentMap_v1()
{ this->clearCache(); }

/////////////
// Finders //
/////////////

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(const MissingET* pMET) const
{ 
  if ( pMET == m_lastMETObject ) 
    { const_iterator fCont(this->begin()); std::advance<const_iterator>(fCont,m_lastContribIndex);  return fCont; }
  else { return this->f_findConst(pMET); }
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(const MissingET* pMET)
{ 
  if ( pMET == m_lastMETObject ) { iterator fCont(this->begin()); std::advance<iterator>(fCont,m_lastContribIndex); return fCont; }
  else { return this->f_find(pMET); }
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(const std::string& name) const
{ 
  if ( m_lastMETObject == 0 || m_lastMETObject->name() != name ) { return this->f_findConst(name); }
  else { const_iterator fCont(this->begin()); std::advance<const_iterator>(fCont,m_lastContribIndex); return fCont; }
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(const std::string& name)
{ 
  if ( m_lastMETObject == 0 || m_lastMETObject->name() != name ) { return this->f_find(name); } 
  else { iterator fCont(this->begin()); std::advance<iterator>(fCont,m_lastContribIndex); return fCont; }
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::find(MissingETBase::Types::bitmask_t src) const
{ 
  if ( m_lastMETObject == 0 || m_lastMETObject->source() != src ) { return this->f_findConst(src); }
  else { const_iterator fCont(this->begin()); std::advance<const_iterator>(fCont,m_lastContribIndex); return fCont; }
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::find(MissingETBase::Types::bitmask_t src)
{ 
  if ( m_lastMETObject == 0 || m_lastMETObject->source() != src ) { return this->f_find(src); }
  else { iterator fCont(this->begin()); std::advance<iterator>(fCont,m_lastContribIndex); return fCont; }
}

size_t MissingETComponentMap_v1::findIndex(const MissingET* pMET) const
{ if ( m_lastMETObject != pMET ) { this->f_findConst(pMET); } return m_lastContribIndex; }

size_t MissingETComponentMap_v1::findIndex(const std::string& name) const
{ if ( m_lastMETObject == 0 || this->at(m_lastContribIndex)->metObject()->name() != name ) { this->f_findConst(name); } return m_lastContribIndex; }

size_t MissingETComponentMap_v1::findIndex(MissingETBase::Types::bitmask_t sw) const
{ if ( m_lastMETObject == 0 || this->at(m_lastContribIndex)->statusWord() != sw ) { this->f_findConst(sw); } return m_lastContribIndex; }

///////////////////////
// Protected finders //
///////////////////////

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::f_findConst(const MissingET* pMET) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject() != pMET ) { ++fCont; }
  this->f_setConstCache(fCont);
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::f_find(const MissingET* pMET)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject() != pMET ) { ++fCont; }
  this->f_setCache(fCont);
  return fCont;
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::f_findConst(const std::string& name) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while (fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  this->f_setConstCache(fCont);
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::f_find(const std::string& name)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->name() != name ) { ++fCont; }
  this->f_setCache(fCont);
  return fCont;
}

MissingETComponentMap_v1::const_iterator MissingETComponentMap_v1::f_findConst(MissingETBase::Types::bitmask_t src) const
{
  const_iterator fCont(this->begin());
  const_iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  this->f_setConstCache(fCont);
  return fCont;
}

MissingETComponentMap_v1::iterator MissingETComponentMap_v1::f_find(MissingETBase::Types::bitmask_t src)
{
  iterator fCont(this->begin());
  iterator lCont(this->end());
  while ( fCont != lCont && (*fCont)->metObject()->source() != src ) { ++fCont; }
  this->f_setCache(fCont);
  return fCont;
}

void MissingETComponentMap_v1::f_setConstCache(const_iterator fCont) const
{
  if ( fCont != this->end() )
    { 
      m_lastMETObject    = (*fCont)->metObject();
      m_lastContribIndex = (size_t)std::distance<const_iterator>(this->begin(),fCont);
    } 
  else { m_lastMETObject = (const MissingET*)0; m_lastContribIndex = MissingETBase::Numerical::invalidIndex(); }
}

void MissingETComponentMap_v1::f_setCache(iterator fCont)
{
  if ( fCont != this->end() )
    { 
      m_lastMETObject    = const_cast<const MissingET*>((*fCont)->metObject());
      m_lastContribIndex =  std::distance<iterator>(this->begin(),fCont);
    }
  else  { m_lastMETObject = (const MissingET*)0; m_lastContribIndex = MissingETBase::Numerical::invalidIndex(); }
}

/////////////////////////
// Retrieve MET object //
/////////////////////////

const MissingET* MissingETComponentMap_v1::retrieveMissingET(const std::string& name) const
{ if ( m_lastMETObject == 0 || m_lastMETObject->name() != name ) { this->f_findConst(name); } return m_lastMETObject; }

const MissingET* MissingETComponentMap_v1::retrieveMissingET(MissingETBase::Types::bitmask_t src,bool excl) const
{ return excl ? this->f_retrieveMissingETExcl(src) :  this->f_retrieveMissingETIncl(src); }

const MissingET* MissingETComponentMap_v1::retrieveMissingET(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw,bool excl) const
{ return excl ? this->f_retrieveMissingETExcl(src,sw) : this->f_retrieveMissingETIncl(src,sw); }

///////////////////////////////////
// Internal MET object retrieval //
///////////////////////////////////

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src)  const
{ if ( m_lastMETObject == 0 || m_lastMETObject->source() != src ) { this->f_findConst(src); } return m_lastMETObject; }

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const
{ 
  if ( m_lastMETObject == 0 || ( m_lastMETObject->source() != src || this->at(m_lastContribIndex)->statusWord() != sw ) )
    {
      const_iterator fCont(this->begin()); const_iterator lCont(this->end());
      while ( fCont != lCont && ( (*fCont)->metObject()->source() != src || (*fCont)->statusWord() != sw ) ) { ++fCont; }
      this->f_setConstCache(fCont);
    }
  return m_lastMETObject; 
}

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src) const
{ 
  if ( m_lastMETObject == 0 || !MissingETBase::Source::hasPattern(m_lastMETObject->source(),src) ) 
    {
      const_iterator fCont(this->begin()); const_iterator lCont(this->end());
      while ( fCont != lCont && !MissingETBase::Source::hasPattern((*fCont)->metObject()->source(),src) ) { ++fCont; }
      this->f_setConstCache(fCont);
    }
  return m_lastMETObject;
}

const MissingET* MissingETComponentMap_v1::f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const
{
  if ( m_lastMETObject == 0 || (!MissingETBase::Source::hasPattern(m_lastMETObject->source(),src) || 
					 !MissingETBase::Status::Tags::hasPattern(this->at(m_lastContribIndex)->statusWord(),sw) ) )
    {
      const_iterator fCont(this->begin()); const_iterator lCont(this->end());
      while ( fCont!= lCont && ( !MissingETBase::Source::hasPattern((*fCont)->metObject()->source(),src) ||
				 !MissingETBase::Status::Tags::hasPattern((*fCont)->statusWord(),sw) ) )
	{ ++fCont; }
      this->f_setConstCache(fCont);
    }
  return m_lastMETObject; 
}

/////////////////////////////
// Object usage management //
/////////////////////////////

bool MissingETComponentMap_v1::checkUsage(const  IParticle* pPart,MissingETBase::UsageHandler::Policy p) const
{ MissingETBase::Types::object_vector_t objects; objects.push_back(pPart); return this->checkUsage(objects,p); }

bool MissingETComponentMap_v1::checkUsage(MissingETBase::Types::object_vector_t& signals,MissingETBase::UsageHandler::Policy p) const 
{
  bool retVal(false);
  switch ( p )
    {
    case MissingETBase::UsageHandler::OnlyCluster:
      { if ( f_extractSignal<xAOD::Type::CaloCluster>(signals) ) { retVal = this->f_checkObjectUsage(signals,m_clusterLinks); }	break; }
    case MissingETBase::UsageHandler::OnlyTrack:
      { if ( f_extractSignal<xAOD::Type::TrackParticle>(signals) ) { retVal = this->f_checkObjectUsage(signals,m_trackLinks); } break; }
    case MissingETBase::UsageHandler::TrackCluster:
      {
	MissingETBase::Types::object_vector_t ins(signals.begin(),signals.end());
	size_t nSig(0);
	if ( f_extractSignal<xAOD::Type::CaloCluster>(signals) )
	  { nSig = signals.size(); this->f_checkObjectUsage(signals,m_clusterLinks); }
	if ( f_extractSignal<xAOD::Type::TrackParticle>(ins) )
	  { nSig += ins.size();	this->f_checkObjectUsage(ins,m_trackLinks); }
	signals.insert(signals.end(),ins.begin(),ins.end());
	retVal = signals.size() == nSig;
	break;
      }
    case MissingETBase::UsageHandler::PhysicsObject:
      { retVal = this->f_checkObjectUsage(signals,m_particleLinks); break; }
    default:
      { break; }
    } // switch 
  return retVal;
}

bool MissingETComponentMap_v1::f_checkObjectUsage(MissingETBase::Types::object_vector_t& signals,signal_vector_t& signalLinks) const
{
  if ( signals.empty() ) { return false; }
  // check if new event
  if ( signalLinks.empty() )  // this also an indication that there is no overlap!
    {
      size_t nSig(m_maxSignalSize);
      const IParticleContainer* pCont = static_cast<const IParticleContainer*>(signals.front()->container());
      // if ( pCont == 0 ) 
      // 	{ printf("MissingETComponentMap::f_checkObjectUsage(...) - WARNING - cannot cast container pointer %p to IParticleContainer\n",(void*)signals.front()->container()); }
      // else
      nSig = pCont->size();
      indexedlink_t sw(MissingETBase::Numerical::invalidLink().get<0>(),MissingETBase::Numerical::invalidLink().get<1>());
      signalLinks.resize(nSig,sw);
      return false;
    }
  // loop on signals
  size_t oldSize(signals.size());
  MissingETBase::Types::object_vector_t::iterator fSig(signals.begin());
  while ( fSig != signals.end() ) 
    {
      size_t fIdx((*fSig)->index()); 
      if( fIdx >= signalLinks.size() ) {
	// if invalid, something probably went wrong in reconstruction, so complain
	// this should raise an out-of-range exception
	printf("MissingETComponentMap::f_checkObjectUsage(...) - ERROR - searching for a signal with index (%lu) beyond signal container range (%lu).", fIdx, signalLinks.size());
	printf("MissingETComponentMap::f_checkObjectUsage(...) - ERROR - this may indicate an inconsistency in the signal containers.");	
      }
      // signal already used
      if ( signalLinks.at(fIdx) != MissingETBase::Numerical::invalidLink() ) { fSig = signals.erase(fSig); }
      // signal not yet used
      else { ++fSig; }
    } // loop on signals
  return oldSize != signals.size(); 
}

bool MissingETComponentMap_v1::f_checkObjectUsage(MissingETBase::Types::object_vector_t& objects,particle_map_t& physicsLinks) const
{
  if ( objects.empty() ) { return false; }
  //
  size_t oldSize(objects.size());
  MissingETBase::Types::object_vector_t::iterator fObj(objects.begin());
  while ( fObj != objects.end() )
    { if ( physicsLinks.find(*fObj) != physicsLinks.end() ) { fObj = objects.erase(fObj); } else { ++fObj; } }
  return objects.size() != oldSize;
}

/////////////////
// Add objects //
/////////////////

bool MissingETComponentMap_v1::addObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,MissingETBase::UsageHandler::Policy p)
{
  bool flag(false);
  switch ( p )
    {
    case MissingETBase::UsageHandler::OnlyCluster:
      { flag = this->setClusters(pMET,pPart,signalList); break; }
    case MissingETBase::UsageHandler::OnlyTrack:
      { flag = this->setTracks(pMET,pPart,signalList); break; }
    case MissingETBase::UsageHandler::TrackCluster:
      { bool cFlg(this->setClusters(pMET,pPart,signalList)); bool tFlg(this->setTracks(pMET,pPart,signalList)); flag = cFlg || tFlg; break; } 
    case MissingETBase::UsageHandler::PhysicsObject:
      { flag = this->setObjects(pMET,pPart,signalList); break; }
    default:
      { break; }
    }
  return flag;
}

bool MissingETComponentMap_v1::setClusters(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList)
{
  // find all clusters
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // check input: contributing particle pointer
  size_t pIdx(this->at(cIdx)->findIndex(pPart)); if ( pIdx == MissingETBase::Numerical::invalidIndex()  ) { return false; }
  indexedlink_t ld(cIdx,pIdx); 
  // make sure cache is not cleared
  bool firstCluster = true;
  MissingETBase::Types::object_vector_t::const_iterator fSig(signalList.begin());
  MissingETBase::Types::object_vector_t::const_iterator lSig(signalList.end());
  for ( ; fSig != lSig; ++fSig ) {
    if((*fSig)->type()==xAOD::Type::CaloCluster) {
      if(firstCluster) {
	firstCluster = false;
	if ( m_clusterLinks.empty() )  { 
	  size_t nClus(m_maxClusterSize);
	  const IParticleContainer* pCont(static_cast<const IParticleContainer*>((*fSig)->container()));
	  // if ( pCont == 0 )
	  //   { printf("MissingETComponentMap::setClusters(...) - WARNING   - cannot access cluster container (invalid NULL pointer), use %i words\n",(int)nClus); }
	  // else
	  nClus = pCont->size();
	  m_clusterLinks.resize(nClus,MissingETBase::Numerical::invalidLink());
	} // empty cluster link list
      } //first cluster
      // all clusters refer to the same MET contribution
      m_clusterLinks[(*fSig)->index()] = ld;
    } // extract CaloClusters
  } // loop over signal list

  return !firstCluster; // true if we processed any clusters
}

bool MissingETComponentMap_v1::setTracks(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList)
{
  // find all clusters
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // check input: contributing particle pointer
  size_t pIdx(this->at(cIdx)->findIndex(pPart)); if ( pIdx == MissingETBase::Numerical::invalidIndex()  ) { return false; }
  indexedlink_t ld(cIdx,pIdx); 
  // make sure cache is not cleared
  bool firstTrack = true;
  size_t nTrack(m_maxTrackSize);
  MissingETBase::Types::object_vector_t::const_iterator fSig(signalList.begin());
  MissingETBase::Types::object_vector_t::const_iterator lSig(signalList.end());
  for ( ; fSig != lSig; ++fSig ) {
    if((*fSig)->type()==xAOD::Type::TrackParticle) {
      if(firstTrack) {
	firstTrack = false;
	if ( m_trackLinks.empty() ) { 
	  const IParticleContainer* pCont(static_cast<const IParticleContainer*>((*fSig)->container()));
	  // if ( pCont == 0 )
	  //   { printf("MissingETComponentMap::setTracks(...) - WARNING   - cannot access track container (invalid NULL pointer), use %i words\n",(int)nTrack); }
	  // else
	  nTrack = pCont->size();
	  m_trackLinks.resize(nTrack,MissingETBase::Numerical::invalidLink());
	} // empty track link list
      } // first track
      // all tracks refer to the same MET contribution
      m_trackLinks[(*fSig)->index()] = ld;
    } // extract TrackParticles
  } // loop over signal list

  return !firstTrack; // true if we processed any clusters
}

bool MissingETComponentMap_v1::setObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& objects)
{
  // check input: MissingET object pointer
  size_t cIdx(this->findIndex(pMET)); if ( cIdx == MissingETBase::Numerical::invalidIndex() ) { return false; }
  // loop input objects
  size_t oldSize(m_particleLinks.size());
  MissingETBase::Types::object_vector_t::const_iterator fObj(objects.begin());
  MissingETBase::Types::object_vector_t::const_iterator lObj(objects.end());
  for ( ; fObj != lObj; ++fObj ) { size_t pIdx(this->at(cIdx)->findIndex(pPart)); m_particleLinks[*fObj] = indexedlink_t(cIdx,pIdx); }
  //
  return oldSize < m_particleLinks.size();
} 

////////////////////
// Cache handlers //
////////////////////
 
void MissingETComponentMap_v1::f_setupLookupCache(signal_vector_t& signalLinks,size_t reserve)
{ signalLinks.clear(); if ( reserve > 0 ) { signalLinks.reserve(reserve); } }

void MissingETComponentMap_v1::f_clearLookupCache()
{ m_clusterLinks.clear(); m_trackLinks.clear(); m_particleLinks.clear(); }

void MissingETComponentMap_v1::f_fillLookupCache()
{
  ////
}

size_t MissingETComponentMap_v1::getComponentIndex(const IParticle* pPart) const { return f_getIndex<0>(pPart); }
size_t MissingETComponentMap_v1::getObjectIndex(const IParticle* pPart)    const { return f_getIndex<1>(pPart); }
// {
//   // check on cluster signal
//   const CaloCluster* pClus(dynamic_cast<const CaloCluster*>(pPart));
//   if ( pClus != 0 ) { return  pClus->index() < m_clusterLinks.size() ? m_clusterLinks.at(pClus->index()).get<1>() : MissingETBase::Numerical::invalidIndex(); }
//   // check on track index
//   const TrackParticle* pTrk(dynamic_cast<const TrackParticle*>(pPart));
//   if ( pTrk != 0 ) { return pTrk->index() < m_trackLinks.size() ? m_trackLinks.at(pTrk->index()).get<1>() : MissingETBase::Numerical::invalidIndex(); }
//   // check on physics objects
//   particle_map_t::const_iterator fPart(m_particleLinks.find(pPart));
//   return fPart != m_particleLinks.end() ? (fPart->second).get<1>() : MissingETBase::Numerical::invalidIndex();
// } 

//bool MissingETComponent_v1::addLink(const IParticle* pPart,MissingETComponent* pContrib,MissingETBase::Types::bitmask_t status)
//{
  // check if cluster signal object
  //  CaloCluster* pClus = dynamic_cast<const CaloCluster*>(pPart);
  //  if ( pClus != 0 ) return this->addCluster(pClus,pContrib,status);
  // check if track signal object
  //  TrackParticle* pTrk = dynamic_cast<const TrackParticle*>(pPart);
  //  if ( pTrk != 0 ) return this->addTrack(pTrk,pContrib,status);
  // build link key and data
//  particle_key_t key(pPart,status); 
//  contrib_idx_t idx = pContrib->index();
  // add to map
//  particle_map_t::iterator fLnk(m_partMETLookup.find(key));
//  if ( fLnk == m_partMETLookup.end() ) { m_partMETLookup.insert(particle_map_t::value_type(key,idx)); }
//  else { fLnk->second = idx; }
//  return true; 
//} 

//bool MissingETComponent_v1::addCluster(const CaloCluster* pClus,MissingETComponent* pContrib,MissingETBase::Types::bitmask_t /* status */)
//{
//  if ( m_clusterIndices.empty() ) { m_clusterIndices.resize(pClus->container()->size(),size_t(-1)); }
  //
//  if ( !this->checkCluster(pClus) ) { m_clusterIndices[pClus->index()] = pContrib->index(); return true; }
//  else { return false; }
//}

//bool MissingETComponent_v1::addTrack(const TrackParticle* pTrack,MissingETComponent* pContrib,MissingETBase::Types::bitmask_ty /* status */ )
//{
// if ( m_trackIndices.empty() ) { m_trackIndices.resize(pTrack->container()->size(),size_t(-1)); }
  //
//  if ( !this->checkTrack(pTrack) ) { m_trackIndices[pTrack->index()] = pContrib->index(); return true; }
//  else { return false; }
//}

//bool MissingETComponent_v1::checkCluster(const CaloCluster* pClus) const
//{ return pClus->index() < m_clusterIndices.size() && m_clusterIndices.at(pClus->index()) != size_t(-1); }

//bool MissingETComponenty_v1::checkTrack(const TrackParticle* pTrk) const
//{ return pTrk->index() < m_trackIndices.size() && m_trackIndices.at(pTrk->index()) != size_t(-1); }
