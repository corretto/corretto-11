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

#ifndef SHARE_VM_GC_SHENANDOAHUTILS_HPP
#define SHARE_VM_GC_SHENANDOAHUTILS_HPP

#include "gc/shared/gcCause.hpp"
#include "gc/shared/gcTraceTime.inline.hpp"
#include "gc/shared/vmGCOperations.hpp"
#include "gc/shared/isGCActiveMark.hpp"
#include "gc/shared/suspendibleThreadSet.hpp"
#include "gc/shenandoah/shenandoahPhaseTimings.hpp"
#include "gc/shenandoah/shenandoahThreadLocalData.hpp"
#include "jfr/jfrEvents.hpp"
#include "memory/allocation.hpp"
#include "runtime/safepoint.hpp"
#include "runtime/vmThread.hpp"
#include "runtime/vmOperations.hpp"
#include "services/memoryService.hpp"

class GCTimer;

class ShenandoahGCSession : public StackObj {
private:
  ShenandoahHeap* const _heap;
  GCTimer*  const _timer;
  GCTracer* const _tracer;

  TraceMemoryManagerStats _trace_cycle;
public:
  ShenandoahGCSession(GCCause::Cause cause);
  ~ShenandoahGCSession();
};

class ShenandoahPausePhase : public StackObj {
private:
  GCTraceTimeImplWrapper<LogLevel::Info, LOG_TAGS(gc)> _tracer;
  ConcurrentGCTimer* const _timer;

public:
  ShenandoahPausePhase(const char* title, bool log_heap_usage = false);
  ~ShenandoahPausePhase();
};

class ShenandoahConcurrentPhase : public StackObj {
private:
  GCTraceTimeImplWrapper<LogLevel::Info, LOG_TAGS(gc)> _tracer;
  ConcurrentGCTimer* const _timer;

public:
  ShenandoahConcurrentPhase(const char* title, bool log_heap_usage = false);
  ~ShenandoahConcurrentPhase();
};

class ShenandoahGCPhase : public StackObj {
private:
  static ShenandoahPhaseTimings::Phase  _current_phase;

  ShenandoahPhaseTimings* const         _timings;
  const ShenandoahPhaseTimings::Phase   _phase;
  ShenandoahPhaseTimings::Phase         _parent_phase;
  double _start;

public:
  ShenandoahGCPhase(ShenandoahPhaseTimings::Phase phase);
  ~ShenandoahGCPhase();

  static ShenandoahPhaseTimings::Phase current_phase() { return _current_phase; }

  static bool is_current_phase_valid();
};

class ShenandoahGCSubPhase: public ShenandoahGCPhase {
private:
  ConcurrentGCTimer* const _timer;

public:
  ShenandoahGCSubPhase(ShenandoahPhaseTimings::Phase phase);
  ~ShenandoahGCSubPhase();
};

class ShenandoahGCWorkerPhase : public StackObj {
private:
  ShenandoahPhaseTimings* const       _timings;
  const ShenandoahPhaseTimings::Phase _phase;
public:
  ShenandoahGCWorkerPhase(ShenandoahPhaseTimings::Phase phase);
  ~ShenandoahGCWorkerPhase();
};

// Aggregates all the things that should happen before/after the pause.
class ShenandoahGCPauseMark : public StackObj {
private:
  ShenandoahHeap* const _heap;
  const GCIdMark                _gc_id_mark;
  const SvcGCMarker             _svc_gc_mark;
  const IsGCActiveMark          _is_gc_active_mark;
  TraceMemoryManagerStats       _trace_pause;

public:
  ShenandoahGCPauseMark(uint gc_id, SvcGCMarker::reason_type type);
};

class ShenandoahSafepoint : public AllStatic {
public:
  // check if Shenandoah GC safepoint is in progress
  static inline bool is_at_shenandoah_safepoint() {
    if (!SafepointSynchronize::is_at_safepoint()) return false;

    VM_Operation* vm_op = VMThread::vm_operation();
    if (vm_op == NULL) return false;

    VM_Operation::VMOp_Type type = vm_op->type();
    return type == VM_Operation::VMOp_ShenandoahInitMark ||
           type == VM_Operation::VMOp_ShenandoahFinalMarkStartEvac ||
           type == VM_Operation::VMOp_ShenandoahInitUpdateRefs ||
           type == VM_Operation::VMOp_ShenandoahFinalUpdateRefs ||
           type == VM_Operation::VMOp_ShenandoahFullGC ||
           type == VM_Operation::VMOp_ShenandoahDegeneratedGC;
  }
};

class ShenandoahWorkerSession : public StackObj {
protected:
  uint _worker_id;

  ShenandoahWorkerSession(uint worker_id);
  ~ShenandoahWorkerSession();
public:
  static inline uint worker_id() {
    Thread* thr = Thread::current();
    uint id = ShenandoahThreadLocalData::worker_id(thr);
    assert(id != ShenandoahThreadLocalData::INVALID_WORKER_ID, "Worker session has not been created");
    return id;
  }
};

class ShenandoahConcurrentWorkerSession : public ShenandoahWorkerSession {
public:
  ShenandoahConcurrentWorkerSession(uint worker_id) : ShenandoahWorkerSession(worker_id) { }
  ~ShenandoahConcurrentWorkerSession();
};

class ShenandoahParallelWorkerSession : public ShenandoahWorkerSession {
public:
  ShenandoahParallelWorkerSession(uint worker_id) : ShenandoahWorkerSession(worker_id) { }
  ~ShenandoahParallelWorkerSession();
};

class ShenandoahSuspendibleThreadSetJoiner {
private:
  SuspendibleThreadSetJoiner _joiner;
public:
  ShenandoahSuspendibleThreadSetJoiner(bool active = true) : _joiner(active) {
    assert(!ShenandoahThreadLocalData::is_evac_allowed(Thread::current()), "STS should be joined before evac scope");
  }
  ~ShenandoahSuspendibleThreadSetJoiner() {
    assert(!ShenandoahThreadLocalData::is_evac_allowed(Thread::current()), "STS should be left after evac scope");
  }
};

class ShenandoahSuspendibleThreadSetLeaver {
private:
  SuspendibleThreadSetLeaver _leaver;
public:
  ShenandoahSuspendibleThreadSetLeaver(bool active = true) : _leaver(active) {
    assert(!ShenandoahThreadLocalData::is_evac_allowed(Thread::current()), "STS should be left after evac scope");
  }
  ~ShenandoahSuspendibleThreadSetLeaver() {
    assert(!ShenandoahThreadLocalData::is_evac_allowed(Thread::current()), "STS should be joined before evac scope");
  }
};

class ShenandoahUtils {
public:
  static size_t round_up_power_of_2(size_t value);
};

#endif // SHARE_VM_GC_SHENANDOAHUTILS_HPP
