/*
 * Copyright (c) 2017, 2019, Red Hat, Inc. All rights reserved.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_GC_SHENANDOAH_SHENANDOAHCODEROOTS_HPP
#define SHARE_VM_GC_SHENANDOAH_SHENANDOAHCODEROOTS_HPP

#include "code/codeCache.hpp"
#include "gc/shenandoah/shenandoahSharedVariables.hpp"
#include "gc/shenandoah/shenandoahLock.hpp"
#include "gc/shenandoah/shenandoahPadding.hpp"
#include "memory/allocation.hpp"
#include "memory/iterator.hpp"

class ShenandoahHeap;
class ShenandoahHeapRegion;

class ShenandoahParallelCodeHeapIterator {
  friend class CodeCache;
private:
  CodeHeap*     _heap;
  shenandoah_padding(0);
  volatile int  _claimed_idx;
  volatile bool _finished;
  shenandoah_padding(1);
public:
  ShenandoahParallelCodeHeapIterator(CodeHeap* heap);
  void parallel_blobs_do(CodeBlobClosure* f);
};

class ShenandoahParallelCodeCacheIterator {
  friend class CodeCache;
private:
  ShenandoahParallelCodeHeapIterator* _iters;
  int                       _length;

private:
  // Noncopyable.
  ShenandoahParallelCodeCacheIterator(const ShenandoahParallelCodeCacheIterator& o);
  ShenandoahParallelCodeCacheIterator& operator=(const ShenandoahParallelCodeCacheIterator& o);
public:
  ShenandoahParallelCodeCacheIterator(const GrowableArray<CodeHeap*>* heaps);
  ~ShenandoahParallelCodeCacheIterator();
  void parallel_blobs_do(CodeBlobClosure* f);
};

// ShenandoahNMethod tuple records the internal locations of oop slots within the nmethod.
// This allows us to quickly scan the oops without doing the nmethod-internal scans, that
// sometimes involves parsing the machine code. Note it does not record the oops themselves,
// because it would then require handling these tuples as the new class of roots.
class ShenandoahNMethod : public CHeapObj<mtGC> {
private:
  nmethod* _nm;
  oop**    _oops;
  int      _oops_count;

public:
  ShenandoahNMethod(nmethod *nm, GrowableArray<oop*>* oops);
  ~ShenandoahNMethod();

  nmethod* nm() {
    return _nm;
  }

  bool has_cset_oops(ShenandoahHeap* heap);

  void assert_alive_and_correct() NOT_DEBUG_RETURN;
  void assert_same_oops(GrowableArray<oop*>* oops) NOT_DEBUG_RETURN;

  static bool find_with_nmethod(void* nm, ShenandoahNMethod* other) {
    return other->_nm == nm;
  }
};

class ShenandoahCodeRootsIterator {
  friend class ShenandoahCodeRoots;
protected:
  ShenandoahHeap* _heap;
  ShenandoahParallelCodeCacheIterator _par_iterator;
  ShenandoahSharedFlag _seq_claimed;
  DEFINE_PAD_MINUS_SIZE(0, DEFAULT_CACHE_LINE_SIZE, sizeof(volatile size_t));
  volatile size_t _claimed;
  DEFINE_PAD_MINUS_SIZE(1, DEFAULT_CACHE_LINE_SIZE, 0);
protected:
  ShenandoahCodeRootsIterator();
  ~ShenandoahCodeRootsIterator();

  template<bool CSET_FILTER>
  void dispatch_parallel_blobs_do(CodeBlobClosure *f);

  template<bool CSET_FILTER>
  void fast_parallel_blobs_do(CodeBlobClosure *f);
};

class ShenandoahAllCodeRootsIterator : public ShenandoahCodeRootsIterator {
public:
  ShenandoahAllCodeRootsIterator() : ShenandoahCodeRootsIterator() {};
  void possibly_parallel_blobs_do(CodeBlobClosure *f);
};

class ShenandoahCsetCodeRootsIterator : public ShenandoahCodeRootsIterator {
public:
  ShenandoahCsetCodeRootsIterator() : ShenandoahCodeRootsIterator() {};
  void possibly_parallel_blobs_do(CodeBlobClosure* f);
};

class ShenandoahCodeRoots : public AllStatic {
  friend class ShenandoahHeap;
  friend class ShenandoahCodeRootsIterator;

public:
  static void initialize();
  static void add_nmethod(nmethod* nm);
  static void remove_nmethod(nmethod* nm);

private:
  static GrowableArray<ShenandoahNMethod*>* _recorded_nms;
  static ShenandoahLock                     _recorded_nms_lock;
};

#endif //SHARE_VM_GC_SHENANDOAH_SHENANDOAHCODEROOTS_HPP
