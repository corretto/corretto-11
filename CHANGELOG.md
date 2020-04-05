# Change Log for Amazon Corretto 11

The following sections describe the changes for each release of Amazon Corretto 11.

## April 2020 critical patch update: Corretto version 11.0.7.10.1

Release Date: Jan 23, 2020

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.6.10.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto to 11.0.7.10.1.  |  All  |  Update Corretto 11 patch set to 11.0.7.10.1.  |   | 
|  Increase default value of maxInlineLevel  |  All  |  Backport from tip of JDK, performance improvement  |  [JDK-8234863](https://bugs.openjdk.java.net/browse/JDK-8234863)  | 
|  Fix missing Amazon cacerts in java.base.jmod  |  All  |  Ensures Corretto runtimes generated with jlink uses the proper cacerts instead of a partial set of certificates  |  [corretto/corretto-11#88](https://github.com/corretto/corretto-11/issues/88)  | 
|  Add support for test image on linux  |  Linux  |  Build process generates additional artifacts to support native jtreg tests  |   | 
|  Add support for test image on macOS  |  macOS  |  Build process generates additional artifacts to support native jtreg tests  |   | 

## Corretto version: 11.0.6.10.1-2 (macOS)

Release Date: Jan 23, 2020

 The following platforms are updated in this release. 

**Target Platforms**
+  macOS 10.13 and later, x86_64 

The following issue is addressed in 11.0.6.10.1-2.

| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  link fails with 'Invalid JMOD file' on macOS 10.15.2 |  macOS | Java module files in macOS builds were broken in previous release  |  [corretto-11#77](https://github.com/corretto/corretto-11/issues/77)  | 

## Corretto version: 11.0.6.10.1-1 (macOS and Windows)

Release Date: Jan 16, 2020

 The following platforms are updated in this release. 

**Target Platforms**
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issue is addressed in 11.0.6.10.1-1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  SSL handshake error with 11.0.6.10.1(macOS and Windows Platforms)  |  macOS, Windows  |  Fixed bad cacerts file on Windows and macOS that was causing SSL connection errors  |  [corretto-11#73](https://github.com/corretto/corretto-11/issues/73)  | 

## January 2020 critical patch update: Corretto version 11.0.6.10.1

Release Date: Jan 15, 2020

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.6.10.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto to 11.0.6.10.1.  |  All  |  Update Corretto 11 patch set to 11.0.6.10.1.  |   | 
|  Debian package doesn't provide java8-runtime like openjdk-11-jre from debian does.  |  Linux，Debian  |  Provides additional provide in corretto deb  |  [corretto-11#35](https://github.com/corretto/corretto-11/issues/35)  | 
|  Backport JDK-8215396  |  All  |  JTabbedPane preferred size calculation is wrong for SCROLL_TAB_LAYOUT.  |  [corretto-11#63](https://github.com/corretto/corretto-11/issues/63)  | 
|  GPG key used to sign debian repository changed | Linux | GPG key of the Corretto Apt repo was updated during the release  |  [corretto-8#209](https://github.com/corretto/corretto-8/issues/209)  |  

The following CVEs are addressed in 11.0.6.10.1.


| CVE # | Component Affected | 
| --- | --- | 
|  CVE-2020-2604  |  Serialization  | 
|  CVE-2020-2601  |  Security  | 
|  CVE-2020-2655  |  JSSE  | 
|  CVE-2020-2593  |  Networking  | 
|  CVE-2020-2654  |  Libraries  | 
|  CVE-2020-2590  |  Security  | 
|  CVE-2020-2659  |  Networking  | 
|  CVE-2020-2583  |  Serialization  | 

## Corretto version: 11.0.5.10.2

Release Date: Nov 20, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  macOS 10.10 and later, x86_64 

The following issues and enhancements are addressed in 11.0.5.10.2.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Java2D Queue Flusher crash when closing lid and/or switching away from external monitors  |  macOS  |  JVM crashes when closing the lid of the macbook or switching between different monitors. This issue was reproducible in both OpenJDK8 and 11.  |  [corretto-11#46](https://github.com/corretto/corretto-11/issues/46)  | 

## October 2019 critical patch update: Corretto version 11.0.5.10.1

Release Date: Oct 15, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.5.10.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto to 11.0.5.10.1.  |  All  |  Update Corretto 11 patch set to 11.0.5.10.1.  |   | 

The following CVEs are addressed in 11.0.5.10.1.


| CVE # | Component Affected | 
| --- | --- | 
|  CVE-2019-2949  |  security-libs/javax.net.ssl  | 
|  CVE-2019-2989  |  core-libs/java.net  | 
|  CVE-2019-2958  |  core-libs/java.lang  | 
|  CVE-2019-2975  |  core-libs/javax.script  | 
|  CVE-2019-2977  |  hotspot/compiler  | 
|  CVE-2019-2999  |  tools/javadoc  | 
|  CVE-2019-2964  |  core-libs/java.util.regex  | 
|  CVE-2019-2962  |  client-libs/2d  | 
|  CVE-2019-2973  |  xml/jaxp  | 
|  CVE-2019-2978  |  core-libs/java.net  | 
|  CVE-2019-2981  |  xml/jaxp  | 
|  CVE-2019-2983  |  client-libs/2d  | 
|  CVE-2019-2987  |  client-libs/2d  | 
|  CVE-2019-2988  |  client-libs/2d  | 
|  CVE-2019-2992  |  client-libs/2d  | 
|  CVE-2019-2894  |  security-libs/javax.net.ssl  | 
|  CVE-2019-2933  |  core-libs  | 
|  CVE-2019-2945  |  core-libs/java.net  | 

## GA release: 11.0.4.11.1: Amazon Corretto 11

Release Date: Sept 17, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 

The following issues and enhancements are addressed in 11.0.4.11.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto 11.0.4.11.1 RC to 11.0.4.11.1 GA.  |  aarch64  |  Amazon Corretto 11.0.4.11.1 for aarch64 is now GA.  |   | 

## July 2019 critical patch update: Corretto version 11.0.4.11.1

Release Date: Jul 16, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.4.11.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto to 11.0.4.11.1.  |  All  |  Update Corretto 11 patch set to 11.0.4.11.1.  |   | 
|  Include OpenJDK cacerts and Amazon specific cacerts in Corretto11.  |  All  |  Update cacerts in Corretto11 to include both OpenJDK cacerts and Amazon cacerts.  |   | 
|  Debian jinfo file.  |  Linux  |  Provide jinfo file to make update-java-alternatives command works with Corretto11 on Debian.  |  [corretto-11#25](https://github.com/corretto/corretto-11/issues/25)  | 
|  Fix the Windows Installer does not set file association for jar files.  |  Windows  |  Jar files can now be double clicked in Windows to open them like they can be with Oracle's JRE.  |  [corretto-11#21](https://github.com/corretto/corretto-11/issues/21)  | 
|  Fix JAVA_HOME remains on uninstall of MSI package.  |  Windows  |  Fix the issue of the system variable JAVA_HOME remains in the path when uninstalling the MSI package on Windows.  |  [corretto-11#17](https://github.com/corretto/corretto-11/issues/17)  | 

The following CVEs are addressed in 11.0.4.11.1.


| CVE # | Component Affected | 
| --- | --- | 
|  CVE-2019-7317  |  AWT (libpng)  | 
|  CVE-2019-2821  |  JSSE  | 
|  CVE-2019-2766  |  Networking  | 
|  CVE-2019-2816  |  Networking  | 
|  CVE-2019-2745  |  Security  | 
|  CVE-2019-2786  |  Security  | 
|  CVE-2019-2818  |  Security  | 
|  CVE-2019-2762  |  Utilities  | 
|  CVE-2019-2769  |  Utilities  | 

## Corretto aarch64 preview: version 11.0.3.7.1

Release Date: June 28, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.17 or later, aarch64 
+  Debian-based Linux using glibc 2.17 or later, aarch64 

The following issues and enhancements are addressed in 11.0.3.7.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Corretto 11 aarch64 preview  |  Linux aarch64  |  add Corretto 11 builds for aarch64.  |   | 

## April 2019 critical patch update: Corretto version 11.0.3.7.1

Release Date: Apr 16, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.3.7.1.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Update Corretto to 11.0.3.7.1.  |  All  |  Update Corretto 11 patch set to 11.0.3.7.1.  |   | 

## 11.0.2.9.3: Amazon Corretto 11

Release Date: Mar 19, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.2.9.3.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  MacOS package installer shows as Corretto 8 instead of Corretto 11.  |  macOS  |  The message in the introduction session of the mac installer is incorrect.  |  [corretto-11#10](https://github.com/corretto/corretto-11/issues/10)  | 
|  MacOS package installer unable to detect Corretto as a JDK package after installation.  |  macOS  |  After installing Corretto-11.0.2.3, /usr/libexec/java_home is unable to detect it as a JDK package.  |  [corretto-11#12](https://github.com/corretto/corretto-11/issues/12)  | 

## GA release: 11.0.2.9.3: Amazon Corretto 11

Release Date: Mar 14, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

The following issues and enhancements are addressed in 11.0.2.9.3.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Bump up version string to 11.0.2.9.3 for GA release.  |  All  |  Corretto 11 GA release.  |   | 

## 11.0.2.9.2: Amazon Corretto 11

Release Date: Mar 4, 2019

 The following platforms are updated in this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 

Added LTS as version-opt to the version string.

The following issues and enhancements are addressed in 11.0.2.9.2.


| Issue Name | Platform | Description | Link | 
| --- | --- | --- | --- | 
|  Added LTS as version-opt to the version string.  |  RPM-based Linux，Debian-based Linux  |  Include LTS in the Java version string.  |   | 

## Initial release: Version Corretto-11.0.2.9.1

Release Date: Feb 12, 2019

 The following platforms are compatible with this release. 

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64 
+  Debian-based Linux using glibc 2.12 or later, x86_64 
+  Windows 7 or later, x86_64 
+  macOS 10.13 and later, x86_64 

 The following are the changes for this release. 


| Patch | Description | Release Date | 
| --- | --- | --- | 
|   Back port from OpenJDK 12, fixing JDK-8202353: "os::readdir should use readdir instead of readdir_r".   |   Enables compilation on Amazon Linux 2, as readdir_r has been deprecated in glibc >= 2.24. Additionally, this also fixes JDK-8202835: "jfr/event/os/TestSystemProcess.java fails on missing events".   |   2019-02-12   | 
|   Back port from OpenJDK 12, fixing JDK-8202794: "Native Unix code should use readdir rather than readdir_r".   |   Enables compilation on Amazon Linux 2, as readdir_r has been deprecated in glibc >= 2.24.   |   2019-02-12   | 
|   Back port from OpenJDK 12, fixing JDK-8207340: "UnixNativeDispatcher close and readdir usages should be fixed".   |   Enables compilation on Amazon Linux 2, as readdir_r has been deprecated in glibc >= 2.24.   |   2019-02-12   | 
|   Back port from OpenJDK 12, fixing JDK-8207748: "Fix for 8202794 breaks tier1 builds".   |   Enables compilation on Amazon Linux 2, as readdir_r has been deprecated in glibc >= 2.24.   |   2019-02-12   | 
