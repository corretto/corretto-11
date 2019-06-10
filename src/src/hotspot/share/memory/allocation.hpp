/*
 * Copyright (c) 1997, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_VM_MEMORY_ALLOCATION_HPP
#define SHARE_VM_MEMORY_ALLOCATION_HPP

#include "runtime/globals.hpp"
#include "utilities/globalDefinitions.hpp"
#include "utilities/macros.hpp"

#include <new>

class AllocFailStrategy {
public:
  enum AllocFailEnum { EXIT_OOM, RETURN_NULL };
};
typedef AllocFailStrategy::AllocFailEnum AllocFailType;

// The virtual machine must never call one of the implicitly declared
// global allocation or deletion functions.  (Such calls may result in
// link-time or run-time errors.)  For convenience and documentation of
// intended use, classes in the virtual machine may be derived from one
// of the following allocation classes, some of which define allocation
// and deletion functions.
// Note: std::malloc and std::free should never called directly.

//
// For objects allocated in the resource area (see resourceArea.hpp).
// - ResourceObj
//
// For objects allocated in the C-heap (managed by: free & malloc and tracked with NMT)
// - CHeapObj
//
// For objects allocated on the stack.
// - StackObj
//
// For classes used as name spaces.
// - AllStatic
//
// For classes in Metaspace (class data)
// - MetaspaceObj
//
// The printable subclasses are used for debugging and define virtual
// member functions for printing. Classes that avoid allocating the
// vtbl entries in the objects should therefore not be the printable
// subclasses.
//
// The following macros and function should be used to allocate memory
// directly in the resource area or in the C-heap, The _OBJ variants
// of the NEW/FREE_C_HEAP macros are used for alloc/dealloc simple
// objects which are not inherited from CHeapObj, note constructor and
// destructor are not called. The preferable way to allocate objects
// is using the new operator.
//
// WARNING: The array variant must only be used for a homogenous array
// where all objects are of the exact type specified. If subtypes are
// stored in the array then must pay attention to calling destructors
// at needed.
//
//   NEW_RESOURCE_ARRAY(type, size)
//   NEW_RESOURCE_OBJ(type)
//   NEW_C_HEAP_ARRAY(type, size)
//   NEW_C_HEAP_OBJ(type, memflags)
//   FREE_C_HEAP_ARRAY(type, old)
//   FREE_C_HEAP_OBJ(objname, type, memflags)
//   char* AllocateHeap(size_t size, const char* name);
//   void  FreeHeap(void* p);
//

// In non product mode we introduce a super class for all allocation classes
// that supports printing.
// We avoid the superclass in product mode to save space.

#ifdef PRODUCT
#define ALLOCATION_SUPER_CLASS_SPEC
#else
#define ALLOCATION_SUPER_CLASS_SPEC : public AllocatedObj
class AllocatedObj {
 public:
  // Printing support
  void print() const;
  void print_value() const;

  virtual void print_on(outputStream* st) const;
  virtual void print_value_on(outputStream* st) const;
};
#endif


/*
 * Memory types
 */
enum MemoryType {
  // Memory type by sub systems. It occupies lower byte.
  mtJavaHeap          = 0x00,  // Java heap
  mtClass             = 0x01,  // memory class for Java classes
  mtThread            = 0x02,  // memory for thread objects
  mtThreadStack       = 0x03,
  mtCode              = 0x04,  // memory for generated code
  mtGC                = 0x05,  // memory for GC
  mtCompiler          = 0x06,  // memory for compiler
  mtInternal          = 0x07,  // memory used by VM, but does not belong to
                                 // any of above categories, and not used for
                                 // native memory tracking
  mtOther             = 0x08,  // memory not used by VM
  mtSymbol            = 0x09,  // symbol
  mtNMT               = 0x0A,  // memory used by native memory tracking
  mtClassShared       = 0x0B,  // class data sharing
  mtChunk             = 0x0C,  // chunk that holds content of arenas
  mtTest              = 0x0D,  // Test type for verifying NMT
  mtTracing           = 0x0E,  // memory used for Tracing
  mtLogging           = 0x0F,  // memory for logging
  mtArguments         = 0x10,  // memory for argument processing
  mtModule            = 0x11,  // memory for module processing
  mtNone              = 0x12,  // undefined
  mt_number_of_types  = 0x13   // number of memory types (mtDontTrack
                                 // is not included as validate type)
};

