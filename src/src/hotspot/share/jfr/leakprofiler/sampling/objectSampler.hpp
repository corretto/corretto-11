/*
 * Copyright (c) 2014, 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
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

#ifndef SHARE_VM_LEAKPROFILER_SAMPLING_OBJECTSAMPLER_HPP
#define SHARE_VM_LEAKPROFILER_SAMPLING_OBJECTSAMPLER_HPP

#include "memory/allocation.hpp"
#include "jfr/utilities/jfrTime.hpp"

typedef u8 traceid;

class BoolObjectClosure;
class JavaThread;
class OopClosure;
class ObjectSample;
class SampleList;
class SamplePriorityQueue;

// Class reponsible for holding samples and
// making sure the samples are evenly distributed as
// new entries are added and removed.
class ObjectSampler : public CHeapObj<mtTracing> {
  friend class LeakProfiler;
  friend class StartOperation;
  friend class StopOperation;
 private:
  SamplePriorityQueue* _priority_queue;
  SampleList* _list;
  JfrTicks _last_sweep;
  size_t _total_allocated;
  size_t _threshold;
  size_t _size;
  bool _dead_samples;

  // Lifecycle
  explicit ObjectSampler(size_t size);
  ~ObjectSampler();
  static bool create(size_t size);
  static bool is_created();
  static void destroy();

  // Sampling
  static void sample(HeapWord* object, size_t size, JavaThread* thread);
  void add(HeapWord* object, size_t size, traceid thread_id, JavaThread* thread);
  void scavenge();
  void remove_dead(ObjectSample* sample);

  // Called by GC
  static void oops_do(BoolObjectClosure* is_alive, OopClosure* f);

  const ObjectSample* item_at(int index) const;
  ObjectSample* item_at(int index);
  int item_count() const;

 public:
  static ObjectSampler* sampler();
  // For operations that require exclusive access (non-safepoint)
  static ObjectSampler* acquire();
  static void release();

  const ObjectSample* first() const;
  ObjectSample* last() const;
  const ObjectSample* last_resolved() const;
  void set_last_resolved(const ObjectSample* sample);
  const JfrTicks& last_sweep() const;
};

#endif // SHARE_VM_LEAKPROFILER_SAMPLING_OBJECTSAMPLER_HPP
