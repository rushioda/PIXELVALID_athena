/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBlock.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaBlock.cxx
 * @author scott snyder
 * @date May 2007
 * @brief These are large blocks of memory that get allocated and
 *        divided up into smaller, uniform elements.
 *        Out-of-line implementation.
 */


#include "AthAllocators/ArenaBlock.h"
#include <cstdlib>


namespace SG {


/// Global number of blocks in use.
size_t ArenaBlock::s_nactive = 0;


/**
 * @brief Create a new block.
 * @param n The number of elements in the new block.
 * @param elt_size The size in bytes of each element.
 * @param ctor If non-null, call this function on each element
 *             in the new block.
 */
ArenaBlock*
ArenaBlock::newBlock (size_t n, size_t elt_size, func_t* ctor)
{
  size_t tot_size = n*elt_size + ArenaBlockBodyOffset;
  ArenaBlock* p = reinterpret_cast<ArenaBlock*> (std::malloc (tot_size));
  ++s_nactive;
  p->m_link = 0;
  p->m_elt_size = elt_size;
  p->m_size = n;
  if (ctor) {
    for (size_t i = 0; i < n; i++)
      ctor (p->index (i, elt_size));
  }
  return p;
}


/**
 * @brief Destroy a block.
 * @param p The block to destroy.
 * @param dtor If non-null, call this function on each element in the block.
 */
void ArenaBlock::destroy (ArenaBlock* p, func_t* dtor)
{
  if (dtor) {
    size_t elt_size = p->eltSize();
    size_t n = p->size();
    for (size_t i = 0; i < n; i++)
      dtor (p->index (i, elt_size));
  }
  --s_nactive;
  std::free (p);
}


/**
 * @brief Destroy all blocks in a list.
 * @param p The first block to destroy.
 * @param dtor If non-null, call this function on each element the blocks.
 *
 * Will destroy all blocks in the linked list headed by @c p.
 */
void
ArenaBlock::destroyList (ArenaBlock* p, func_t* dtor)
{
  while (p) {
    ArenaBlock* next = p->link();
    destroy (p, dtor);
    p = next;
  }
}


/**
 * @brief Concatenate two lists of blocks.
 * @param headp Pointer to pointer to the head of the list.
 * @param tail Pointer to list to append to the end.
 *
 * The list @c tail is appended to the end of the list @c *headp.
 * (@c headp is a pointer-to-pointer to be able to handle the case
 * of an empty list.)
 */
void ArenaBlock::appendList (ArenaBlock** link, ArenaBlock* tail)
{
  while (*link)
    link = &(*link)->link();
  *link = tail;
}


/**
 * @brief Call a function on elements in a list of blocks.
 * @param p Pointer to the head of the list.
 * @param func Function to apply.
 * @param Number of elements in the first block on which
 *        to call the function.
 *
 * This will loop through the elements in all blocks on the list,
 * calling @c func.  In the first block, we apply the function
 * only to the first @c n elements.  In subsequent blocks, the
 * function is applied to all elements.
 */
void ArenaBlock::applyList (ArenaBlock* p,
                            func_t* func,
                            size_t n)
{
  if (!p) return;
  size_t elt_size = p->eltSize();
  if (n > p->size())
    n = p->size();
  while (1) {
    for (size_t i = 0; i < n; i++)
      func (p->index (i, elt_size));
    p = p->link();
    if (!p) break;
    n = p->size();
  }
}


/**
 * @brief Return the per-block memory overhead, in bytes.
 *
 * This tries to include malloc overhead as well, but that may just
 * be an estimate.  Don't rely on this to be exact.
 */
size_t ArenaBlock::overhead()
{
  // The extra size_t is a guesstimate of malloc overhead.
  return ArenaBlockBodyOffset + sizeof (size_t);
}


} // namespace SG
