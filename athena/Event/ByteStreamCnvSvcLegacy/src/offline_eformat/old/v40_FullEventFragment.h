//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file eformat/FullEventFragment.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: salnikov $
 * $Revision: 726499 $
 * $Date: 2016-02-26 17:52:24 +0100 (Fri, 26 Feb 2016) $
 *
 * @brief Defines the Event Fragment entity. The definition is based on the
 * update of ATL-DAQ-98-129, by D.Francis et al.
 */

#ifndef OFFLINE_EFORMAT_V40_FULLEVENTFRAGMENT_H
#define OFFLINE_EFORMAT_V40_FULLEVENTFRAGMENT_H

#include <vector>

#include "v40_Header.h"
#include "v40_ROBFragment.h"
#include "eformat/Problem.h"
#include "eformat/blob.h"

namespace offline_eformat { namespace v40 {

  /**
   * Describes how to access the contents of an event fragment, as prescribed
   * by the event format note.
   */
  class FullEventFragment : public Header {

  public: //interface

   /**
     * To build a fragment given the containing buffer. I need to know
     * where the fragment starts in order to do that.
     *
     * @param it The exact position where this fragment should start.
     */
    FullEventFragment (const uint32_t* it);

    /**
     * Builds an empty, otherwise useless FullEventFragment
     */
    FullEventFragment ();

    /**
     * Copy constructor
     *
     * @param other The fragment to be copied
     */
    FullEventFragment (const FullEventFragment& other);

    /**
     * Constructor that only works with T=<real pointer>. This constructor is
     * pretty much like the one with the pointer, but it will fetch the pointer
     * from the u32 blob. It is only used in the python bindings.
     *
     * @param blob The blob which contains the block of data this fragment is
     * in.
     */
    FullEventFragment (const eformat::helper::u32list& blob);

    /**
     * Constructor that only works with T=<real pointer>. This constructor is
     * pretty much like the one with the pointer, but it will fetch the pointer
     * from the u32 blob. It is only used in the python bindings.
     *
     * @param blob The blob which contains the block of data this fragment is
     * in.
     */
    FullEventFragment (const eformat::helper::u32slice& blob);

    /**
     * Destructor virtualisation
     */
    virtual ~FullEventFragment();

    /**
     * Copy operator
     *
     * @param other The fragment to be copied
     */
    FullEventFragment& operator= (const FullEventFragment& other);

    /**
     * Manual re-assignment
     *
     * @param it The position pointing the first word of this fragment
     */
    FullEventFragment& assign (const uint32_t* it);

    /**
     * Says if the the fragment is valid. You can control the
     * exception behaviour.
     */
    bool check_impl
    (const uint16_t version=eformat::MAJOR_V40_VERSION,
     bool exc=true) const;

    /**
     * Says if the the fragment is valid. This method throw exceptions.
     */
    bool check (const uint16_t version=eformat::MAJOR_V40_VERSION) const;

    /**
     * Says if the the fragment is valid. This does NOT throw exceptions.
     */
    bool check_noex (const uint16_t version=eformat::MAJOR_V40_VERSION) const;

    /**
     * Says if the Fragment and all children are valid. You can control the
     * exception behaviour.
     */
    bool check_tree_impl
    (const uint16_t version=eformat::MAJOR_V40_VERSION,
     const uint16_t rod_version=eformat::MAJOR_V31_VERSION,
     const bool exc=true) const;

