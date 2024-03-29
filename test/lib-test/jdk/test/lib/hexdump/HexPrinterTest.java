/*
 * Copyright (c) 2019, 2020, Oracle and/or its affiliates. All rights reserved.
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

package jdk.test.lib.hexdump;

import jdk.test.lib.hexdump.HexPrinter;

import org.testng.Assert;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;


/*
 * @test
 * @summary Check HexPrinter formatting
 * @library /test/lib
 * @compile HexPrinterTest.java
 * @run testng jdk.test.lib.hexdump.HexPrinterTest
 */
public class HexPrinterTest {

    @Test
    static void testMinimalToStringByteArray() {
        int len = 16;
        byte[] bytes = genData(len);
        StringBuilder expected = new StringBuilder(bytes.length * 2);
        for (int i = 0; i < len; i++)
            expected.append(String.format("%02x", bytes[i]));
        String actual = HexPrinter.minimal().toString(bytes);
        System.out.println(actual);
        Assert.assertEquals(actual, expected.toString(), "Minimal format incorrect");
    }

    @DataProvider(name = "ColumnParams")
    Object[][] columnParams() {
        return new Object[][]{
                {"%4d: ", "%d ", 10, " ; ", 50, HexPrinter.Formatters.PRINTABLE, "\n"},
                {"%03o: ", "%d ", 16, " ; ", 50, HexPrinter.Formatters.ofPrimitive(byte.class, ""), "\n"},
                {"%5d: ", "%02x:", 16, " ; ", 50, HexPrinter.Formatters.ofPrimitive(byte.class, ""), "\n"},
                {"%5d: ", "%3d", 16, " ; ", 50, HexPrinter.Formatters.ofPrimitive(byte.class, ""), "\n"},
                {"%05o: ", "%3o", 8, " ; ", 50, HexPrinter.Formatters.ofPrimitive(byte.class, ""), "\n"},
                {"%6x: ", "%02x", 8, " | ", 50, HexPrinter.Formatters.ofPrimitive(byte.class, "%d "), "\n"},
                {"%2x: ", "%02x", 8, " | ", 50, HexPrinter.Formatters.PRINTABLE, "\n"},
                {"%5d: ", "%02x", 16, " | ", 50, HexPrinter.Formatters.ofPrimitive(short.class, "%d "), "\n"},
        };
    }

    @DataProvider(name = "BuiltinParams")
    Object[][] builtinParams() {
        return new Object[][]{
                {"minimal", "", "%02x", 16, "", 64, HexPrinter.Formatters.NONE, ""},
                {"canonical", "%08x  ", "%02x ", 16, "|", 31, HexPrinter.Formatters.ASCII, "|\n"},
                {"simple", "%5d: ", "%02x ", 16, " // ", 64, HexPrinter.Formatters.PRINTABLE, "\n"},
                {"source", "    ", "(byte)%3d, ", 8, " // ", 64, HexPrinter.Formatters.PRINTABLE,
                        "\n"},
        };
    }

    @Test(dataProvider = "BuiltinParams")
    public void testBuiltins(String name, String offsetFormat, String binFormat, int colWidth,
                             String annoDelim, int annoWidth,
                             HexPrinter.Formatter mapper, String lineSep) {
        HexPrinter f = switch (name) {
            case "minimal" -> HexPrinter.minimal();
            case "simple" -> HexPrinter.simple();
            case "canonical" -> HexPrinter.canonical();
            case "source" -> HexPrinter.source();
            default -> throw new IllegalStateException("Unexpected value: " + name);
        };

        testParams(f, offsetFormat, binFormat, colWidth, annoDelim, annoWidth, mapper, lineSep);

        String actual = f.toString();
        HexPrinter f2 = HexPrinter.simple()
                .withOffsetFormat(offsetFormat)
                .withBytesFormat(binFormat, colWidth)
                .formatter(mapper, annoDelim, annoWidth)
                .withLineSeparator(lineSep);
        String expected = f2.toString();
        Assert.assertEquals(actual, expected, "toString of " + name + " does not match");
    }

    @Test(dataProvider = "ColumnParams")
    public void testToStringTwoLines(String offsetFormat, String binFormat, int colWidth,
                                     String annoDelim, int annoWidth,
                                     HexPrinter.Formatter mapper, String lineSep) {
        HexPrinter f = HexPrinter.simple()
                .withOffsetFormat(offsetFormat)
                .withBytesFormat(binFormat, colWidth)
                .formatter(mapper, annoDelim, annoWidth)
                .withLineSeparator(lineSep);
        testParams(f, offsetFormat, binFormat, colWidth, annoDelim, annoWidth, mapper, lineSep);
    }