typedef MemoryType MEMFLAGS;


#if INCLUDE_NMT

extern bool NMT_track_callsite;

#else

const bool NMT_track_callsite = false;

#endif // INCLUDE_NMT

class NativeCallStack;


char* AllocateHeap(size_t size,
                   MEMFLAGS flags,
                   const NativeCallStack& stack,
                   AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);
char* AllocateHeap(size_t size,
                   MEMFLAGS flags,
                   AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);

char* ReallocateHeap(char *old,
                     size_t size,
                     MEMFLAGS flag,
                     AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);

void FreeHeap(void* p);

template <MEMFLAGS F> class CHeapObj ALLOCATION_SUPER_CLASS_SPEC {
 public:
  ALWAYSINLINE void* operator new(size_t size) throw() {
    return (void*)AllocateHeap(size, F);
  }

  ALWAYSINLINE void* operator new(size_t size,
                                  const NativeCallStack& stack) throw() {
    return (void*)AllocateHeap(size, F, stack);
  }

  ALWAYSINLINE void* operator new(size_t size, const std::nothrow_t&,
                                  const NativeCallStack& stack) throw() {
    return (void*)AllocateHeap(size, F, stack, AllocFailStrategy::RETURN_NULL);
  }

  ALWAYSINLINE void* operator new(size_t size, const std::nothrow_t&) throw() {
    return (void*)AllocateHeap(size, F, AllocFailStrategy::RETURN_NULL);
  }

  ALWAYSINLINE void* operator new[](size_t size) throw() {
    return (void*)AllocateHeap(size, F);
  }

  ALWAYSINLINE void* operator new[](size_t size,
                                  const NativeCallStack& stack) throw() {
    return (void*)AllocateHeap(size, F, stack);
  }

  ALWAYSINLINE void* operator new[](size_t size, const std::nothrow_t&,
                                    const NativeCallStack& stack) throw() {
    return (void*)AllocateHeap(size, F, stack, AllocFailStrategy::RETURN_NULL);
  }

  ALWAYSINLINE void* operator new[](size_t size, const std::nothrow_t&) throw() {
    return (void*)AllocateHeap(size, F, AllocFailStrategy::RETURN_NULL);
  }

  void  operator delete(void* p)     { FreeHeap(p); }
  void  operator delete [] (void* p) { FreeHeap(p); }
};

// Base class for objects allocated on the stack only.
// Calling new or delete will result in fatal error.

class StackObj ALLOCATION_SUPER_CLASS_SPEC {
 private:
  void* operator new(size_t size) throw();
  void* operator new [](size_t size) throw();
#ifdef __IBMCPP__
 public:
#endif
  void  operator delete(void* p);
  void  operator delete [](void* p);
};

// Base class for objects stored in Metaspace.
// Calling delete will result in fatal error.
//
// Do not inherit from something with a vptr because this class does
// not introduce one.  This class is used to allocate both shared read-only
// and shared read-write classes.
//

class ClassLoaderData;
class MetaspaceClosure;

class MetaspaceObj {
  // When CDS is enabled, all shared metaspace objects are mapped
  // into a single contiguous memory block, so we can use these
  // two pointers to quickly determine if something is in the
  // shared metaspace.
  //
  // When CDS is not enabled, both pointers are set to NULL.
  static void* _shared_metaspace_base; // (inclusive) low address
  static void* _shared_metaspace_top;  // (exclusive) high address

 public:
  bool is_metaspace_object() const;
  bool is_shared() const {
    // If no shared metaspace regions are mapped, _shared_metaspace_{base,top} will
    // both be NULL and all values of p will be rejected quickly.
    return (((void*)this) < _shared_metaspace_top && ((void*)this) >= _shared_metaspace_base);
  }
  void print_address_on(outputStream* st) const;  // nonvirtual address printing

