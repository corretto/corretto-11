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
 * @test TestGCId
 * @bug 8043607
 * @summary Ensure that the GCId is logged
 * @requires vm.gc=="null"
 * @key gc
 * @library /test/lib
 * @modules java.base/jdk.internal.misc
 *          java.management
 * @build sun.hotspot.WhiteBox
 * @run driver ClassFileInstaller sun.hotspot.WhiteBox sun.hotspot.WhiteBox$WhiteBoxPermission
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions -XX:+WhiteBoxAPI TestGCId
 */

import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;
import sun.hotspot.gc.GC;

public class TestGCId {
  public static void main(String[] args) throws Exception {
    testGCId("UseParallelGC");
    testGCId("UseG1GC");
    testGCId("UseConcMarkSweepGC");
    testGCId("UseSerialGC");
    if (GC.Shenandoah.isSupported()) {
        testGCId("UseShenandoahGC");
    }
  }

  private static void verifyContainsGCIDs(OutputAnalyzer output) {
    output.shouldMatch("\\[.*\\]\\[.*\\]\\[.*\\] GC\\(0\\) ");
    output.shouldMatch("\\[.*\\]\\[.*\\]\\[.*\\] GC\\(1\\) ");
    output.shouldHaveExitValue(0);
  }

  private static void testGCId(String gcFlag) throws Exception {
    ProcessBuilder pb_default =
      ProcessTools.createJavaProcessBuilder("-XX:+UnlockExperimentalVMOptions", "-XX:+" + gcFlag, "-Xlog:gc", "-Xmx10M", GCTest.class.getName());
    verifyContainsGCIDs(new OutputAnalyzer(pb_default.start()));
  }

  static class GCTest {
    private static byte[] garbage;
    public static void main(String [] args) {
      System.out.println("Creating garbage");
      // create 128MB of garbage. This should result in at least one GC
      for (int i = 0; i < 1024; i++) {
        garbage = new byte[128 * 1024];
      }
      // do a system gc to get one more gc
      System.gc();
      System.out.println("Done");
    }
  }
}
