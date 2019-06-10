/*
 * Copyright (c) 1999, 2018, Oracle and/or its affiliates. All rights reserved.
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

// no precompiled headers
#include "jvm.h"
#include "asm/macroAssembler.hpp"
#include "classfile/classLoader.hpp"
#include "classfile/systemDictionary.hpp"
#include "classfile/vmSymbols.hpp"
#include "code/codeCache.hpp"
#include "code/icBuffer.hpp"
#include "code/vtableStubs.hpp"
#include "interpreter/interpreter.hpp"
#include "memory/allocation.inline.hpp"
#include "nativeInst_sparc.hpp"
#include "os_share_linux.hpp"
#include "prims/jniFastGetField.hpp"
#include "prims/jvm_misc.hpp"
#include "runtime/arguments.hpp"
#include "runtime/extendedPC.hpp"
#include "runtime/frame.inline.hpp"
#include "runtime/interfaceSupport.inline.hpp"
#include "runtime/java.hpp"
#include "runtime/javaCalls.hpp"
#include "runtime/mutexLocker.hpp"
#include "runtime/osThread.hpp"
#include "runtime/sharedRuntime.hpp"
#include "runtime/stubRoutines.hpp"
#include "runtime/thread.inline.hpp"
#include "runtime/timer.hpp"
#include "utilities/debug.hpp"
#include "utilities/events.hpp"
#include "utilities/vmError.hpp"

// Linux/Sparc has rather obscure naming of registers in sigcontext
// different between 32 and 64 bits
#define SIG_PC(x) ((x)->sigc_regs.tpc)
#define SIG_NPC(x) ((x)->sigc_regs.tnpc)
#define SIG_REGS(x) ((x)->sigc_regs)

// those are to reference registers in sigcontext
enum {
  CON_G0 = 0,
  CON_G1,
  CON_G2,
  CON_G3,
  CON_G4,
  CON_G5,
  CON_G6,
  CON_G7,
  CON_O0,
  CON_O1,
  CON_O2,
  CON_O3,
  CON_O4,
  CON_O5,
  CON_O6,
  CON_O7,
};

// For Forte Analyzer AsyncGetCallTrace profiling support - thread is
// currently interrupted by SIGPROF.
// os::Solaris::fetch_frame_from_ucontext() tries to skip nested
// signal frames. Currently we don't do that on Linux, so it's the
// same as os::fetch_frame_from_context().
ExtendedPC os::Linux::fetch_frame_from_ucontext(Thread* thread,
                                                const ucontext_t* uc,
                                                intptr_t** ret_sp,
                                                intptr_t** ret_fp) {
  assert(thread != NULL, "just checking");
  assert(ret_sp != NULL, "just checking");
  assert(ret_fp != NULL, "just checking");

  return os::fetch_frame_from_context(uc, ret_sp, ret_fp);
}

ExtendedPC os::fetch_frame_from_context(const void* ucVoid,
                                        intptr_t** ret_sp,
                                        intptr_t** ret_fp) {
  const ucontext_t* uc = (const ucontext_t*) ucVoid;
  ExtendedPC  epc;

  if (uc != NULL) {
    epc = ExtendedPC(os::Linux::ucontext_get_pc(uc));
    if (ret_sp) {
      *ret_sp = os::Linux::ucontext_get_sp(uc);
    }
    if (ret_fp) {
      *ret_fp = (intptr_t*)NULL;
    }
  } else {
    // construct empty ExtendedPC for return value checking
    epc = ExtendedPC(NULL);
    if (ret_sp) {
      *ret_sp = (intptr_t*) NULL;
    }
    if (ret_fp) {
      *ret_fp = (intptr_t*) NULL;
    }
  }

  return epc;
}

frame os::fetch_frame_from_context(const void* ucVoid) {
  intptr_t* sp;
  ExtendedPC epc = fetch_frame_from_context(ucVoid, &sp, NULL);
  return frame(sp, frame::unpatchable, epc.pc());
}

frame os::get_sender_for_C_frame(frame* fr) {
  return frame(fr->sender_sp(), frame::unpatchable, fr->sender_pc());
}

frame os::current_frame() {
  intptr_t* sp = StubRoutines::Sparc::flush_callers_register_windows_func()();
  frame myframe(sp, frame::unpatchable,
                CAST_FROM_FN_PTR(address, os::current_frame));
  if (os::is_first_C_frame(&myframe)) {
    // stack is not walkable
    return frame(NULL, frame::unpatchable, NULL);
  } else {
    return os::get_sender_for_C_frame(&myframe);
  }
}

address os::current_stack_pointer() {
  register void *sp __asm__ ("sp");
  return (address)sp;
}

char* os::non_memory_address_word() {
  // Must never look like an address returned by reserve_memory,
  // even in its subfields (as defined by the CPU immediate fields,
  // if the CPU splits constants across multiple instructions).
  // On SPARC, 0 != %hi(any real address), because there is no
  // allocation in the first 1Kb of the virtual address space.
  return (char*) 0;
}

void os::print_context(outputStream *st, const void *context) {
  if (context == NULL) return;

  const ucontext_t* uc = (const ucontext_t*)context;
  sigcontext* sc = (sigcontext*)context;
  st->print_cr("Registers:");

  st->print_cr(" G1=" INTPTR_FORMAT " G2=" INTPTR_FORMAT
               " G3=" INTPTR_FORMAT " G4=" INTPTR_FORMAT,
               SIG_REGS(sc).u_regs[CON_G1],
               SIG_REGS(sc).u_regs[CON_G2],
               SIG_REGS(sc).u_regs[CON_G3],
               SIG_REGS(sc).u_regs[CON_G4]);
  st->print_cr(" G5=" INTPTR_FORMAT " G6=" INTPTR_FORMAT
               " G7=" INTPTR_FORMAT " Y=0x%x",
               SIG_REGS(sc).u_regs[CON_G5],
               SIG_REGS(sc).u_regs[CON_G6],
               SIG_REGS(sc).u_regs[CON_G7],
               SIG_REGS(sc).y);
  st->print_cr(" O0=" INTPTR_FORMAT " O1=" INTPTR_FORMAT
               " O2=" INTPTR_FORMAT " O3=" INTPTR_FORMAT,
               SIG_REGS(sc).u_regs[CON_O0],
               SIG_REGS(sc).u_regs[CON_O1],
               SIG_REGS(sc).u_regs[CON_O2],
               SIG_REGS(sc).u_regs[CON_O3]);
  st->print_cr(" O4=" INTPTR_FORMAT " O5=" INTPTR_FORMAT
               " O6=" INTPTR_FORMAT " O7=" INTPTR_FORMAT,
               SIG_REGS(sc).u_regs[CON_O4],
               SIG_REGS(sc).u_regs[CON_O5],
               SIG_REGS(sc).u_regs[CON_O6],
               SIG_REGS(sc).u_regs[CON_O7]);


  intptr_t *sp = (intptr_t *)os::Linux::ucontext_get_sp(uc);
  st->print_cr(" L0=" INTPTR_FORMAT " L1=" INTPTR_FORMAT
               " L2=" INTPTR_FORMAT " L3=" INTPTR_FORMAT,
               sp[L0->sp_offset_in_saved_window()],
               sp[L1->sp_offset_in_saved_window()],
               sp[L2->sp_offset_in_saved_window()],
               sp[L3->sp_offset_in_saved_window()]);
  st->print_cr(" L4=" INTPTR_FORMAT " L5=" INTPTR_FORMAT
               " L6=" INTPTR_FORMAT " L7=" INTPTR_FORMAT,
               sp[L4->sp_offset_in_saved_window()],
               sp[L5->sp_offset_in_saved_window()],
               sp[L6->sp_offset_in_saved_window()],
               sp[L7->sp_offset_in_saved_window()]);
  st->print_cr(" I0=" INTPTR_FORMAT " I1=" INTPTR_FORMAT
               " I2=" INTPTR_FORMAT " I3=" INTPTR_FORMAT,
               sp[I0->sp_offset_in_saved_window()],
               sp[I1->sp_offset_in_saved_window()],
               sp[I2->sp_offset_in_saved_window()],
               sp[I3->sp_offset_in_saved_window()]);
  st->print_cr(" I4=" INTPTR_FORMAT " I5=" INTPTR_FORMAT
               " I6=" INTPTR_FORMAT " I7=" INTPTR_FORMAT,
               sp[I4->sp_offset_in_saved_window()],
               sp[I5->sp_offset_in_saved_window()],
               sp[I6->sp_offset_in_saved_window()],
               sp[I7->sp_offset_in_saved_window()]);

  st->print_cr(" PC=" INTPTR_FORMAT " nPC=" INTPTR_FORMAT,
               SIG_PC(sc),
               SIG_NPC(sc));
  st->cr();
  st->cr();

  st->print_cr("Top of Stack: (sp=" INTPTR_FORMAT ")", p2i(sp));
  print_hex_dump(st, (address)sp, (address)(sp + 32), sizeof(intptr_t));
  st->cr();

  // Note: it may be unsafe to inspect memory near pc. For example, pc may
  // point to garbage if entry point in an nmethod is corrupted. Leave
  // this at the end, and hope for the best.
  address pc = os::Linux::ucontext_get_pc(uc);
  print_instructions(st, pc, sizeof(char));
  st->cr();
}


void os::print_register_info(outputStream *st, const void *context) {
  if (context == NULL) return;

  const ucontext_t *uc = (const ucontext_t*)context;
  const sigcontext* sc = (const sigcontext*)context;
  intptr_t *sp = (intptr_t *)os::Linux::ucontext_get_sp(uc);

  st->print_cr("Register to memory mapping:");
  st->cr();

  // this is only for the "general purpose" registers
  st->print("G1="); print_location(st, SIG_REGS(sc).u_regs[CON_G1]);
  st->print("G2="); print_location(st, SIG_REGS(sc).u_regs[CON_G2]);
  st->print("G3="); print_location(st, SIG_REGS(sc).u_regs[CON_G3]);
  st->print("G4="); print_location(st, SIG_REGS(sc).u_regs[CON_G4]);
  st->print("G5="); print_location(st, SIG_REGS(sc).u_regs[CON_G5]);
  st->print("G6="); print_location(st, SIG_REGS(sc).u_regs[CON_G6]);
  st->print("G7="); print_location(st, SIG_REGS(sc).u_regs[CON_G7]);
  st->cr();

  st->print("O0="); print_location(st, SIG_REGS(sc).u_regs[CON_O0]);
  st->print("O1="); print_location(st, SIG_REGS(sc).u_regs[CON_O1]);
  st->print("O2="); print_location(st, SIG_REGS(sc).u_regs[CON_O2]);
  st->print("O3="); print_location(st, SIG_REGS(sc).u_regs[CON_O3]);
  st->print("O4="); print_location(st, SIG_REGS(sc).u_regs[CON_O4]);
  st->print("O5="); print_location(st, SIG_REGS(sc).u_regs[CON_O5]);
  st->print("O6="); print_location(st, SIG_REGS(sc).u_regs[CON_O6]);
  st->print("O7="); print_location(st, SIG_REGS(sc).u_regs[CON_O7]);
  st->cr();

  st->print("L0="); print_location(st, sp[L0->sp_offset_in_saved_window()]);
  st->print("L1="); print_location(st, sp[L1->sp_offset_in_saved_window()]);
  st->print("L2="); print_location(st, sp[L2->sp_offset_in_saved_window()]);
  st->print("L3="); print_location(st, sp[L3->sp_offset_in_saved_window()]);
  st->print("L4="); print_location(st, sp[L4->sp_offset_in_saved_window()]);
  st->print("L5="); print_location(st, sp[L5->sp_offset_in_saved_window()]);
  st->print("L6="); print_location(st, sp[L6->sp_offset_in_saved_window()]);
  st->print("L7="); print_location(st, sp[L7->sp_offset_in_saved_window()]);
  st->cr();

  st->print("I0="); print_location(st, sp[I0->sp_offset_in_saved_window()]);
  st->print("I1="); print_location(st, sp[I1->sp_offset_in_saved_window()]);
  st->print("I2="); print_location(st, sp[I2->sp_offset_in_saved_window()]);
  st->print("I3="); print_location(st, sp[I3->sp_offset_in_saved_window()]);
  st->print("I4="); print_location(st, sp[I4->sp_offset_in_saved_window()]);
  st->print("I5="); print_location(st, sp[I5->sp_offset_in_saved_window()]);
  st->print("I6="); print_location(st, sp[I6->sp_offset_in_saved_window()]);
  st->print("I7="); print_location(st, sp[I7->sp_offset_in_saved_window()]);
  st->cr();
}


address os::Linux::ucontext_get_pc(const ucontext_t* uc) {
  return (address) SIG_PC((sigcontext*)uc);
}

void os::Linux::ucontext_set_pc(ucontext_t* uc, address pc) {
  sigcontext* ctx = (sigcontext*) uc;
  SIG_PC(ctx)  = (intptr_t)pc;
  SIG_NPC(ctx) = (intptr_t)(pc+4);
}

intptr_t* os::Linux::ucontext_get_sp(const ucontext_t *uc) {
  return (intptr_t*)
    ((intptr_t)SIG_REGS((sigcontext*)uc).u_regs[CON_O6] + STACK_BIAS);
}

// not used on Sparc
intptr_t* os::Linux::ucontext_get_fp(const ucontext_t *uc) {
  ShouldNotReachHere();
  return NULL;
}

// Utility functions

inline static bool checkPrefetch(sigcontext* uc, address pc) {
  if (StubRoutines::is_safefetch_fault(pc)) {
    os::Linux::ucontext_set_pc((ucontext_t*)uc, StubRoutines::continuation_for_safefetch_fault(pc));
    return true;
  }
  return false;
}

inline static bool checkOverflow(sigcontext* uc,
                                 address pc,
                                 address addr,
                                 JavaThread* thread,
                                 address* stub) {
  // check if fault address is within thread stack
  if (thread->on_local_stack(addr)) {
    // stack overflow
    if (thread->in_stack_yellow_reserved_zone(addr)) {
      thread->disable_stack_yellow_reserved_zone();
      if (thread->thread_state() == _thread_in_Java) {
        // Throw a stack overflow exception.  Guard pages will be reenabled
        // while unwinding the stack.
        *stub =
          SharedRuntime::continuation_for_implicit_exception(thread,
                                                             pc,
                                                             SharedRuntime::STACK_OVERFLOW);
      } else {
        // Thread was in the vm or native code.  Return and try to finish.
        return true;
      }
    } else if (thread->in_stack_red_zone(addr)) {
      // Fatal red zone violation.  Disable the guard pages and fall through
      // to handle_unexpected_exception way down below.
      thread->disable_stack_red_zone();
      tty->print_raw_cr("An irrecoverable stack overflow has occurred.");

      // This is a likely cause, but hard to verify. Let's just print
      // it as a hint.
      tty->print_raw_cr("Please check if any of your loaded .so files has "
                        "enabled executable stack (see man page execstack(8))");
    } else {
      // Accessing stack address below sp may cause SEGV if current
      // thread has MAP_GROWSDOWN stack. This should only happen when
      // current thread was created by user code with MAP_GROWSDOWN flag
      // and then attached to VM. See notes in os_linux.cpp.
      if (thread->osthread()->expanding_stack() == 0) {
        thread->osthread()->set_expanding_stack();
        if (os::Linux::manually_expand_stack(thread, addr)) {
          thread->osthread()->clear_expanding_stack();
          return true;
        }
        thread->osthread()->clear_expanding_stack();
      } else {
        fatal("recursive segv. expanding stack.");
      }
    }
  }
  return false;
}

inline static bool checkPollingPage(address pc, address fault, address* stub) {
  if (os::is_poll_address(fault)) {
    *stub = SharedRuntime::get_poll_stub(pc);
    return true;
  }
  return false;
}

inline static bool checkByteBuffer(address pc, address npc, JavaThread * thread, address* stub) {
  // BugId 4454115: A read from a MappedByteBuffer can fault
  // here if the underlying file has been truncated.
  // Do not crash the VM in such a case.
  CodeBlob* cb = CodeCache::find_blob_unsafe(pc);
  CompiledMethod* nm = cb->as_compiled_method_or_null();
  if (nm != NULL && nm->has_unsafe_access()) {
    *stub = SharedRuntime::handle_unsafe_access(thread, npc);
    return true;
  }
  return false;
}

inline static bool checkVerifyOops(address pc, address fault, address* stub) {
  if (pc >= MacroAssembler::_verify_oop_implicit_branch[0]
      && pc <  MacroAssembler::_verify_oop_implicit_branch[1] ) {
    *stub     =  MacroAssembler::_verify_oop_implicit_branch[2];
    warning("fixed up memory fault in +VerifyOops at address "
            INTPTR_FORMAT, p2i(fault));
    return true;
  }
  return false;
}

inline static bool checkFPFault(address pc, int code,
                                JavaThread* thread, address* stub) {
  if (code == FPE_INTDIV || code == FPE_FLTDIV) {
    *stub =
      SharedRuntime::
      continuation_for_implicit_exception(thread,
                                          pc,
                                          SharedRuntime::IMPLICIT_DIVIDE_BY_ZERO);
    return true;
  }
  return false;
}

inline static bool checkNullPointer(address pc, intptr_t fault,
                                    JavaThread* thread, address* stub) {
  if (!MacroAssembler::needs_explicit_null_check(fault)) {
    // Determination of interpreter/vtable stub/compiled code null
    // exception
    *stub =
      SharedRuntime::
      continuation_for_implicit_exception(thread, pc,
                                          SharedRuntime::IMPLICIT_NULL);
    return true;
  }
  return false;
}

inline static bool checkFastJNIAccess(address pc, address* stub) {
  address addr = JNI_FastGetField::find_slowcase_pc(pc);
  if (addr != (address)-1) {
    *stub = addr;
    return true;
  }
  return false;
}

inline static bool checkSerializePage(JavaThread* thread, address addr) {
  return os::is_memory_serialize_page(thread, addr);
}

inline static bool checkZombie(sigcontext* uc, address* pc, address* stub) {
  if (nativeInstruction_at(*pc)->is_zombie()) {
    // zombie method (ld [%g0],%o7 instruction)
    *stub = SharedRuntime::get_handle_wrong_method_stub();

    // At the stub it needs to look like a call from the caller of this
    // method (not a call from the segv site).
    *pc = (address)SIG_REGS(uc).u_regs[CON_O7];
    return true;
  }
  return false;
}

inline static bool checkICMiss(sigcontext* uc, address* pc, address* stub) {
#ifdef COMPILER2
  if (nativeInstruction_at(*pc)->is_ic_miss_trap()) {
#ifdef ASSERT
#ifdef TIERED
    CodeBlob* cb = CodeCache::find_blob_unsafe(*pc);
    assert(cb->is_compiled_by_c2(), "Wrong compiler");
#endif // TIERED
#endif // ASSERT
    // Inline cache missed and user trap "Tne G0+ST_RESERVED_FOR_USER_0+2" taken.
    *stub = SharedRuntime::get_ic_miss_stub();
    // At the stub it needs to look like a call from the caller of this
    // method (not a call from the segv site).
    *pc = (address)SIG_REGS(uc).u_regs[CON_O7];
    return true;
  }
#endif  // COMPILER2
  return false;
}

extern "C" JNIEXPORT int
JVM_handle_linux_signal(int sig,
                        siginfo_t* info,
                        void* ucVoid,
                        int abort_if_unrecognized) {
  // in fact this isn't ucontext_t* at all, but struct sigcontext*
  // but Linux porting layer uses ucontext_t, so to minimize code change
  // we cast as needed
  ucontext_t* ucFake = (ucontext_t*) ucVoid;
  sigcontext* uc = (sigcontext*)ucVoid;

  Thread* t = Thread::current_or_null_safe();

  // Must do this before SignalHandlerMark, if crash protection installed we will longjmp away
  // (no destructors can be run)
  os::ThreadCrashProtection::check_crash_protection(sig, t);

  SignalHandlerMark shm(t);

  // Note: it's not uncommon that JNI code uses signal/sigset to install
  // then restore certain signal handler (e.g. to temporarily block SIGPIPE,
  // or have a SIGILL handler when detecting CPU type). When that happens,
  // JVM_handle_linux_signal() might be invoked with junk info/ucVoid. To
  // avoid unnecessary crash when libjsig is not preloaded, try handle signals
  // that do not require siginfo/ucontext first.

  if (sig == SIGPIPE || sig == SIGXFSZ) {
    // allow chained handler to go first
    if (os::Linux::chained_handler(sig, info, ucVoid)) {
      return true;
    } else {
      // Ignoring SIGPIPE/SIGXFSZ - see bugs 4229104 or 6499219
      return true;
    }
  }

#ifdef CAN_SHOW_REGISTERS_ON_ASSERT
  if ((sig == SIGSEGV || sig == SIGBUS) && info != NULL && info->si_addr == g_assert_poison) {
    handle_assert_poison_fault(ucVoid, info->si_addr);
    return 1;
  }
#endif

  JavaThread* thread = NULL;
  VMThread* vmthread = NULL;
  if (os::Linux::signal_handlers_are_installed) {
    if (t != NULL ){
      if(t->is_Java_thread()) {
        thread = (JavaThread*)t;
      }
      else if(t->is_VM_thread()){
        vmthread = (VMThread *)t;
      }
    }
  }

  // decide if this trap can be handled by a stub
  address stub = NULL;
  address pc = NULL;
  address npc = NULL;

  //%note os_trap_1
  if (info != NULL && uc != NULL && thread != NULL) {
    pc = address(SIG_PC(uc));
    npc = address(SIG_NPC(uc));

    // Check to see if we caught the safepoint code in the
    // process of write protecting the memory serialization page.
    // It write enables the page immediately after protecting it
    // so we can just return to retry the write.
    if ((sig == SIGSEGV) && checkSerializePage(thread, (address)info->si_addr)) {
      // Block current thread until the memory serialize page permission restored.
      os::block_on_serialize_page_trap();
      return 1;
    }

    if (checkPrefetch(uc, pc)) {
      return 1;
    }

    // Handle ALL stack overflow variations here
    if (sig == SIGSEGV) {
      if (checkOverflow(uc, pc, (address)info->si_addr, thread, &stub)) {
        return 1;
      }
    }

    if (sig == SIGBUS &&
        thread->thread_state() == _thread_in_vm &&
        thread->doing_unsafe_access()) {
      stub = SharedRuntime::handle_unsafe_access(thread, npc);
    }

    if (thread->thread_state() == _thread_in_Java) {
      do {
        // Java thread running in Java code => find exception handler if any
        // a fault inside compiled code, the interpreter, or a stub

        if ((sig == SIGSEGV) && checkPollingPage(pc, (address)info->si_addr, &stub)) {
          break;
        }

        if ((sig == SIGBUS) && checkByteBuffer(pc, npc, thread, &stub)) {
          break;
        }

        if ((sig == SIGSEGV || sig == SIGBUS) &&
            checkVerifyOops(pc, (address)info->si_addr, &stub)) {
          break;
        }

        if ((sig == SIGSEGV) && checkZombie(uc, &pc, &stub)) {
          break;
        }

        if ((sig == SIGILL) && checkICMiss(uc, &pc, &stub)) {
          break;
        }

        if ((sig == SIGFPE) && checkFPFault(pc, info->si_code, thread, &stub)) {
          break;
        }

        if ((sig == SIGSEGV) &&
            checkNullPointer(pc, (intptr_t)info->si_addr, thread, &stub)) {
          break;
        }
      } while (0);

      // jni_fast_Get<Primitive>Field can trap at certain pc's if a GC kicks in
      // and the heap gets shrunk before the field access.
      if ((sig == SIGSEGV) || (sig == SIGBUS)) {
        checkFastJNIAccess(pc, &stub);
      }
    }

    if (stub != NULL) {
      // save all thread context in case we need to restore it
      thread->set_saved_exception_pc(pc);
      thread->set_saved_exception_npc(npc);
      os::Linux::ucontext_set_pc((ucontext_t*)uc, stub);
      return true;
    }
  }

  // signal-chaining
  if (os::Linux::chained_handler(sig, info, ucVoid)) {
    return true;
  }

  if (!abort_if_unrecognized) {
    // caller wants another chance, so give it to him
    return false;
  }

  if (pc == NULL && uc != NULL) {
    pc = os::Linux::ucontext_get_pc((const ucontext_t*)uc);
  }

  // unmask current signal
  sigset_t newset;
  sigemptyset(&newset);
  sigaddset(&newset, sig);
  sigprocmask(SIG_UNBLOCK, &newset, NULL);

  VMError::report_and_die(t, sig, pc, info, ucVoid);

  ShouldNotReachHere();
  return false;
}

void os::Linux::init_thread_fpu_state(void) {
  // Nothing to do
}

int os::Linux::get_fpu_control_word() {
  return 0;
}

void os::Linux::set_fpu_control_word(int fpu) {
  // nothing
}

bool os::is_allocatable(size_t bytes) {
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// thread stack

// Minimum usable stack sizes required to get to user code. Space for
// HotSpot guard pages is added later.
size_t os::Posix::_compiler_thread_min_stack_allowed = 64 * K;
size_t os::Posix::_java_thread_min_stack_allowed = 64 * K;
size_t os::Posix::_vm_internal_thread_min_stack_allowed = 128 * K;

// return default stack size for thr_type
size_t os::Posix::default_stack_size(os::ThreadType thr_type) {
  // default stack size (compiler thread needs larger stack)
  size_t s = (thr_type == os::compiler_thread ? 4 * M : 1 * M);
  return s;
}

#ifndef PRODUCT
void os::verify_stack_alignment() {
}
#endif

int os::extra_bang_size_in_bytes() {
  // SPARC does not require the additional stack bang.
  return 0;
}
