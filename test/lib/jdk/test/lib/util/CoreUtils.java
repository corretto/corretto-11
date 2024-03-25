/*
 * Copyright (c) 2020, 2022, Oracle and/or its affiliates. All rights reserved.
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

package jdk.test.lib.util;

import jdk.test.lib.Asserts;
import jdk.test.lib.Platform;
import jdk.test.lib.process.OutputAnalyzer;
import jdk.test.lib.process.ProcessTools;

import jtreg.SkippedException;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Scanner;
import java.util.zip.GZIPInputStream;

public class CoreUtils {

    private static final String RUN_SHELL_NO_LIMIT = "ulimit -c unlimited && ";

    /**
     * Returns a {@code ulimit} command that will allow for an unlimited core file size
     * if the platform supports it.
     *
     * @return {@code String} for the ulimit command if supported by the platform,
     * otherwise {@code null}.
     */
    private static String getCoreUlimitCommand() {
        String result = null;
        try {
            OutputAnalyzer output = ProcessTools.executeProcess("sh", "-c", RUN_SHELL_NO_LIMIT + "ulimit -c");
            if (output.getExitValue() != 0) {
                result = null;
            } else if (!output.getStdout().contains("unlimited")) {
                result = null;
            } else {
                result = RUN_SHELL_NO_LIMIT; // success
            }
        } catch (Throwable t) {
            System.out.println("Exception in getCoreUlimitCommand(): " + t.toString());
            result = null;
        }
        System.out.println("Run test with ulimit -c: " +
                (result == null ? "default" : "unlimited"));
        return result;
    }

    /**
     * Return a {@code ProcessBuilder} that has been prefixed with
     * a {@code ulimit} command to allow for an unlimited core file size.
     *
     * @param pb {@code ProcessBuilder} to prefix with the ulimit command
     * @return New {@code ProcessBuilder} with prefixed {@code ulimit} command if
     * supported. Otherwise the passed in {@code ProcessBuilder} is returned.
     */
    public static ProcessBuilder addCoreUlimitCommand(ProcessBuilder pb) {
        String cmd = ProcessTools.getCommandLine(pb);
        String ulimitCmd = getCoreUlimitCommand();
        if (ulimitCmd == null) {
            return pb;
        } else {
            if (Platform.isWindows()) {
                // In order to launch on Windows using "sh -c", we need to first
                // convert the path to use forward slashes and do some extra quoting.
                cmd = cmd.replace('\\', '/').replace(";", "\\;").replace("|", "\\|");
            }
            return new ProcessBuilder("sh", "-c", ulimitCmd + cmd);
        }
    }

    /**
     * Find the path to the core file mentioned in the output and return its path.
     *
     * @param crashOutputString {@code String} to search in for the core file path
     * @return Location of core file if found in the output, otherwise {@code null}.
     */
    public static String getCoreFileLocation(String crashOutputString) throws IOException {
        unzipCores(new File("."));

        // Find the core file
        String coreFileLocation = parseCoreFileLocationFromOutput(crashOutputString);
        if (coreFileLocation != null) {
            Asserts.assertGT(new File(coreFileLocation).length(), 0L, "Unexpected core size");
            System.out.println("Found core file: " + coreFileLocation);
            return coreFileLocation; // success!
        }

        // See if we can figure out the likely reason the core file was not found.
        // Throw SkippedException if appropriate.
        if (Platform.isOSX()) {
            File coresDir = new File("/cores");
            if (!coresDir.isDirectory()) {
                throw new RuntimeException(coresDir + " is not a directory");
            }
            // The /cores directory is usually not writable on macOS 10.15
            if (!coresDir.canWrite()) {
                throw new SkippedException("Directory \"" + coresDir + "\" is not writable");
            }
            if (Platform.isHardenedOSX()) {
                if (Platform.getOsVersionMajor() > 10 ||
                        (Platform.getOsVersionMajor() == 10 && Platform.getOsVersionMinor() >= 15))
                {
                    // We can't generate cores files with hardened binaries on OSX 10.15 and later.
                    throw new SkippedException("Cannot produce core file with hardened binary on OSX 10.15 and later");
                }
            }
        } else if (Platform.isLinux()) {
            // Check if a crash report tool is installed.
            File corePatternFile = new File(CORE_PATTERN_FILE_NAME);
            try (Scanner scanner = new Scanner(corePatternFile)) {
                while (scanner.hasNextLine()) {
                    String line = scanner.nextLine();
                    line = line.trim();
                    System.out.println(line);
                    if (line.startsWith("|")) {
                        System.out.println(
                                "\nThis system uses a crash report tool ($cat /proc/sys/kernel/core_pattern).\n" +
                                        "Core files might not be generated. Please reset /proc/sys/kernel/core_pattern\n" +
                                        "to enable core generation. Skipping this test.");
                        throw new SkippedException("This system uses a crash report tool");
                    }
                }
            }
        }
        throw new RuntimeException("Couldn't find core file location in: '" + crashOutputString + "'");
    }

    private static final String CORE_PATTERN_FILE_NAME = "/proc/sys/kernel/core_pattern";
    private static final String LOCATION_STRING = "location: ";

    private static String parseCoreFileLocationFromOutput(String crashOutputString) {
        System.out.println("crashOutputString = [" + crashOutputString + "]");
        if(crashOutputString == null || crashOutputString.equals("")) {
            return null;
        }
        // Find the line of output that contains LOCATION_STRING
        Asserts.assertTrue(crashOutputString.contains(LOCATION_STRING),
                "Output doesn't contain the location of core file.");
        String stringWithLocation = Arrays.stream(crashOutputString.split("\\r?\\n"))
                .filter(str -> str.contains(LOCATION_STRING))
                .findFirst()
                .get();
        stringWithLocation = stringWithLocation.substring(stringWithLocation
                .indexOf(LOCATION_STRING) + LOCATION_STRING.length());
        System.out.println("getCoreFileLocation found stringWithLocation = " + stringWithLocation);

        // Find the core file name in the output.
        String coreWithPid;
        if (stringWithLocation.contains("or ") && !Platform.isWindows()) {
            Matcher m = Pattern.compile("or.* ([^ ]+[^\\)])\\)?").matcher(stringWithLocation);
            if (!m.find()) {
                throw new RuntimeException("Couldn't find path to core inside location string");
            }
            coreWithPid = m.group(1);
        } else {
            coreWithPid = stringWithLocation.trim();
        }
        if (new File(coreWithPid).exists()) {
            return coreWithPid;
        }

        // Look for file named "core" in the cwd.
        String justCore = Paths.get("core").toString();
        if (new File(justCore).exists()) {
            return justCore;
        }

        // Look for the core file name found in the output, but do so in the cwd.
        Path coreWithPidPath = Paths.get(coreWithPid);
        String justFile = coreWithPidPath.getFileName().toString();
        if (new File(justFile).exists()) {
            return justFile;
        }

        // Look for file named "core" in the path to the core file found in the output.
        Path parent = coreWithPidPath.getParent();
        if (parent != null) {
            String coreWithoutPid = parent.resolve("core").toString();
            if (new File(coreWithoutPid).exists()) {
                return coreWithoutPid;
            }
        }
        return null;
    }

    private static void unzipCores(File dir) {
        File[] gzCores = dir.listFiles((directory, name) -> name.matches("core(\\.\\d+)?\\.gz"));
        for (File gzCore : gzCores) {
            String coreFileName = gzCore.getName().replace(".gz", "");
            System.out.println("Unzipping core into " + coreFileName);
            try (GZIPInputStream gzis = new GZIPInputStream(Files.newInputStream(gzCore.toPath()))) {
                Files.copy(gzis, Paths.get(coreFileName));
            } catch (IOException e) {
                throw new SkippedException("Not able to unzip file: " + gzCore.getAbsolutePath(), e);
            }
        }
    }

}