  static void set_shared_metaspace_range(void* base, void* top) {
    _shared_metaspace_base = base;
    _shared_metaspace_top = top;
  }
  static void* shared_metaspace_base() { return _shared_metaspace_base; }
  static void* shared_metaspace_top()  { return _shared_metaspace_top;  }

#define METASPACE_OBJ_TYPES_DO(f) \
  f(Class) \
  f(Symbol) \
  f(TypeArrayU1) \
  f(TypeArrayU2) \
  f(TypeArrayU4) \
  f(TypeArrayU8) \
  f(TypeArrayOther) \
  f(Method) \
  f(ConstMethod) \
  f(MethodData) \
  f(ConstantPool) \
  f(ConstantPoolCache) \
  f(Annotations) \
  f(MethodCounters)

#define METASPACE_OBJ_TYPE_DECLARE(name) name ## Type,
#define METASPACE_OBJ_TYPE_NAME_CASE(name) case name ## Type: return #name;

  enum Type {
    // Types are MetaspaceObj::ClassType, MetaspaceObj::SymbolType, etc
    METASPACE_OBJ_TYPES_DO(METASPACE_OBJ_TYPE_DECLARE)
    _number_of_types
  };

  static const char * type_name(Type type) {
    switch(type) {
    METASPACE_OBJ_TYPES_DO(METASPACE_OBJ_TYPE_NAME_CASE)
    default:
      ShouldNotReachHere();
      return NULL;
    }
  }

  static MetaspaceObj::Type array_type(size_t elem_size) {
    switch (elem_size) {
    case 1: return TypeArrayU1Type;
    case 2: return TypeArrayU2Type;
    case 4: return TypeArrayU4Type;
    case 8: return TypeArrayU8Type;
    default:
      return TypeArrayOtherType;
    }
  }

  void* operator new(size_t size, ClassLoaderData* loader_data,
                     size_t word_size,
                     Type type, Thread* thread) throw();
                     // can't use TRAPS from this header file.
  void operator delete(void* p) { ShouldNotCallThis(); }

  // Declare a *static* method with the same signature in any subclass of MetaspaceObj
  // that should be read-only by default. See symbol.hpp for an example. This function
  // is used by the templates in metaspaceClosure.hpp
  static bool is_read_only_by_default() { return false; }
};

// Base class for classes that constitute name spaces.

class Arena;

class AllStatic {
 public:
  AllStatic()  { ShouldNotCallThis(); }
  ~AllStatic() { ShouldNotCallThis(); }
};


extern char* resource_allocate_bytes(size_t size,
    AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);
extern char* resource_allocate_bytes(Thread* thread, size_t size,
    AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);
extern char* resource_reallocate_bytes( char *old, size_t old_size, size_t new_size,
    AllocFailType alloc_failmode = AllocFailStrategy::EXIT_OOM);
extern void resource_free_bytes( char *old, size_t size );

//----------------------------------------------------------------------
// Base class for objects allocated in the resource area per default.
// Optionally, objects may be allocated on the C heap with
// new(ResourceObj::C_HEAP) Foo(...) or in an Arena with new (&arena)
// ResourceObj's can be allocated within other objects, but don't use
// new or delete (allocation_type is unknown).  If new is used to allocate,
// use delete to deallocate.
class ResourceObj ALLOCATION_SUPER_CLASS_SPEC {
 public:
  enum allocation_type { STACK_OR_EMBEDDED = 0, RESOURCE_AREA, C_HEAP, ARENA, allocation_mask = 0x3 };
  static void set_allocation_type(address res, allocation_type type) NOT_DEBUG_RETURN;
#ifdef ASSERT
 private:
  // When this object is allocated on stack the new() operator is not
  // called but garbage on stack may look like a valid allocation_type.
  // Store negated 'this' pointer when new() is called to distinguish cases.
  // Use second array's element for verification value to distinguish garbage.
  uintptr_t _allocation_t[2];
  bool is_type_set() const;
 public:
  allocation_type get_allocation_type() const;
  bool allocated_on_stack()    const { return get_allocation_type() == STACK_OR_EMBEDDED; }
  bool allocated_on_res_area() const { return get_allocation_type() == RESOURCE_AREA; }
  bool allocated_on_C_heap()   const { return get_allocation_type() == C_HEAP; }
  bool allocated_on_arena()    const { return get_allocation_type() == ARENA; }
  ResourceObj(); // default constructor
  ResourceObj(const ResourceObj& r); // default copy constructor
  ResourceObj& operator=(const ResourceObj& r); // default copy assignment
  ~ResourceObj();
#endif // ASSERT

