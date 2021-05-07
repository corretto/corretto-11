/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
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

package jdk.test.lib;

import java.io.IOException;
import java.io.PrintStream;
import java.io.UncheckedIOException;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static java.net.NetworkInterface.getNetworkInterfaces;
import static java.util.Collections.list;

/**
 * Helper class for retrieving network interfaces and local addresses
 * suitable for testing.
 */
public class NetworkConfiguration {

    private Map<NetworkInterface,List<Inet4Address>> ip4Interfaces;
    private Map<NetworkInterface,List<Inet6Address>> ip6Interfaces;
    private final boolean isIPv6Available;
    private boolean has_testableipv6address = false;
    private boolean has_sitelocaladdress = false;
    private boolean has_linklocaladdress = false;
    private boolean has_globaladdress = false;

    private NetworkConfiguration(
            Map<NetworkInterface,List<Inet4Address>> ip4Interfaces,
            Map<NetworkInterface,List<Inet6Address>> ip6Interfaces) {
        this.ip4Interfaces = ip4Interfaces;
        this.ip6Interfaces = ip6Interfaces;

        // initialize properties that can be queried
        isIPv6Available = !ip6Interfaces().collect(Collectors.toList()).isEmpty();
        ip6Interfaces().forEach(nif -> {
            ip6Addresses(nif)
                // On Solaris or AIX, a configuration with only local or loopback
                // addresses does not fully enable IPv6 operations.
                // E.g. IPv6 multicasting does not work.
                // So, don't set has_testableipv6address if we only find these.
                .filter(addr -> Platform.isSolaris() || Platform.isAix() ?
                    !(addr.isAnyLocalAddress() || addr.isLoopbackAddress()) : true)
                .forEach(ia -> {
                    has_testableipv6address = true;
                    if (ia.isLinkLocalAddress()) has_linklocaladdress = true;
                    if (ia.isSiteLocalAddress()) has_sitelocaladdress = true;

                    if (!ia.isLinkLocalAddress() &&
                        !ia.isSiteLocalAddress() &&
                        !ia.isLoopbackAddress()) {
                        has_globaladdress = true;
                    }
                });
        });
    }

    private static boolean isNotExcludedInterface(NetworkInterface nif) {
        if (Platform.isOSX() && nif.getName().contains("awdl")) {
            return false;
        }
        if (Platform.isWindows()) {
            String dName = nif.getDisplayName();
            if (dName != null && dName.contains("Teredo")) {
                return false;
            }
        }
        return true;
    }

