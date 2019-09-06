/*
 * Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
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

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.ArrayList;

import jdk.test.lib.apps.LingeredApp;
/**
 * @test
 * @bug 8193124
 * @summary Test the clhsdb 'findpc' command
 * @requires vm.hasSA
 * @requires vm.compiler1.enabled
 * @library /test/lib
 * @run main/othervm ClhsdbFindPC
 */

public class ClhsdbFindPC {

    private static void testFindPC(boolean withXcomp) throws Exception {
        LingeredApp theApp = null;
        try {
            ClhsdbLauncher test = new ClhsdbLauncher();

            theApp = new LingeredAppWithTrivialMain();
            if (withXcomp) {
                LingeredApp.startApp(List.of("-Xcomp"), theApp);
            } else {
                LingeredApp.startApp(List.of("-Xint"), theApp);
            }
            System.out.print("Started LingeredApp ");
            if (withXcomp) {
                System.out.print("(-Xcomp) ");
            } else {
                System.out.print("(-Xint) ");
            }
            System.out.println("with pid " + theApp.getPid());

            // Run 'jstack -v' command to get the pc
            List<String> cmds = List.of("jstack -v");
            String output = test.run(theApp.getPid(), cmds, null, null);

            // Test the 'findpc' command passing in the pc obtained from
            // the 'jstack -v' command
            cmds = new ArrayList<String>();

            // Output could be null if the test was skipped due to
            // attach permission issues.
            if (output != null) {
                String cmdStr = null;
                String[] parts = output.split("LingeredAppWithTrivialMain.main");
                String[] tokens = parts[1].split(" ");
                for (String token : tokens) {
                    if (token.contains("pc")) {
                        String[] address = token.split("=");
                        // address[1] represents the address of the Method
                        cmdStr = "findpc " + address[1].replace(",","");
                        cmds.add(cmdStr);
                        break;
                    }
                }

                Map<String, List<String>> expStrMap = new HashMap<>();
                if (withXcomp) {
                    expStrMap.put(cmdStr, List.of(
                            "In code in NMethod for LingeredAppWithTrivialMain.main",
                            "content:",
                            "oops:",
                            "frame size:"));
                } else {
                    expStrMap.put(cmdStr, List.of(
                            "In interpreter codelet"));
                }

                test.run(theApp.getPid(), cmds, expStrMap, null);
            }
        } catch (Exception ex) {
            throw new RuntimeException("Test ERROR " + ex, ex);
        } finally {
            LingeredApp.stopApp(theApp);
        }
    }

    public static void main(String[] args) throws Exception {
        System.out.println("Starting the ClhsdbFindPC test");
        testFindPC(true);
        testFindPC(false);
        System.out.println("Test PASSED");
    }
}

