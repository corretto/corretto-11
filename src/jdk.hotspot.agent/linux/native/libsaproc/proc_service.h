/*
 * Copyright (c) 2003, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _PROC_SERVICE_H_
#define _PROC_SERVICE_H_

#include <stdio.h>
#include <sys/procfs.h>
#ifdef INCLUDE_SA_ATTACH
#include <thread_db.h>
#endif
#include "jni.h"

// Linux does not have the proc service library, though it does provide the
// thread_db library which can be used to manipulate threads without having
// to know the details of NPTL

// copied from Solaris "proc_service.h"
typedef enum {
        PS_OK,          /* generic "call succeeded" */
        PS_ERR,         /* generic error */
        PS_BADPID,      /* bad process handle */
        PS_BADLID,      /* bad lwp identifier */
        PS_BADADDR,     /* bad address */
        PS_NOSYM,       /* p_lookup() could not find given symbol */
        PS_NOFREGS      /* FPU register set not available for given lwp */
} ps_err_e;

#endif /* _PROC_SERVICE_H_ */
