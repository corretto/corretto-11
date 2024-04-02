/*
 * Copyright (c) 2003, 2023, Oracle and/or its affiliates. All rights reserved.
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
 * @bug 4794413
 * @summary Tests that access to temporaryLostComponent from two different threads doesn't cause a deadlock
 * @key headful
 * @run main TemporaryLostComponentDeadlock
*/
import java.awt.Button;
import java.awt.Dialog;
import java.awt.EventQueue;
import java.awt.Frame;

public class TemporaryLostComponentDeadlock {
    static Dialog frame1;
    static Frame frame;

    public static void main(String[] args) throws Exception {
        EventQueue.invokeAndWait(() -> {
            frame = new Frame("frame");
            frame1 = new Dialog(frame, "Frame 1", false);
            frame1.add(new Button("focus owner"));
            frame1.pack();
            frame1.setLocationRelativeTo(null);
            frame1.setVisible(true);
        });

        Thread t1 = new Thread() {
            public void run() {
                synchronized(frame1) {
                    frame1.dispose();
                    synchronized(frame1) {
                        frame1.notify();
                    }
                }
            }
        };
        try {
            synchronized(frame1) {
                t1.start();
                frame1.wait();
            }
        } catch( InterruptedException ie) {
        } finally {
            if (frame != null) {
                frame.dispose();
            }
        }
        System.out.println("Test PASSED");
    }

}// class TemporaryLostComponentDeadlock
