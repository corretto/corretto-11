/*
 * Copyright (c) 2019, SAP SE. All rights reserved.
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

/**
 * @test
 * @bug 8220374
 * @summary C2: LoopStripMining doesn't strip as expected
 * @requires vm.compiler2.enabled
 *
 * @run main/othervm -XX:+UnlockDiagnosticVMOptions -XX:+SafepointTimeout -XX:+SafepointALot
 *                   -XX:+AbortVMOnSafepointTimeout -XX:SafepointTimeoutDelay=500 -XX:GuaranteedSafepointInterval=500
 *                   -XX:-TieredCompilation -XX:+UseCountedLoopSafepoints -XX:LoopStripMiningIter=1000
 *                   -XX:LoopUnrollLimit=0 -XX:CompileCommand=compileonly,CheckLoopStripMining::test_loop -Xcomp CheckLoopStripMining
 *
 */

public class CheckLoopStripMining {

  public static int test_loop(int x) {
      int sum = 0;
      if (x != 0) {
          for (int y = 1; y < Integer.MAX_VALUE; ++y) {
              if (y % x == 0) ++sum;
          }
      }
      return sum;
  }

  public static void main(String args[]) {
    int sum = test_loop(3);
    System.out.println("sum: " + sum);
  }
}
