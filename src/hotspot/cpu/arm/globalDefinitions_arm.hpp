/*
 * Copyright (c) 2008, 2016, Oracle and/or its affiliates. All rights reserved.
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

#ifndef CPU_ARM_VM_GLOBALDEFINITIONS_ARM_HPP
#define CPU_ARM_VM_GLOBALDEFINITIONS_ARM_HPP

#ifdef AARCH64
#define AARCH64_ONLY(code) code
#define AARCH64_ONLY_ARG(arg) , arg
#define NOT_AARCH64(code)
#define NOT_AARCH64_ARG(arg)
#else
#define AARCH64_ONLY(code)
#define AARCH64_ONLY_ARG(arg)
#define NOT_AARCH64(code) code
#define NOT_AARCH64_ARG(arg) , arg
#endif

const int StackAlignmentInBytes = AARCH64_ONLY(16) NOT_AARCH64(8);

// Indicates whether the C calling conventions require that
// 32-bit integer argument values are extended to 64 bits.
const bool CCallingConventionRequiresIntsAsLongs = false;

#ifdef __SOFTFP__
const bool HaveVFP = false;
#else
const bool HaveVFP = true;
#endif

#if defined(__ARM_PCS_VFP) || defined(AARCH64)
#define __ABI_HARD__
#endif

#if defined(__ARM_ARCH_7A__) || defined(AARCH64)
#define SUPPORTS_NATIVE_CX8
#endif

#define STUBROUTINES_MD_HPP    "stubRoutines_arm.hpp"
#define INTERP_MASM_MD_HPP     "interp_masm_arm.hpp"
#define TEMPLATETABLE_MD_HPP   "templateTable_arm.hpp"
#ifdef AARCH64
#define ADGLOBALS_MD_HPP       "adfiles/adGlobals_arm_64.hpp"
#define AD_MD_HPP              "adfiles/ad_arm_64.hpp"
#else
#define ADGLOBALS_MD_HPP       "adfiles/adGlobals_arm_32.hpp"
#define AD_MD_HPP              "adfiles/ad_arm_32.hpp"
#endif
#define C1_LIRGENERATOR_MD_HPP "c1_LIRGenerator_arm.hpp"

#endif // CPU_ARM_VM_GLOBALDEFINITIONS_ARM_HPP
