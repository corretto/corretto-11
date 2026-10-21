/*
 * Copyright (c) 2026, Oracle and/or its affiliates. All rights reserved.
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
 */

/*
 * @test 8384736
 * @summary Verify the correct handshake is used for remote agents.
 * @run junit RemoteAgentHandshakeTest
 */

import java.util.*;
import java.util.stream.Collectors;
import jdk.jshell.JShell;
import jdk.jshell.execution.JdiExecutionControlProvider;
import jdk.jshell.execution.RemoteExecutionControl;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

public class RemoteAgentHandshakeTest {

    @Test
    public void testCustomAgentOldHandshake() throws Exception {
        assertThrows(IllegalStateException.class, () -> {
            try (JShell jshell =
                    JShell.builder()
                          .executionEngine(new JdiExecutionControlProvider(),
                                           Map.of(JdiExecutionControlProvider.PARAM_REMOTE_AGENT,
                                                  OldHandShake.class.getName()))
                          .remoteVMOptions("-classpath", System.getProperty("test.classes"))
                          .build()) {
                jshell.eval("1");
            }
        });
    }

    @Test
    public void testCustomAgentNewHandshake() throws Exception {
        try (JShell jshell =
                JShell.builder()
                      .executionEngine(new JdiExecutionControlProvider(),
                                       Map.of(JdiExecutionControlProvider.PARAM_REMOTE_AGENT,
                                              NewHandShake.class.getName()))
                      .remoteVMOptions("-classpath", System.getProperty("test.classes"))
                      .build()) {
            jshell.eval("1");
        }
    }

    @Test
    public void testDefaultAgentUsesNewHandshake() throws Exception {
        List<String> commandLines;
        try (JShell jshell = JShell.create()) {
            jshell.eval("1");
            commandLines = ProcessHandle.current().descendants()
                    .map(descendant -> descendant.info().commandLine())
                    .flatMap(Optional::stream)
                    .collect(Collectors.toList());
        }
        // Unavailable e.g. on Windows
        assumeTrue(!commandLines.isEmpty(), "commandLine is unavailable on this platform");

        String commandLine = commandLines.stream()
                .filter(line -> line.contains("RemoteExecutionControl"))
                .findAny()
                .orElseThrow(() -> new AssertionError("No RemoteExecutionControl among: " + commandLines));
        if (!commandLine.endsWith(" -1")) {
            throw new AssertionError("Does not contain a correct port: '" + commandLine + "'");
        }
    }

    public static class OldHandShake extends RemoteExecutionControl {
        public static void main(String[] args) throws Exception {
            if (Integer.parseInt(args[0]) <= 0) {
                throw new AssertionError("Unexpected port number!");
            }

            RemoteExecutionControl.main(args);
        }
    }

    public static class NewHandShake extends RemoteExecutionControl {
        public static void main(String[] args) throws Exception {
            if (Integer.parseInt(args[0]) != (-1)) {
                throw new AssertionError("Unexpected port number!");
            }

            RemoteExecutionControl.main(args);
        }
    }
}