    /**
     * Says if the Fragment and all children are valid. This throws exceptions.
     */
    bool check_tree (const uint16_t version=eformat::MAJOR_V40_VERSION,
        const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Says if the Fragment, all children and all RODs are valid, without
     * throwing exceptions.
     */
    bool check_tree_noex (const uint16_t version=eformat::MAJOR_V40_VERSION,
        const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Tells which problems the current fragment has.
     */
    void problems (std::vector<eformat::FragmentProblem>& p,
        const uint16_t version=eformat::MAJOR_V40_VERSION) const;

    /**
     * Tells which problems the current fragment and all the children have
     */
    void problems_tree (std::vector<eformat::helper::ProblemContainer>& p,
			const uint16_t version=eformat::MAJOR_V40_VERSION,
			const uint16_t rod_version=eformat::MAJOR_V31_VERSION)
      const;

    /**
     * Retrieves a vector of all ROBs from this fragment 
     */
    void robs (std::vector<ROBFragment>& robs) const;

    /**
     * Retrieves vectors of good and bad ROBs from this fragment, separated
     */
    void robs (std::vector<ROBFragment>& good, std::vector<ROBFragment>& bad,
        const uint16_t version=eformat::MAJOR_V40_VERSION,
        const uint16_t rod_version=eformat::MAJOR_V31_VERSION) const;

    /**
     * Returns amount of seconds since the (unix) origin, for the bunch
     * crossing time.
     */
    uint32_t bc_time_seconds() const;

    /**
     * Returns amount of nanoseconds aditionally to the amount of seconds
     * returned by bc_time_seconds, for the bunch crossing time.
     */
    uint32_t bc_time_nanoseconds() const;

    /**
     * Returns the global identifier
     */
    uint32_t global_id() const;

    /**
     * Returns the run type
     */
    uint32_t run_type() const;

    /**
     * Returns the run number
     */
    uint32_t run_no() const;

    /**
     * Returns the luminosity block number
     */
    uint16_t lumi_block() const;

    /**
     * Returns the lvl1 identifier
     */
    uint32_t lvl1_id() const;

    /**
     * Returns the bunch crossing identifier
     */
    uint16_t bc_id() const;

    /**
     * Returns the lvl1 trigger type
     */
    uint8_t lvl1_trigger_type() const;

    /**
     * Returns the number of lvl1 trigger info words
     */
    uint32_t nlvl1_trigger_info() const;

    /**
     * Returns an iterator to the first of the LVL1 trigger info words
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* lvl1_trigger_info() const;
    inline void lvl1_trigger_info (const uint32_t*& it) const //legacy
    { it = lvl1_trigger_info(); }

    /**
     * Returns the number of lvl2 trigger info words
     */
    uint32_t nlvl2_trigger_info() const;

    /**
     * Returns an iterator to the first of the LVL2 trigger info words
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* lvl2_trigger_info() const;
    inline void lvl2_trigger_info (const uint32_t*& it) const //legacy
    { it = lvl2_trigger_info(); }

    /**
     * Returns the number of Event Filter words
     */
    uint32_t nevent_filter_info () const;

    /**
     * Returns an iterator to the first of the EF info words
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* event_filter_info() const;
    inline void event_filter_info (const uint32_t*& it) const //legacy
    { it = event_filter_info(); }

    /**
     * Returns the number of stream tag words
     */
    uint32_t nstream_tag () const;

    /**
     * Returns an iterator to the first of the stream tag words
     *
     * @param it An <em>updateable</em> iterator you should provide.
     */
    const uint32_t* stream_tag() const;
    inline void stream_tag (const uint32_t*& it) const //legacy
    { it = stream_tag(); }

    /**
     * Returns the number of children available.
     */
    virtual uint32_t nchildren () const;

    /**
     * Returns the nth child fragment. If the nth fragment doesn't exist, the
     * behaviour is undefined.
     *
     * @param p A preallocated pointer you should provide.
     * @param n The fragment position, starting at zero, of the child fragment
     * you would like to get.
     */
    const uint32_t* child(size_t n) const;
    virtual inline void child (const uint32_t*& p, size_t n) const //legacy
    { p = child(n); }

    /**
     * Returns the nth child fragment. If the nth fragment doesn't exist, an
     * exception is thrown. 
     *
     * @warning Use this method with care, it is slower than the equivalent
     * method that doesn't check.
     *
     * @param p A preallocated pointer you should provide.
     * @param n The fragment position, starting at zero, of the child fragment
     * you would like to get.
     */
    const uint32_t* child_check(size_t n) const;
    virtual inline void child_check (const uint32_t*& p, size_t n) const
    { p = child_check(n); } //legacy

    /**
     * Returns all the children of this fragment. The input to this method is a
     * valid set of iterators to existing, pre-allocated pointers
     *
     * @param p A pointer to a set of preallocated const uint32_t*'s to set to the
     * position of the children of this fragment.
     * @param max The maximum number of children, p can point to.
     *
     * @return The number of children found on this fragment
     */
    virtual uint32_t children (const uint32_t** p, size_t max) const;

  private: //representation

    const uint32_t* m_start; ///< my start word

  };

}}

#endif /* OFFLINE_EFORMAT_V40_FULLEVENTFRAGMENT_H */