    public static void testParams(HexPrinter printer, String offsetFormat, String binFormat, int colWidth,
                                  String annoDelim, int annoWidth,
                                  HexPrinter.Formatter mapper, String lineSep) {
        byte[] bytes = genData(colWidth * 2);
        System.out.println("Params: " + printer.toString());
        String out = printer.toString(bytes);
        System.out.println(out);

        // Compare the actual output with the expected output of each formatting element
        int padToWidth = colWidth * String.format(binFormat, 0xff).length();
        int ndx = 0;
        int valuesStart = 0;
        int l;
        for (int i = 0; i < bytes.length; i++) {
            if (i % colWidth == 0) {
                String offset = String.format(offsetFormat, i);
                l = offset.length();
                Assert.assertEquals(out.substring(ndx, ndx + l), offset,
                        "offset format mismatch: " + ndx);
                ndx += l;
                valuesStart = ndx;
            }
            String value = String.format(binFormat, (0xff & bytes[i]));
            l = value.length();
            Assert.assertEquals(out.substring(ndx, ndx + l), value,
                    "value format mismatch: " + ndx + ", i: " + i);
            ndx += l;
            if (((i + 1) % colWidth) == 0) {
                // Rest of line is for padding, delimiter, formatter
                String padding = " ".repeat(padToWidth - (ndx - valuesStart));
                Assert.assertEquals(out.substring(ndx, ndx + padding.length()), padding, "padding");
                ndx += padding.length();
                Assert.assertEquals(out.substring(ndx, ndx + annoDelim.length()), annoDelim,
                        "delimiter mismatch");
                ndx += annoDelim.length();

                // Formatter output is tested separately
                ndx = out.indexOf(lineSep, ndx) + lineSep.length();
            }
        }
    }

    @DataProvider(name = "PrimitiveFormatters")
    Object[][] formatterParams() {
        return new Object[][]{
                {byte.class, ""},
                {byte.class, "%02x: "},
                {short.class, "%d "},
                {int.class, "%08x, "},
                {long.class, "%16x "},
                {float.class, "%3.4f "},
                {double.class, "%6.3g "},
                {boolean.class, "%b "},
        };
    }

    @Test(dataProvider = "PrimitiveFormatters")
    public void testFormatter(Class<?> primClass, String fmtString) {
        HexPrinter.Formatter formatter = HexPrinter.Formatters.ofPrimitive(primClass, fmtString);
        // Create a byte array with data for two lines
        int colWidth = 8;
        byte[] bytes = genData(colWidth);
        StringBuilder sb = new StringBuilder();
        DataInputStream in = new DataInputStream(new ByteArrayInputStream(bytes));
        DataInputStream in2 = new DataInputStream(new ByteArrayInputStream(bytes));
        try {
            while (true) {
                formatter.annotate(in, sb);
                Object n = readPrimitive(primClass, in2);
                String expected = String.format(fmtString, n);
                Assert.assertEquals(sb.toString(), expected, "mismatch");
                sb.setLength(0);
            }
        } catch (IOException ioe) {
            // EOF is done
        }
        try {
            Assert.assertEquals(in.available(), 0, "not all input consumed");
            Assert.assertEquals(in2.available(), 0, "not all 2nd stream input consumed");
        } catch (IOException ioe) {
            //
        }
    }

    @Test(dataProvider = "PrimitiveFormatters")
    static void testHexPrinterPrimFormatter(Class<?> primClass, String fmtString) {
        // Create a byte array with data for two lines
        int colWidth = 8;
        byte[] bytes = genData(colWidth);

        HexPrinter p = HexPrinter.simple()
                .formatter(primClass, fmtString);
        String actual = p.toString(bytes);
        System.out.println(actual);
        // The formatter should produce the same output as using the formatter method
        // with an explicit formatter for the primitive
        String expected = HexPrinter.simple()
                .formatter(HexPrinter.Formatters.ofPrimitive(primClass, fmtString))
                .toString(bytes);
        Assert.assertEquals(actual, expected, "mismatch");
    }

    private static Object readPrimitive(Class<?> primClass, DataInputStream in) throws IOException {
        if (int.class.equals(primClass)) {
            return in.readInt();
        } else if (byte.class.equals(primClass)) {
            return (int) in.readByte();
        } else if (short.class.equals(primClass)) {
            return in.readShort();
        } else if (char.class.equals(primClass)) {
            return in.readChar();
        } else if (long.class.equals(primClass)) {
            return in.readLong();
        } else if (float.class.equals(primClass)) {
            return in.readFloat();
        } else if (double.class.equals(primClass)) {
            return in.readDouble();
        } else if (boolean.class.equals(primClass)) {
            return in.readBoolean();
        } else {
            throw new RuntimeException("unknown primitive class: " + primClass);
        }
    }

    @DataProvider(name = "sources")
    Object[][] sources() {
        return new Object[][]{
                {genBytes(21), 0, -1},
                {genBytes(21), 5, 12},
        };
    }

    public static byte[] genData(int len) {
        // Create a byte array with data for two lines
        byte[] bytes = new byte[len];
        for (int i = 0; i < len / 2; i++) {
            bytes[i] = (byte) (i + 'A');
            bytes[i + len / 2] = (byte) (i + 'A' + 128);
        }
        return bytes;
    }