 public:
  void* operator new(size_t size, allocation_type type, MEMFLAGS flags) throw();
  void* operator new [](size_t size, allocation_type type, MEMFLAGS flags) throw();
  void* operator new(size_t size, const std::nothrow_t&  nothrow_constant,
      allocation_type type, MEMFLAGS flags) throw();
  void* operator new [](size_t size, const std::nothrow_t&  nothrow_constant,
      allocation_type type, MEMFLAGS flags) throw();

  void* operator new(size_t size, Arena *arena) throw();

  void* operator new [](size_t size, Arena *arena) throw();

  void* operator new(size_t size) throw() {
      address res = (address)resource_allocate_bytes(size);
      DEBUG_ONLY(set_allocation_type(res, RESOURCE_AREA);)
      return res;
  }

  void* operator new(size_t size, const std::nothrow_t& nothrow_constant) throw() {
      address res = (address)resource_allocate_bytes(size, AllocFailStrategy::RETURN_NULL);
      DEBUG_ONLY(if (res != NULL) set_allocation_type(res, RESOURCE_AREA);)
      return res;
  }

  void* operator new [](size_t size) throw() {
      address res = (address)resource_allocate_bytes(size);
      DEBUG_ONLY(set_allocation_type(res, RESOURCE_AREA);)
      return res;
  }

  void* operator new [](size_t size, const std::nothrow_t& nothrow_constant) throw() {
      address res = (address)resource_allocate_bytes(size, AllocFailStrategy::RETURN_NULL);
      DEBUG_ONLY(if (res != NULL) set_allocation_type(res, RESOURCE_AREA);)
      return res;
  }

  void  operator delete(void* p);
  void  operator delete [](void* p);
};

// One of the following macros must be used when allocating an array
// or object to determine whether it should reside in the C heap on in
// the resource area.

#define NEW_RESOURCE_ARRAY(type, size)\
  (type*) resource_allocate_bytes((size) * sizeof(type))

#define NEW_RESOURCE_ARRAY_RETURN_NULL(type, size)\
  (type*) resource_allocate_bytes((size) * sizeof(type), AllocFailStrategy::RETURN_NULL)

#define NEW_RESOURCE_ARRAY_IN_THREAD(thread, type, size)\
  (type*) resource_allocate_bytes(thread, (size) * sizeof(type))

#define NEW_RESOURCE_ARRAY_IN_THREAD_RETURN_NULL(thread, type, size)\
  (type*) resource_allocate_bytes(thread, (size) * sizeof(type), AllocFailStrategy::RETURN_NULL)

#define REALLOC_RESOURCE_ARRAY(type, old, old_size, new_size)\
  (type*) resource_reallocate_bytes((char*)(old), (old_size) * sizeof(type), (new_size) * sizeof(type))

#define REALLOC_RESOURCE_ARRAY_RETURN_NULL(type, old, old_size, new_size)\
  (type*) resource_reallocate_bytes((char*)(old), (old_size) * sizeof(type),\
                                    (new_size) * sizeof(type), AllocFailStrategy::RETURN_NULL)

#define FREE_RESOURCE_ARRAY(type, old, size)\
  resource_free_bytes((char*)(old), (size) * sizeof(type))

#define FREE_FAST(old)\
    /* nop */

#define NEW_RESOURCE_OBJ(type)\
  NEW_RESOURCE_ARRAY(type, 1)

#define NEW_RESOURCE_OBJ_RETURN_NULL(type)\
  NEW_RESOURCE_ARRAY_RETURN_NULL(type, 1)

#define NEW_C_HEAP_ARRAY3(type, size, memflags, pc, allocfail)\
  (type*) AllocateHeap((size) * sizeof(type), memflags, pc, allocfail)

#define NEW_C_HEAP_ARRAY2(type, size, memflags, pc)\
  (type*) (AllocateHeap((size) * sizeof(type), memflags, pc))

#define NEW_C_HEAP_ARRAY(type, size, memflags)\
  (type*) (AllocateHeap((size) * sizeof(type), memflags))

