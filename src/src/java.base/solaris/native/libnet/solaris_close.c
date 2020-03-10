/*
 * Copyright (c) 2014, 2019, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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
 */

#include <errno.h>
#include <sys/socket.h>
#include <stropts.h>
#include <unistd.h>
#include "jvm.h"
#include "net_util.h"

/* Support for restartable system calls on Solaris. */

#define RESTARTABLE_RETURN_INT(_cmd) do {             \
    int _result;                                      \
    if (1) {                                          \
        do {                                          \
            _result = _cmd;                           \
        } while((_result == -1) && (errno == EINTR));      \
        return _result;                               \
    }                                                 \
} while(0)

int NET_Read(int s, void* buf, size_t len) {
    RESTARTABLE_RETURN_INT(recv(s, buf, len, 0));
}

int NET_NonBlockingRead(int s, void* buf, size_t len) {
    RESTARTABLE_RETURN_INT(recv(s, buf, len, MSG_DONTWAIT));
}

int NET_RecvFrom(int s, void *buf, int len, unsigned int flags,
                 struct sockaddr *from, socklen_t *fromlen) {
    RESTARTABLE_RETURN_INT(recvfrom(s, buf, len, flags, from, fromlen));
}

int NET_Send(int s, void *msg, int len, unsigned int flags) {
    RESTARTABLE_RETURN_INT(send(s, msg, len, flags));
}

int NET_SendTo(int s, const void *msg, int len,  unsigned  int flags,
               const struct sockaddr *to, int tolen) {
    RESTARTABLE_RETURN_INT(sendto(s, msg, len, flags, to, tolen));
}

int NET_Connect(int s, struct sockaddr *addr, int addrlen) {
    RESTARTABLE_RETURN_INT(connect(s, addr, addrlen));
}

int NET_Accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
    RESTARTABLE_RETURN_INT(accept(s, addr, addrlen));
}

int NET_SocketClose(int fd) {
    return close(fd);
}

int NET_Dup2(int fd, int fd2) {
    return dup2(fd, fd2);
}

int NET_Poll(struct pollfd *ufds, unsigned int nfds, int timeout) {
    RESTARTABLE_RETURN_INT(poll(ufds, nfds, timeout));
}

int NET_Timeout(JNIEnv *env, int s, long timeout, jlong nanoTimeStamp) {
    int result;
    jlong prevNanoTime = nanoTimeStamp;
    jlong nanoTimeout = (jlong) timeout * NET_NSEC_PER_MSEC;
    struct pollfd pfd;
    pfd.fd = s;
    pfd.events = POLLIN;

    for(;;) {
        result = poll(&pfd, 1, nanoTimeout / NET_NSEC_PER_MSEC);
        if (result < 0 && errno == EINTR) {
            jlong newNanoTime = JVM_NanoTime(env, 0);
            nanoTimeout -= newNanoTime - prevNanoTime;
            if (nanoTimeout < NET_NSEC_PER_MSEC)
                return 0;
            prevNanoTime = newNanoTime;
        } else {
            return result;
        }
    }
}