    private static boolean isNotLoopback(NetworkInterface nif) {
        try {
            return !nif.isLoopback();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    private boolean hasIp4Addresses(NetworkInterface nif) {
        return ip4Interfaces.get(nif).stream().anyMatch(a -> !a.isAnyLocalAddress());
    }

    private boolean hasIp6Addresses(NetworkInterface nif) {
        return ip6Interfaces.get(nif).stream().anyMatch(a -> !a.isAnyLocalAddress());
    }

    private boolean supportsIp4Multicast(NetworkInterface nif) {
        try {
            if (!nif.supportsMulticast()) {
                return false;
            }

            // On AIX there is a bug:
            // When IPv6 is enabled on the system, the JDK opens sockets as AF_INET6.
            // If there's an interface configured with IPv4 addresses only, it should
            // be able to become the network interface for a multicast socket (that
            // could be in both, IPv4 or IPv6 space). But both possible setsockopt
            // calls for either IPV6_MULTICAST_IF or IP_MULTICAST_IF return
            // EADDRNOTAVAIL. So we must skip such interfaces here.
            if (Platform.isAix() && isIPv6Available() && !hasIp6Addresses(nif)) {
                return false;
            }

            return hasIp4Addresses(nif);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    private boolean supportsIp6Multicast(NetworkInterface nif) {
        try {
            if (!nif.supportsMulticast()) {
                return false;
            }

            return hasIp6Addresses(nif);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    /**
     * Returns whether IPv6 is available at all.
     * This should resemble the result of native ipv6_available() in net_util.c
     */
    public boolean isIPv6Available() {
        return isIPv6Available;
    }

    /**
     * Does any (usable) IPv6 address exist in the network configuration?
     */
    public boolean hasTestableIPv6Address() {
        return has_testableipv6address;
    }

    /**
     * Does any site local address exist?
     */
    public boolean hasSiteLocalAddress() {
        return has_sitelocaladdress;
    }

    /**
     * Does any link local address exist?
     */
    public boolean hasLinkLocalAddress() {
        return has_linklocaladdress;
    }

    /**
     * Does any global IPv6 address exist?
     */
    public boolean has_globaladdress() {
        return has_globaladdress;
    }

    /**
     * Returns a stream of interfaces suitable for functional tests.
     */
    public Stream<NetworkInterface> interfaces() {
        return Stream.concat(ip4Interfaces(), ip6Interfaces())
                     .distinct();
    }

    /**
     * Returns a stream of interfaces suitable for IPv4 functional tests.
     */
    public Stream<NetworkInterface> ip4Interfaces() {
        return ip4Interfaces.keySet()
                            .stream()
                            .filter(NetworkConfiguration::isNotExcludedInterface)
                            .filter(this::hasIp4Addresses);
    }

    /**
     * Returns a stream of interfaces suitable for IPv6 functional tests.
     */
    public Stream<NetworkInterface> ip6Interfaces() {
        return ip6Interfaces.keySet()
                            .stream()
                            .filter(NetworkConfiguration::isNotExcludedInterface)
                            .filter(this::hasIp6Addresses);
    }

    /**
     * Returns a stream of interfaces suitable for functional tests.
     */
    public Stream<NetworkInterface> multicastInterfaces(boolean includeLoopback) {
        return Stream
            .concat(ip4MulticastInterfaces(includeLoopback),
                    ip6MulticastInterfaces(includeLoopback))
            .distinct();
    }

    /**
     * Returns a stream of interfaces suitable for IPv4 multicast tests.
     *
     * The loopback interface will not be included.
     */
    public Stream<NetworkInterface> ip4MulticastInterfaces() {
        return ip4MulticastInterfaces(false);
    }

    /**
     * Returns a stream of interfaces suitable for IPv4 multicast tests.
     */
    public Stream<NetworkInterface> ip4MulticastInterfaces(boolean includeLoopback) {
        return (includeLoopback) ?
            ip4Interfaces().filter(this::supportsIp4Multicast) :
            ip4Interfaces().filter(this::supportsIp4Multicast)
                .filter(NetworkConfiguration::isNotLoopback);
    }

    /**
     * Returns a stream of interfaces suitable for IPv6 multicast tests.
     *
     * The loopback interface will not be included.
     */
    public Stream<NetworkInterface> ip6MulticastInterfaces() {
        return ip6MulticastInterfaces(false);
    }

    /**
     * Returns a stream of interfaces suitable for IPv6 multicast tests.
     */
    public Stream<NetworkInterface> ip6MulticastInterfaces(boolean includeLoopback) {
        return (includeLoopback) ?
            ip6Interfaces().filter(this::supportsIp6Multicast) :
            ip6Interfaces().filter(this::supportsIp6Multicast)
                .filter(NetworkConfiguration::isNotLoopback);
    }

    /**
     * Returns all addresses on all "functional" interfaces.
     */
    public Stream<InetAddress> addresses(NetworkInterface nif) {
        return Stream.concat(ip4Interfaces.get(nif).stream(),
                             ip6Interfaces.get(nif).stream());
    }

    /**
     * Returns all IPv4 addresses on all "functional" interfaces.
     */
    public Stream<Inet4Address> ip4Addresses() {
        return ip4Interfaces().flatMap(this::ip4Addresses);
    }

    /**
     * Returns all IPv6 addresses on all "functional" interfaces.
     */
    public Stream<Inet6Address> ip6Addresses() {
        return ip6Interfaces().flatMap(this::ip6Addresses);
    }

    /**
     * Returns all IPv4 addresses the given interface.
     */
    public Stream<Inet4Address> ip4Addresses(NetworkInterface nif) {
        return ip4Interfaces.get(nif).stream();
    }

    /**
     * Returns all IPv6 addresses for the given interface.
     */
    public Stream<Inet6Address> ip6Addresses(NetworkInterface nif) {
        return ip6Interfaces.get(nif).stream();
    }

    @Override
    public String toString() {
        return interfaces().map(NetworkConfiguration::interfaceInformation)
                           .collect(Collectors.joining());
    }

    /**
     * Return a NetworkConfiguration instance.
     */
    public static NetworkConfiguration probe() throws IOException {
        Map<NetworkInterface, List<Inet4Address>> ip4Interfaces = new HashMap<>();
        Map<NetworkInterface, List<Inet6Address>> ip6Interfaces = new HashMap<>();

        List<NetworkInterface> nifs = list(getNetworkInterfaces());
        for (NetworkInterface nif : nifs) {
            // ignore interfaces that are down
            if (!nif.isUp() || nif.isPointToPoint()) {
                continue;
            }

            List<Inet4Address> ip4Addresses = new LinkedList<>();
            List<Inet6Address> ip6Addresses = new LinkedList<>();
            ip4Interfaces.put(nif, ip4Addresses);
            ip6Interfaces.put(nif, ip6Addresses);
            for (InetAddress addr : list(nif.getInetAddresses())) {
                if (addr instanceof Inet4Address) {
                    ip4Addresses.add((Inet4Address) addr);
                } else if (addr instanceof Inet6Address) {
                    ip6Addresses.add((Inet6Address) addr);
                }
            }
        }
        return new NetworkConfiguration(ip4Interfaces, ip6Interfaces);
    }

    /** Returns detailed information for the given interface. */
    public static String interfaceInformation(NetworkInterface nif) {
        StringBuilder sb = new StringBuilder();
        try {
            sb.append("Display name: ")
              .append(nif.getDisplayName())
              .append("\n");
            sb.append("Name: ")
              .append(nif.getName())
              .append("\n");
            for (InetAddress inetAddress : list(nif.getInetAddresses())) {
                sb.append("InetAddress: ")
                  .append(inetAddress)
                  .append("\n");
            }
            sb.append("Up? ")
              .append(nif.isUp())
              .append("\n");
            sb.append("Loopback? ")
              .append(nif.isLoopback())
              .append("\n");
            sb.append("PointToPoint? ")
              .append(nif.isPointToPoint())
              .append("\n");
            sb.append("Supports multicast? ")
              .append(nif.supportsMulticast())
              .append("\n");
            sb.append("Virtual? ")
              .append(nif.isVirtual())
              .append("\n");
            sb.append("Hardware address: ")
              .append(Arrays.toString(nif.getHardwareAddress()))
              .append("\n");
            sb.append("MTU: ")
              .append(nif.getMTU())
              .append("\n");
            sb.append("Index: ")
              .append(nif.getIndex())
              .append("\n");
            sb.append("\n");
            return sb.toString();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    /** Prints all the system interface information to the give stream. */
    public static void printSystemConfiguration(PrintStream out) {
        try {
            out.println("*** all system network interface configuration ***");
            for (NetworkInterface nif : list(getNetworkInterfaces())) {
                out.print(interfaceInformation(nif));
            }
            out.println("*** end ***");
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }
}