    public static byte[] genFloat(int len) {
        byte[] bytes = null;
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream();
             DataOutputStream out = new DataOutputStream(baos)) {
            for (int i = 0; i < len; i++) {
                out.writeFloat(i);
            }
            bytes = baos.toByteArray();
        } catch (IOException unused) {
        }
        return bytes;
    }

    public static byte[] genDouble(int len) {
        byte[] bytes = null;
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream();
             DataOutputStream out = new DataOutputStream(baos)) {
            for (int i = 0; i < len; i++) {
                out.writeDouble(i);
            }
            bytes = baos.toByteArray();
        } catch (IOException unused) {
        }
        return bytes;
    }

    public static byte[] genBytes(int len) {
        byte[] bytes = new byte[len];
        for (int i = 0; i < len; i++)
            bytes[i] = (byte) ('A' + i);
        return bytes;
    }

    public ByteBuffer genByteBuffer(int len) {
        return ByteBuffer.wrap(genBytes(len));
    }

    public InputStream genInputStream(int len) {
        return new ByteArrayInputStream(genBytes(len));
    }

    @Test
    public void testNilPrinterBigBuffer() {
        byte[] bytes = new byte[1024];
        HexPrinter p = HexPrinter.minimal();
        String r = p.toString(bytes);
        Assert.assertEquals(r.length(), bytes.length * 2, "encoded byte wrong size");
        Assert.assertEquals(r.replace("00", "").length(), 0, "contents not all zeros");
    }

    @Test(dataProvider = "sources")
    public void testToStringByteBuffer(byte[] bytes, int offset, int length) {
        if (length < 0)
            length = bytes.length - offset;
        ByteBuffer bb = ByteBuffer.wrap(bytes, 0, bytes.length);
        System.out.printf("Source: %s, off: %d, len: %d%n",
                bytes.getClass().getName(), offset, length);
        String actual;
        if (offset == 0 && length < 0) {
            bb.position(offset);
            bb.limit(length);
            actual = HexPrinter.simple().toString(bb);
        } else
            actual = HexPrinter.simple().toString(bb, offset, length);
        System.out.println(actual);
        String expected = HexPrinter.simple().toString(bytes, offset, length);
        Assert.assertEquals(actual, expected, "mismatch in format()");
    }

    @Test(dataProvider = "sources")
    public void testFormatBytes(byte[] bytes, int offset, int length) {
        int len = length >= 0 ? length : bytes.length;
        System.out.printf("Source: %s, off: %d, len: %d%n",
                "bytes", offset, len);
        StringBuilder sb = new StringBuilder();
        if (offset == 0 && length < 0)
            HexPrinter.simple().dest(sb).format(bytes);
        else
            HexPrinter.simple().dest(sb).format(bytes, offset, len);
        String actual = sb.toString();
        System.out.println(actual);
        String expected = HexPrinter.simple().toString(bytes, offset, len);
        Assert.assertEquals(actual, expected, "mismatch in format()");
    }

    @Test(dataProvider = "sources")
    public void testFormatByteBuffer(byte[] bytes, int offset, int length) {
        if (length < 0)
            length = bytes.length - offset;
        ByteBuffer bb = ByteBuffer.wrap(bytes, 0, bytes.length);
        System.out.printf("Source: %s, off: %d, len: %d%n",
                bytes.getClass().getName(), offset, length);
        StringBuilder sb = new StringBuilder();
        if (offset == 0 && length < 0) {
            bb.position(offset);
            bb.limit(length);
            HexPrinter.simple().dest(sb).format(bb);
        } else
            HexPrinter.simple().dest(sb).format(bb, offset, length);
        String actual = sb.toString();
        System.out.println(actual);
        String expected = HexPrinter.simple().toString(bytes, offset, length);
        Assert.assertEquals(actual, expected, "mismatch in format()");
    }

    @Test(dataProvider = "sources")
    public void testFormatInputStream(byte[] bytes, int offset, int length) {
        // Offset is ignored
        InputStream is = new ByteArrayInputStream(bytes, 0, length);
        StringBuilder sb = new StringBuilder();
        System.out.printf("Source: %s, off: %d, len: %d%n",
                bytes.getClass().getName(), offset, length);
        HexPrinter.simple().dest(sb).format(is);
        String actual = sb.toString();
        System.out.println(actual);
        String expected = HexPrinter.simple().toString(bytes, 0, length);
        Assert.assertEquals(actual, expected, "mismatch in format()");
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void testNullByteArray() {
        HexPrinter.simple().dest(System.out).format((byte[]) null);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void testNullByteArrayOff() {
        HexPrinter.simple().dest(System.out).format((byte[]) null, 0, 1);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void testNullByteBuffer() {
        HexPrinter.simple().dest(System.out).format((ByteBuffer) null);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void testNullByteBufferOff() {
        HexPrinter.simple().dest(System.out).format((ByteBuffer) null, 0, 1);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void testNullInputStream() {
        HexPrinter.simple().dest(System.out).format((InputStream) null);
    }

}
