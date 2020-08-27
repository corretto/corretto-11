/*
 * Copyright (c) 2019, Red Hat, Inc. All rights reserved.
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


#include "precompiled.hpp"


#include "classfile/classLoaderData.hpp"
#include "classfile/systemDictionary.hpp"
#include "code/codeCache.hpp"
#include "gc/shenandoah/shenandoahAsserts.hpp"
#include "gc/shenandoah/shenandoahHeap.hpp"
#include "gc/shenandoah/shenandoahPhaseTimings.hpp"
#include "gc/shenandoah/shenandoahRootVerifier.hpp"
#include "gc/shenandoah/shenandoahStringDedup.hpp"
#include "gc/shenandoah/shenandoahUtils.hpp"
#include "gc/shared/weakProcessor.hpp"
#include "memory/universe.hpp"
#include "runtime/thread.hpp"
#include "services/management.hpp"
#include "utilities/debug.hpp"

// Check for overflow of number of root types.
STATIC_ASSERT((static_cast<uint>(ShenandoahRootVerifier::AllRoots) + 1) > static_cast<uint>(ShenandoahRootVerifier::AllRoots));

ShenandoahRootVerifier::ShenandoahRootVerifier() : _types(AllRoots) {
}

void ShenandoahRootVerifier::excludes(RootTypes types) {
  _types = static_cast<ShenandoahRootVerifier::RootTypes>(static_cast<uint>(_types) & (~static_cast<uint>(types)));
}

bool ShenandoahRootVerifier::verify(RootTypes type) const {
  return (_types & type) != 0;
}

void ShenandoahRootVerifier::oops_do(OopClosure* oops) {
  CodeBlobToOopClosure blobs(oops, !CodeBlobToOopClosure::FixRelocations);
  if (verify(CodeRoots)) {
    shenandoah_assert_locked_or_safepoint(CodeCache_lock);
    CodeCache::blobs_do(&blobs);
  }

  if (verify(CLDGRoots)) {
    shenandoah_assert_safepoint();
    CLDToOopClosure clds(oops, false);
    ClassLoaderDataGraph::cld_do(&clds);
  }

  if (verify(SerialRoots)) {
    shenandoah_assert_safepoint();
    Universe::oops_do(oops);
    Management::oops_do(oops);
    JvmtiExport::oops_do(oops);
    ObjectSynchronizer::oops_do(oops);
    SystemDictionary::oops_do(oops);
  }

  if (verify(JNIHandleRoots)) {
    shenandoah_assert_safepoint();
    JNIHandles::oops_do(oops);
  }

  if (verify(WeakRoots)) {
    shenandoah_assert_safepoint();
    AlwaysTrueClosure always_true;
    WeakProcessor::weak_oops_do(&always_true, oops);
  }

  if (ShenandoahStringDedup::is_enabled() && verify(StringDedupRoots)) {
    shenandoah_assert_safepoint();
    ShenandoahStringDedup::oops_do_slow(oops);
  }

  if (verify(ThreadRoots)) {
    shenandoah_assert_safepoint();
    // Do thread roots the last. This allows verification code to find
    // any broken objects from those special roots first, not the accidental
    // dangling reference from the thread root.
    Threads::possibly_parallel_oops_do(false, oops, &blobs);
  }
}

void ShenandoahRootVerifier::roots_do(OopClosure* oops) {
  shenandoah_assert_safepoint();

  CodeBlobToOopClosure blobs(oops, !CodeBlobToOopClosure::FixRelocations);
  CodeCache::blobs_do(&blobs);

  CLDToOopClosure clds(oops, false);
  ClassLoaderDataGraph::cld_do(&clds);

  Universe::oops_do(oops);
  Management::oops_do(oops);
  JvmtiExport::oops_do(oops);
  JNIHandles::oops_do(oops);
  ObjectSynchronizer::oops_do(oops);
  SystemDictionary::oops_do(oops);

  AlwaysTrueClosure always_true;
  WeakProcessor::weak_oops_do(&always_true, oops);

  if (ShenandoahStringDedup::is_enabled()) {
    ShenandoahStringDedup::oops_do_slow(oops);
  }

  // Do thread roots the last. This allows verification code to find
  // any broken objects from those special roots first, not the accidental
  // dangling reference from the thread root.
  Threads::possibly_parallel_oops_do(false, oops, &blobs);
}

void ShenandoahRootVerifier::strong_roots_do(OopClosure* oops) {
  shenandoah_assert_safepoint();

  CodeBlobToOopClosure blobs(oops, !CodeBlobToOopClosure::FixRelocations);

  CLDToOopClosure clds(oops, false);
  ClassLoaderDataGraph::roots_cld_do(&clds, NULL);

  Universe::oops_do(oops);
  Management::oops_do(oops);
  JvmtiExport::oops_do(oops);
  JNIHandles::oops_do(oops);
  ObjectSynchronizer::oops_do(oops);
  SystemDictionary::oops_do(oops);

  // Do thread roots the last. This allows verification code to find
  // any broken objects from those special roots first, not the accidental
  // dangling reference from the thread root.
  Threads::possibly_parallel_oops_do(false, oops, &blobs);
}
