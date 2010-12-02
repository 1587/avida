/*
 *  pagemap.h
 *  Avida
 *
 *  Added by David on 10/14/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Copyright (c) 2005, Google Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: Sanjay Ghemawat <opensource@google.com>
//
// A data structure used by the caching malloc.  It maps from page# to
// a pointer that contains info about that page.  We use two
// representations: one for 32-bit addresses, and another for 64 bit
// addresses.  Both representations provide the same interface.  The
// first representation is implemented as a flat array, the seconds as
// a three-level radix tree that strips away approximately 1/3rd of
// the bits every time.
//
// The BITS parameter should be the number of bits required to hold
// a page number.  E.g., with 32 bit pointers and 4K pages (i.e.,
// page offset fits in lower 12 bits), BITS == 20.

#ifndef TCMALLOC_PAGEMAP_H_
#define TCMALLOC_PAGEMAP_H_

#include "tcmalloc-platform.h"
#if defined HAVE_STDINT_H
#include <stdint.h>
#elif defined HAVE_INTTYPES_H
#include <inttypes.h>
#else
#include <sys/types.h>
#endif
#include "internal_logging.h"

// Single-level array
template <int BITS>
class TCMalloc_PageMap1 {
 private:
  static const int LENGTH = 1 << BITS;

  void** array_;

 public:
  typedef uintptr_t Number;

  explicit TCMalloc_PageMap1(void* (*allocator)(size_t)) {
    array_ = reinterpret_cast<void**>((*allocator)(sizeof(void*) << BITS));
    memset(array_, 0, sizeof(void*) << BITS);
  }

  // Ensure that the map contains initialized entries "x .. x+n-1".
  // Returns true if successful, false if we could not allocate memory.
  bool Ensure(Number x, size_t n) {
    // Nothing to do since flat array was allocated at start.  All
    // that's left is to check for overflow (that is, we don't want to
    // ensure a number y where array_[y] would be an out-of-bounds
    // access).
    return n <= LENGTH - x;   // an overflow-free way to do "x + n <= LENGTH"
  }

  void PreallocateMoreMemory() {}

  // Return the current value for KEY.  Returns NULL if not yet set,
  // or if k is out of range.
  void* get(Number k) const {
    if ((k >> BITS) > 0) {
      return NULL;
    }
    return array_[k];
  }

  // REQUIRES "k" is in range "[0,2^BITS-1]".
  // REQUIRES "k" has been ensured before.
  //
  // Sets the value for KEY.
  void set(Number k, void* v) {
    array_[k] = v;
  }
};

// Two-level radix tree
template <int BITS>
class TCMalloc_PageMap2 {
 private:
  // Put 32 entries in the root and (2^BITS)/32 entries in each leaf.
  static const int ROOT_BITS = 5;
  static const unsigned int ROOT_LENGTH = 1 << ROOT_BITS;

  static const int LEAF_BITS = BITS - ROOT_BITS;
  static const int LEAF_LENGTH = 1 << LEAF_BITS;

  // Leaf node
  struct Leaf {
    void* values[LEAF_LENGTH];
  };

  Leaf* root_[ROOT_LENGTH];             // Pointers to 32 child nodes
  void* (*allocator_)(size_t);          // Memory allocator

 public:
  typedef uintptr_t Number;

  explicit TCMalloc_PageMap2(void* (*allocator)(size_t)) {
    allocator_ = allocator;
    memset(root_, 0, sizeof(root_));
  }

  void* get(Number k) const {
    const Number i1 = k >> LEAF_BITS;
    const Number i2 = k & (LEAF_LENGTH-1);
    if ((k >> BITS) > 0 || root_[i1] == NULL) {
      return NULL;
    }
    return root_[i1]->values[i2];
  }

  void set(Number k, void* v) {
    ASSERT(k >> BITS == 0);
    const Number i1 = k >> LEAF_BITS;
    const Number i2 = k & (LEAF_LENGTH-1);
    root_[i1]->values[i2] = v;
  }

  bool Ensure(Number start, size_t n) {
    for (Number key = start; key <= start + n - 1; ) {
      const Number i1 = key >> LEAF_BITS;

      // Check for overflow
      if (i1 >= ROOT_LENGTH)
        return false;

      // Make 2nd level node if necessary
      if (root_[i1] == NULL) {
        Leaf* leaf = reinterpret_cast<Leaf*>((*allocator_)(sizeof(Leaf)));
        if (leaf == NULL) return false;
        memset(leaf, 0, sizeof(*leaf));
        root_[i1] = leaf;
      }

      // Advance key past whatever is covered by this leaf node
      key = ((key >> LEAF_BITS) + 1) << LEAF_BITS;
    }
    return true;
  }

  void PreallocateMoreMemory() {
    // Allocate enough to keep track of all possible pages
    Ensure(0, 1 << BITS);
  }
};

// Three-level radix tree
template <int BITS>
class TCMalloc_PageMap3 {
 private:
  // How many bits should we consume at each interior level
  static const unsigned int INTERIOR_BITS = (BITS + 2) / 3; // Round-up
  static const unsigned int INTERIOR_LENGTH = 1 << INTERIOR_BITS;

  // How many bits should we consume at leaf level
  static const int LEAF_BITS = BITS - 2*INTERIOR_BITS;
  static const int LEAF_LENGTH = 1 << LEAF_BITS;

  // Interior node
  struct Node {
    Node* ptrs[INTERIOR_LENGTH];
  };

  // Leaf node
  struct Leaf {
    void* values[LEAF_LENGTH];
  };

  Node* root_;                          // Root of radix tree
  void* (*allocator_)(size_t);          // Memory allocator

  Node* NewNode() {
    Node* result = reinterpret_cast<Node*>((*allocator_)(sizeof(Node)));
    if (result != NULL) {
      memset(result, 0, sizeof(*result));
    }
    return result;
  }

 public:
  typedef uintptr_t Number;

  explicit TCMalloc_PageMap3(void* (*allocator)(size_t)) {
    allocator_ = allocator;
    root_ = NewNode();
  }

  void* get(Number k) const {
    const Number i1 = k >> (LEAF_BITS + INTERIOR_BITS);
    const Number i2 = (k >> LEAF_BITS) & (INTERIOR_LENGTH-1);
    const Number i3 = k & (LEAF_LENGTH-1);
    if ((k >> BITS) > 0 ||
        root_->ptrs[i1] == NULL || root_->ptrs[i1]->ptrs[i2] == NULL) {
      return NULL;
    }
    return reinterpret_cast<Leaf*>(root_->ptrs[i1]->ptrs[i2])->values[i3];
  }

  void set(Number k, void* v) {
    ASSERT(k >> BITS == 0);
    const Number i1 = k >> (LEAF_BITS + INTERIOR_BITS);
    const Number i2 = (k >> LEAF_BITS) & (INTERIOR_LENGTH-1);
    const Number i3 = k & (LEAF_LENGTH-1);
    reinterpret_cast<Leaf*>(root_->ptrs[i1]->ptrs[i2])->values[i3] = v;
  }

  bool Ensure(Number start, size_t n) {
    for (Number key = start; key <= start + n - 1; ) {
      const Number i1 = key >> (LEAF_BITS + INTERIOR_BITS);
      const Number i2 = (key >> LEAF_BITS) & (INTERIOR_LENGTH-1);

      // Check for overflow
      if (i1 >= INTERIOR_LENGTH || i2 >= INTERIOR_LENGTH)
        return false;

      // Make 2nd level node if necessary
      if (root_->ptrs[i1] == NULL) {
        Node* n = NewNode();
        if (n == NULL) return false;
        root_->ptrs[i1] = n;
      }

      // Make leaf node if necessary
      if (root_->ptrs[i1]->ptrs[i2] == NULL) {
        Leaf* leaf = reinterpret_cast<Leaf*>((*allocator_)(sizeof(Leaf)));
        if (leaf == NULL) return false;
        memset(leaf, 0, sizeof(*leaf));
        root_->ptrs[i1]->ptrs[i2] = reinterpret_cast<Node*>(leaf);
      }

      // Advance key past whatever is covered by this leaf node
      key = ((key >> LEAF_BITS) + 1) << LEAF_BITS;
    }
    return true;
  }

  void PreallocateMoreMemory() {
  }
};

#endif  // TCMALLOC_PAGEMAP_H_