#define NEW_C_HEAP_ARRAY2_RETURN_NULL(type, size, memflags, pc)\
  NEW_C_HEAP_ARRAY3(type, (size), memflags, pc, AllocFailStrategy::RETURN_NULL)

#define NEW_C_HEAP_ARRAY_RETURN_NULL(type, size, memflags)\
  NEW_C_HEAP_ARRAY3(type, (size), memflags, CURRENT_PC, AllocFailStrategy::RETURN_NULL)

#define REALLOC_C_HEAP_ARRAY(type, old, size, memflags)\
  (type*) (ReallocateHeap((char*)(old), (size) * sizeof(type), memflags))

#define REALLOC_C_HEAP_ARRAY_RETURN_NULL(type, old, size, memflags)\
  (type*) (ReallocateHeap((char*)(old), (size) * sizeof(type), memflags, AllocFailStrategy::RETURN_NULL))

#define FREE_C_HEAP_ARRAY(type, old) \
  FreeHeap((char*)(old))

// allocate type in heap without calling ctor
#define NEW_C_HEAP_OBJ(type, memflags)\
  NEW_C_HEAP_ARRAY(type, 1, memflags)

#define NEW_C_HEAP_OBJ_RETURN_NULL(type, memflags)\
  NEW_C_HEAP_ARRAY_RETURN_NULL(type, 1, memflags)

// deallocate obj of type in heap without calling dtor
#define FREE_C_HEAP_OBJ(objname)\
  FreeHeap((char*)objname);

// for statistics
#ifndef PRODUCT
class AllocStats : StackObj {
  julong start_mallocs, start_frees;
  julong start_malloc_bytes, start_mfree_bytes, start_res_bytes;
 public:
  AllocStats();

  julong num_mallocs();    // since creation of receiver
  julong alloc_bytes();
  julong num_frees();
  julong free_bytes();
  julong resource_bytes();
  void   print();
};
#endif


//------------------------------ReallocMark---------------------------------
// Code which uses REALLOC_RESOURCE_ARRAY should check an associated
// ReallocMark, which is declared in the same scope as the reallocated
// pointer.  Any operation that could __potentially__ cause a reallocation
// should check the ReallocMark.
class ReallocMark: public StackObj {
protected:
  NOT_PRODUCT(int _nesting;)

public:
  ReallocMark()   PRODUCT_RETURN;
  void check()    PRODUCT_RETURN;
};

// Helper class to allocate arrays that may become large.
// Uses the OS malloc for allocations smaller than ArrayAllocatorMallocLimit
// and uses mapped memory for larger allocations.
// Most OS mallocs do something similar but Solaris malloc does not revert
// to mapped memory for large allocations. By default ArrayAllocatorMallocLimit
// is set so that we always use malloc except for Solaris where we set the
// limit to get mapped memory.
template <class E>
class ArrayAllocator : public AllStatic {
 private:
  static bool should_use_malloc(size_t length);

  static E* allocate_malloc(size_t length, MEMFLAGS flags);
  static E* allocate_mmap(size_t length, MEMFLAGS flags);

  static void free_malloc(E* addr, size_t length);
  static void free_mmap(E* addr, size_t length);

 public:
  static E* allocate(size_t length, MEMFLAGS flags);
  static E* reallocate(E* old_addr, size_t old_length, size_t new_length, MEMFLAGS flags);
  static void free(E* addr, size_t length);
};

// Uses mmaped memory for all allocations. All allocations are initially
// zero-filled. No pre-touching.
template <class E>
class MmapArrayAllocator : public AllStatic {
 private:
  static size_t size_for(size_t length);

 public:
  static E* allocate_or_null(size_t length, MEMFLAGS flags);
  static E* allocate(size_t length, MEMFLAGS flags);
  static void free(E* addr, size_t length);
};

// Uses malloc:ed memory for all allocations.
template <class E>
class MallocArrayAllocator : public AllStatic {
 public:
  static size_t size_for(size_t length);

  static E* allocate(size_t length, MEMFLAGS flags);
  static void free(E* addr);
};

#endif // SHARE_VM_MEMORY_ALLOCATION_HPP
