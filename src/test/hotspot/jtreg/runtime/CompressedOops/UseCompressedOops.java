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
 */

/*
 * @test
 * @bug 8022865
 * @summary Tests for different combination of UseCompressedOops options
 * @library /test/lib
 * @modules java.base/jdk.internal.misc
 *          java.management
 * @build sun.hotspot.WhiteBox
 * @run driver ClassFileInstaller sun.hotspot.WhiteBox sun.hotspot.WhiteBox$WhiteBoxPermission
 * @run main/othervm -XX:+UnlockDiagnosticVMOptions -XX:+WhiteBoxAPI -Xbootclasspath/a:. UseCompressedOops
 */
import java.util.ArrayList;
import java.util.Collections;
import jdk.test.lib.Platform;
import jdk.test.lib.process.ProcessTools;
import jdk.test.lib.process.OutputAnalyzer;
import sun.hotspot.gc.GC;

import sun.hotspot.code.Compiler;

public class UseCompressedOops {

    public static void main(String[] args) throws Exception {
        testCompressedOopsModesGCs();
        testCompressedOopsModesGCs("-XX:+UseLargePages");
    }

    public static void testCompressedOopsModesGCs(String... flags) throws Exception {
        ArrayList<String> args = new ArrayList<>();
        Collections.addAll(args, flags);

        // Test default.
        testCompressedOopsModes(args);
        // Test GCs.
        testCompressedOopsModes(args, "-XX:+UseG1GC");
        if (!Compiler.isGraalEnabled()) { // Graal does not support CMS
            testCompressedOopsModes(args, "-XX:+UseConcMarkSweepGC");
        }
        testCompressedOopsModes(args, "-XX:+UseSerialGC");
        testCompressedOopsModes(args, "-XX:+UseParallelGC");
        testCompressedOopsModes(args, "-XX:+UseParallelOldGC");
        if (GC.Shenandoah.isSupported()) {
            testCompressedOopsModes(args, "-XX:+UnlockExperimentalVMOptions", "-XX:+UseShenandoahGC");
        }
    }

    public static void testCompressedOopsModes(ArrayList<String> flags1, String... flags2) throws Exception {
        ArrayList<String> args = new ArrayList<>();
        args.addAll(flags1);
        Collections.addAll(args, flags2);

        if (Platform.is64bit()) {
            // Explicitly turn of compressed oops
            testCompressedOops(args, "-XX:-UseCompressedOops", "-Xmx32m")
                .shouldNotContain("Compressed Oops")
                .shouldHaveExitValue(0);

            // Compressed oops should be on by default
            testCompressedOops(args, "-Xmx32m")
                .shouldContain("Compressed Oops mode")
                .shouldHaveExitValue(0);

            // Explicly enabling compressed oops
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32m")
                .shouldContain("Compressed Oops mode")
                .shouldHaveExitValue(0);

            // Skip the following three test cases if we're on OSX or Solaris.
            //
            // OSX doesn't seem to care about HeapBaseMinAddress and Solaris
            // puts the heap way up, forcing different behaviour.
            if (!Platform.isOSX() && !Platform.isSolaris()) {
                // Larger than 4gb heap should result in zero based with shift 3
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx5g")
                    .shouldContain("Zero based")
                    .shouldContain("Oop shift amount: 3")
                    .shouldHaveExitValue(0);

                // Larger than 3gb heap and HeapBaseMinAddress=1g should result in zero based with shift 3
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx3200m", "-XX:HeapBaseMinAddress=1g")
                    .shouldContain("Zero based")
                    .shouldContain("Oop shift amount: 3")
                    .shouldHaveExitValue(0);

                // Small heap above 4gb should result in zero based with shift 3
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32m", "-XX:HeapBaseMinAddress=4g")
                    .shouldContain("Zero based")
                    .shouldContain("Oop shift amount: 3")
                    .shouldHaveExitValue(0);

                // Small heap above 32gb should result in non-zero based with shift 3
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32m", "-XX:HeapBaseMinAddress=32g")
                    .shouldContain("Non-zero disjoint base")
                    .shouldContain("Oop shift amount: 3")
                    .shouldHaveExitValue(0);

                // Small heap above 32gb should result in non-zero based with shift 3
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32m", "-XX:HeapBaseMinAddress=72704m")
                    .shouldContain("Non-zero based")
                    .shouldContain("Oop shift amount: 3")
                    .shouldHaveExitValue(0);

                // 32gb heap with heap base above 64gb and object alignment set to 16 bytes should result
                // in non-zero based with shift 4
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32g", "-XX:ObjectAlignmentInBytes=16",
                               "-XX:HeapBaseMinAddress=64g")
                    .shouldContain("Non-zero disjoint base")
                    .shouldContain("Oop shift amount: 4")
                    .shouldHaveExitValue(0);

                // 32gb heap with object alignment set to 16 bytes should result in zero based with shift 4
                testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32g", "-XX:ObjectAlignmentInBytes=16")
                    .shouldContain("Zero based")
                    .shouldContain("Oop shift amount: 4")
                    .shouldHaveExitValue(0);
            }

            // This is a pathologic case for the heap allocation algorithm. Regression test.
            // HeapBaseMinAddress must be 2g and should not be set on the command line.
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx2g")
                .shouldNotContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx29g", "-XX:CompressedClassSpaceSize=1g")
                .shouldNotContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);

            // Explicitly enabling compressed oops with 32gb heap should result a warning
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32g")
                .shouldContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);

            // 32gb heap should not result a warning
            testCompressedOops(args, "-Xmx32g")
                .shouldNotContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);

            // Explicitly enabling compressed oops with 32gb heap and object
            // alignment set to 8 byte should result a warning
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32g", "-XX:ObjectAlignmentInBytes=8")
                .shouldContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);

            // 64gb heap and object alignment set to 16 bytes should result in a warning
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx64g", "-XX:ObjectAlignmentInBytes=16")
                .shouldContain("Max heap size too large for Compressed Oops")
                .shouldHaveExitValue(0);

        } else {
            // Compressed oops should only apply to 64bit platforms
            testCompressedOops(args, "-XX:+UseCompressedOops", "-Xmx32m")
                .shouldContain("Unrecognized VM option 'UseCompressedOops'")
                .shouldHaveExitValue(1);
        }
    }

    private static OutputAnalyzer testCompressedOops(ArrayList<String> flags1, String... flags2) throws Exception {
        ArrayList<String> args = new ArrayList<>();

        // Always run with these three:
        args.add("-XX:+PrintCompressedOopsMode");
        args.add("-Xms32m");

        // Add the extra flags
        args.addAll(flags1);
        Collections.addAll(args, flags2);

        args.add("-version");

        ProcessBuilder pb = ProcessTools.createJavaProcessBuilder(args.toArray(new String[0]));
        return new OutputAnalyzer(pb.start());
    }
}
