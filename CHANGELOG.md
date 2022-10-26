# Change Log for Amazon Corretto 11

The following sections describe the changes for each release of Amazon Corretto 11.

## Corretto version: 11.0.17.8.1

Release Date: October 18, 2022

**Target Platforms <sup>1</sup>**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Linux using muslc 1.2.2 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.15 and later, x86_64
+  macOS 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 11.0.17.8.1:

| Issue Name                                        | Platform | Description                                                             | Link                                                                          |
|---------------------------------------------------|----------|-------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-11.0.17+8                              | All      | Update Corretto baseline to OpenJDK 11.0.17+8                           | [jdk-11.0.17+8](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.17+8) |
| Update Timezone Data to 2022e                     | All      | All tzdata updates up to 2022e                                          | [#292](https://github.com/corretto/corretto-11/pull/292) [#293](https://github.com/corretto/corretto-11/pull/293) |
| Add jpeg, alsa and fonts as headless dependencies | Amazon Linux| Add libraries that could be used in headless mode to RPM dependecies | [#287](https://github.com/corretto/corretto-11/pull/287) |
| Update amazon cacerts                             | All      | Update amazon cacerts file from amazonlinux                             |                                                                               |

The following CVEs are addressed in 11.0.17.8.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2022-21626 | 5.3  | security-libs/java.security |
| CVE-2022-21618 | 5.3  | security-libs/org.ietf.jgss |
| CVE-2022-21628 | 5.3  | core-libs/java.net          |
| CVE-2022-39399 | 3.7  | core-libs/java.net          |
| CVE-2022-21619 | 3.7  | security-libs/java.security |
| CVE-2022-21624 | 3.7  | core-libs/javax.naming      |


## Corretto version: 11.0.16.9.1
Release Date: August 15, 2022


**Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.15 and later, x86_64
+ macos 11.0 and later, aarch64

The following issues are addressed in 11.0.16.9.1

| Issue Name                                       | Platform    | Description                                                                          | Link                                                                          |
|--------------------------------------------------|-------------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-11.0.16.1+1                              | All         | Updates Corretto baseline to OpenJDK 11.0.16.1+1                                        | [jdk-11.0.16.1+1](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.16.1%2B1) |
| Resolve C2 compiler crash                              | All         | [JDK-8279219](https://bugs.openjdk.org/browse/JDK-8279219) caused regressions in the OpenJDK 11.0.16 and OpenJDK 17.0.4 releases and we are backing it out. See [JDK-8291665](https://bugs.openjdk.org/browse/JDK-8291665).                                       | [JDK-8292260](https://bugs.openjdk.org/browse/JDK-8292260) |
| Unify Info.plist files with correct version strings | macOS |  | [JDK-8252145](https://bugs.openjdk.org/browse/JDK-8252145)

## Corretto version: 11.0.16.8.3
Release Date: July 28, 2022

**Target Platforms**

+  macOS 10.15 and later, x86_64
+  macOS 11.0 and later, aarch64

The following issues are addressed in 11.0.16.8.3

| Issue Name                                            | Platform | Description                                                                           | Link                                                       |
|-------------------------------------------------------|----------|---------------------------------------------------------------------------------------|------------------------------------------------------------|
| jlink error Hash of java.rmi differs to expected hash | macOS    | jlink mods contained hashes calculated using the older format, no code changes        | [#280](https://github.com/corretto/corretto-11/issues/280) |
| Fix macOS packaging                                   | macOS    | Update macos packaging logic to avoid duplicate paths after installation              | [#277](https://github.com/corretto/corretto-11/pull/277)   |


## Corretto version: 11.0.16.8.2

Release Date: July 25, 2022
 
**Target Platforms**
 
+ Amazon Linux 2022, x86_64
+ Amazon Linux 2022, aarch64
 
The following issues are addressed in 11.0.16.8.2
 
| Issue Name                                       | Platform    | Description                                                     | Link |
|--------------------------------------------------|-------------|-----------------------------------------------------------------|------|
| Add provides java-devel                          | AL2/AL2022  | Adding provides java-devel as required by 3p packages           |      |
 

## Corretto version: 11.0.16.8.1

Release Date: July 19, 2022

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.15 and later, x86_64
+  macOS 11.0 and later, aarch64

The following issues are addressed in 11.0.16.8.1.

| Issue Name                                   | Platform | Description                                                                           | Link                                                                          |
|----------------------------------------------|----------|---------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-11.0.16+8                         | All      | Update Corretto baseline to OpenJDK 11.0.16+8                                         | [jdk-11.0.16+8](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.16+8) |
| Enable bundled zlib library on macOS aarch64 | MacOS    | Updates to use bundled (not the system) version of the zlib library on macOS aarch64  | [#36](https://github.com/corretto/corretto-18/pull/36)                        |
| Update amazon cacerts                        | All      | Update amazon cacerts file from amazonlinux                                           |                                                                               |

The following CVEs are addressed in 11.0.16.8.1

| CVE            | CVSS | Component        |
|----------------|------|------------------|
| CVE-2022-34169 | 7.5  | xml/jaxp         |
| CVE-2022-21541 | 5.9  | hotspot/runtime  |
| CVE-2022-21540 | 5.3  | hotspot/compiler | 

## Corretto version: 11.0.15.9.1

Release Date: April 19, 2022

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64
+  macOS 11.0 and later, aarch64

The following CVEs are addressed in 11.0.15.9.1
 
| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2022-21476 | 7.5 | security-libs/java.security |
| CVE-2022-21496 | 5.3 | core-libs/javax.naming |
| CVE-2022-21434 | 5.3 | core-libs/java.lang |
| CVE-2022-21426 | 5.3 | xml/jaxp |
| CVE-2022-21443 | 3.7 | security-libs/java.security |

## Corretto version: 11.0.14.10.1

Release Date: February 8, 2022

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.14.10.1.

| Issue Name             | Platform | Description | Link |
|------------------------| --- | --- | --- |
| Import jdk-11.0.14.1+1 | All | Update Corretto baseline to OpenJDK 11.0.14.1+1 | [jdk-11.0.14.1+1](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.14.1%2B1)
| Backport JDK-8218546 | All | Unable to connect to https://google.com using java.net.HttpClient | [JDK-8218546](https://bugs.openjdk.java.net/browse/JDK-8218546)

## Corretto version: 11.0.14.9.1

Release Date: January 18, 2022

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.14.9.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.14+9 | All | Update Corretto baseline to OpenJDK 11.0.14+9 | [jdk-11.0.14+9](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.14%2B9)
| Use SHA256 hashes in RPM File Digest | RPM-based Linux | Update plugins to support FIPS compliant file digests in generated RPMs | [#344](https://github.com/corretto/corretto-8/issues/344) |
| Sign Mac bundle | Macos x64 | The Macos tgz contained signed files, but was not signed as a bundle | |
| Update Amazon CACerts | All | Pull in the latest CA Certificates from Amazon Linux | |
| 64 bit .dll on Windows x86 | Windows x86 | 64 bit .dll was used by the Corretto 11 on windows x86 | [corretto-11#238](https://github.com/corretto/corretto-11/issues/238) |

The following CVEs are addressed in 11.0.14.9.1

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2022-21341 | 5.3 | core-libs/java.io:serialization |
| CVE-2022-21365 | 5.3 | client-libs/javax.imageio |
| CVE-2022-21282 | 5.3 | xml/jaxp |
| CVE-2022-21291 | 5.3 | hotspot/runtime |
| CVE-2022-21277 | 5.3 | client-libs/javax.imageio | 	
| CVE-2022-21305 | 5.3 | hotspot/compiler |
| CVE-2022-21299 | 5.3 | xml/jaxp |
| CVE-2022-21296 | 5.3 | xml/jaxp |
| CVE-2022-21283 | 5.3 | core-libs/java.util |
| CVE-2022-21340 | 5.3 | security-libs/java.security |
| CVE-2022-21293 | 5.3 | core-libs/java.lang |
| CVE-2022-21294 | 5.3 | core-libs/java.util |
| CVE-2022-21360 | 5.3 | client-libs/javax.imageio |
| CVE-2022-21248 | 3.7 | core-libs/java.io:serialization |
| CVE-2022-21366 | 5.3 | client-libs/javax.imageio |

## Corretto version: 11.0.13.8.1

Release Date: October 19, 2021

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.13.8.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.13+8 | All | Update Corretto baseline to OpenJDK 11.0.13+8 | [jdk-11.0.13+8](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.13%2B8)
| Add audio-input entitlements | Macos | Package is missing entitlements to properly request access to microphone | N/A |

The following CVEs are addressed in 11.0.13.8.1

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2021-35567 | 6.8 | security-libs/java.security |
| CVE-2021-35550 | 5.9 | security-libs/javax.net.ssl |
| CVE-2021-35586 | 5.3 | client-libs/javax.imageio |
| CVE-2021-35564 | 5.3 | security-libs/java.security |
| CVE-2021-35561 | 5.3 | core-libs/java.util |
| CVE-2021-35565 | 5.3 | core-libs/java.net |
| CVE-2021-35559 | 5.3 | client-libs/javax.swing |
| CVE-2021-35578 | 5.3 | security-libs/javax.net.ssl |
| CVE-2021-35556 | 5.3 | client-libs/javax.swing |
| CVE-2021-35603 | 3.7 | security-libs/javax.net.ssl |
| CVE-2021-35588 | 3.1 | hotspot/runtime |

## Corretto version: 11.0.12.7.2

Release Date: July 20, 2021

**Target Platforms**

+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.12.7.2.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| 8266248: Compilation failure in PLATFORM_API_MacOSX_MidiUtils.c with Xcode 12.5 | MacOS | Compilation failure in PLATFORM_API_MacOSX_MidiUtils.c with Xcode 12.5 | [JDK-8266248](https://bugs.openjdk.java.net/browse/JDK-8266248)

## Corretto version: 11.0.12.7.1

Release Date: July 20, 2021

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.12.7.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.12+6 | All | Update Corretto baseline to OpenJDK 11.0.12+6 | [jdk-11.0.12+6](https://github.com/openjdk/jdk11u/releases/tag/jdk-11.0.12%2B6)
| Add support for GCC 10. | All | Add support for building corretto with gcc 10. | |
| Aarch64 enhancements | aarch64 | 8186670: Implement _onSpinWait() intrinsic for AArch64 <br/>  8258604: Use 'isb' instruction in SpinPause on linux-aarch64 | |
| x86 enhancements | x86 | Fix missing code from 8222074: Enhance auto vectorization for x86 | |
| Update amazon cacerts | All | Update amazon cacerts file from amazonlinux | |
| Serial GC Bugfix effecting Java-11 Runtime in Lambda | All | [8268635: Corrupt oop in ClassLoaderData](https://bugs.openjdk.java.net/browse/JDK-8268635) | |

The following CVEs are addressed in 11.0.12.7.1

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2021-2388 | 7.5 |	hotspot/compiler |
| CVE-2021-2369 | 4.3 | security-libs/java.security |
| CVE-2021-2341 | 3.1 | core-libs/java.net |


## Corretto version: 11.0.11.9.1

Release Date: April 20, 2021

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.11.9.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.11.9.1 | All | Updates Corretto baseline to OpenJDK 11.0.10+9 | [jdk-11.0.11+9](http://hg.openjdk.java.net/jdk-updates/jdk11u/rev/4ba63d63eae5)
| Update Alpine flags to include Shenandoah and VM bug URL | Alpine-based Linux | Updating Corretto Alpine build flags to include Shenandoah garbage collector and to match up with the other Corretto platforms. | [corretto-11#180](https://github.com/corretto/corretto-11/pull/180)
| Remove expired cacerts | All | Removes expired cacerts "thawtepremiumserverca" and "verisigntsaca" | [corretto-11#184](https://github.com/corretto/corretto-11/pull/184)
| Fix C2 compilation failed with assert(!q->is_MergeMem()) | All | Include [JDK-8221592](https://bugs.openjdk.java.net/browse/JDK-8221592) and [JDK-8223581](https://bugs.openjdk.java.net/browse/JDK-8223581) to address C2 compilation failed with assert(!q->is_MergeMem()) | [JDK-8221592](https://bugs.openjdk.java.net/browse/JDK-8221592), [JDK-8223581](https://bugs.openjdk.java.net/browse/JDK-8223581)
| Enable TLS 1.0 and 1.1 | All | Re-enable TLS 1.0 and 1.1 which has been disabled in upstream 11.0.11+9 | |

The following CVEs are addressed in 11.0.11.9.1

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2021-2161  | 5.9 | core-libs/java.io
| CVE-2021-2163  | 5.3 | security-libs/java.security

## Corretto version: 11.0.10.9.1

Release Date: January 19, 2021

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.10.9.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.10+9 | all | Updates Corretto baseline to OpenJDK 11.0.10+9 | [jdk-11.0.10+9](http://hg.openjdk.java.net/jdk-updates/jdk11u/rev/405102e26a62) |
| Change directory structure of Corretto 11 repository | All | Move the contents of the `src` directory up one level. This enables simpler merges from upstream and better history tracking | [Move content of src one level up](https://github.com/corretto/corretto-11/commit/926668219e0a135e48638ba2d4b66230ec7a0195)
| Proper Detection of Graviton 2 CPUs | linux aarch64 | Includes [JDK-8256488](https://bugs.openjdk.java.net/browse/JDK-8256488), [JDK-8233912](https://bugs.openjdk.java.net/browse/JDK-8233912), [JDK-8257436](https://bugs.openjdk.java.net/browse/JDK-8257436), [JDK-8255351](https://bugs.openjdk.java.net/browse/JDK-8255351) | [Add detection for Graviton 2 CPUs](https://bugs.openjdk.java.net/browse/JDK-8255351)
| Support adaptive heuristic for Shenandoah Garbage Collector | all | Includes [JDK-8255984](https://bugs.openjdk.java.net/browse/JDK-8255984) and [JDK-8256912](https://bugs.openjdk.java.net/browse/JDK-8256912) | [Shenandoah: "adaptive" heuristic is prone to missing load spikes](https://bugs.openjdk.java.net/browse/JDK-8255984)

This version addresses a number of security vulnerabilities that do not have an associated CVE.

## Corretto version: 11.0.9.12.1

Release Date: November 4, 2020

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Linux using glibc 2.25 or later, Arm (Preview)
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues are addressed in 11.0.9.12.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-11.0.9.1+1 | all | Updates Corretto baseline to OpenJDK 11.0.9.1+1 | [jdk-11.0.9.1+1](http://hg.openjdk.java.net/jdk-updates/jdk11u/rev/27723943c0dd) |
| 8255603: Memory/Performance regression after JDK-8210985| all | Fixes a problem with the initial size of SSLSessionCache being too big | [JDK-8255603](https://bugs.openjdk.java.net/browse/JDK-8255603) |
|  Move files in root of tar.gz to subfolder | linux | Fixes an issue in some generic linux artifacts where some of the text included files were in the root directory or missing | |
| Use default values for java.version | all | Stop overriding the java.version value (build only) | |
| [mac] Use wildcard to match the JDK version number when renaming the artifacts | mac | Non-permanent fix to support building with a PATCH element in the java version (build only) | |
| [mac] Temporarily update JVMVersion in Info.plist template to include the patch number for 11.0.9.1 | mac | Non-permanent fix to support generating a .pkg with a PATCH element in the java version (build only) | |


## Corretto version: 11.0.9.11.2

Release Date: October 22, 2020

**Target Platforms**

+  Windows 7 or later, x86_64

The following issue is addressed in 11.0.9.11.2.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| jlink error hash of java.rmi differs to expected hash | Windows | The jlink failed to work due to wrong module hashes in java.base.jmod | [corretto-11#144](https://github.com/corretto/corretto-11/issues/144) |

## October 2020 critical patch update: Corretto version 11.0.9.11.1

Release Date: October 20, 2020

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues and enhancements are addressed in 11.0.9.11.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Update Corretto to 11.0.9.11.1.  |  All  |  Update Corretto 11 patch set to 11.0.9.11.1.  |   |
| Enable Shenandoah on build time | All | Corretto builds include the Shenandoah garbage collector |  |
| Add -moutline-atomics for linux aarch64 builds | Linux aarch64 | Improves performance on aarch64 systems |  |
| Need to eliminate excessive i2l conversions | Linux aarch64 | Improves performance on aarch64 systems | [JDK-8248043](https://bugs.openjdk.java.net/browse/JDK-8248043) |
| AARCH64: I2L/L2I conversions can be skipped for masked positive values | Linux aarch64 | Improves performance on aarch64 systems | [JDK-8248870](https://bugs.openjdk.java.net/browse/JDK-8248870) |
| AARCH64: Faster Math.signum(fp) | Linux aarch64 | Improves performance on aarch64 systems | [JDK-8251525](https://bugs.openjdk.java.net/browse/JDK-8251525) |
| ARM64: save some words in itable stub | Linux aarch64 | Improves performance on aarch64 systems | [JDK-8252311](https://bugs.openjdk.java.net/browse/JDK-8252311) |
| C2: Excessive RemoveUseless passes during incremental inlining | All | | [JDK-8059241](https://bugs.openjdk.java.net/browse/JDK-8059241) |
| Remove unnecessary caching of Parker object in java.lang.Thread | All | | [JDK-8222518](https://bugs.openjdk.java.net/browse/JDK-8222518) |
| AArch64: the const STUB_THRESHOLD in macroAssembler_aarch64.cpp needs to be tuned | Linux aarch64 | Optimize the stub thresholds of string_compare intrinsics | [JDK-8229351](https://bugs.openjdk.java.net/browse/JDK-8229351) |
| Improve performance of InflaterOutputStream.write() | All | | [JDK-8242848](https://bugs.openjdk.java.net/browse/JDK-8242848) |
| Set legal directory file permissions. | Linux | Ensures files under the legal folder are marked as read only |
| Fix GCC8 and GCC9 warnings | All | Multiple changes, including [JDK-8215009](https://bugs.openjdk.java.net/browse/JDK-8215009), [JDK-8214854](https://bugs.openjdk.java.net/browse/JDK-8214854), [JDK-8220074](https://bugs.openjdk.java.net/browse/JDK-8220074), [JDK-8219675](https://bugs.openjdk.java.net/browse/JDK-8219675), [JDK-8244653](https://bugs.openjdk.java.net/browse/JDK-8244653), to eliminate newer GCC related warnings | [JDK-8215009](https://bugs.openjdk.java.net/browse/JDK-8215009), [JDK-8214854](https://bugs.openjdk.java.net/browse/JDK-8214854), [JDK-8220074](https://bugs.openjdk.java.net/browse/JDK-8220074), [JDK-8219675](https://bugs.openjdk.java.net/browse/JDK-8219675), [JDK-8244653](https://bugs.openjdk.java.net/browse/JDK-8244653)


The following CVEs are addressed in 11.0.9.11.1

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2020-14803  | 5.3 | core-libs/java.io
| CVE-2020-14792  | 4.2 | hotspot/compiler
| CVE-2020-14782  | 3.7 | security-libs/java.security
| CVE-2020-14797  | 3.7 | core-libs/java.nio
| CVE-2020-14781  | 3.7 | core-libs/javax.naming
| CVE-2020-14779  | 3.7 | core-libs/java.io:serialization
| CVE-2020-14796  | 3.1 | core-libs/java.io
| CVE-2020-14798  | 3.1 | core-libs/java.io


## July 2020 critical patch update: Corretto version 11.0.8.10.1

Release Date: July 14, 2020

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues and enhancements are addressed in 11.0.8.10.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Update Corretto to 11.0.8.10.1.  |  All  |  Update Corretto 11 patch set to 11.0.8.10.1.  |   |
| Add Corretto 11 x86 | Linux | Added Linux x86 32-bit as a supported platform | |
| Add Corretto 11 alpine | Linux | Added Linux Alpine x86_64 as a supported platform | [corretto/corretto-11#99](https://github.com/corretto/corretto-11/pull/99) |
| Backport JDK-8247408 | All | 8247408: IdealGraph bit check expression canonicalization | [JDK-8247408](https://bugs.openjdk.java.net/browse/JDK-8247408)
| Update Amazon linux cacerts	| All | Updates the set of certificates from Amazon Linux bundled with Corretto | |
| Add provides to match openjdk | Linux | Update the provides clause for generic linux rpm and deb files | |
| Remove expired certificates | All | Removed three expired CA certificates present in upstream

The following CVE are addressed in 11.0.8.10.1.

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2020-14583 | 8.3 | core-libs/java.io |
| CVE-2020-14593 | 7.4 | client-libs/2d |
| CVE-2020-14562 | 5.3 | client-libs/javax.imageio |
| CVE-2020-14621 | 5.3 | xml/jaxp |
| CVE-2020-14556 | 4.8 | core-libs/java.util.concurrent |
| CVE-2020-14573 | 3.7 | hotspot/compiler |
| CVE-2020-14581 | 3.7 | client-libs/2d |
| CVE-2020-14577 | 3.7 | security-libs/javax.net.ssl |


## Corretto version: 11.0.7.10.1-1

Release Date: April 22, 2020

**Target Platforms**
+  macOS 10.13 and later, x86_64

The following issues and enhancements are addressed in 11.0.7.10.1-1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
|jlink error Hash of java.rmi differs to expected hash| macOS | java.base.jmod throws hash mismatch error when using jlink | [corretto-11#96](https://github.com/corretto/corretto-11/issues/96) |
|macOS 11.0.7.10.1 tgz is not signed | macOS | Binaries in macOS tgz are not properly signed | |

## April 2020 critical patch update: Corretto version 11.0.7.10.1

Release Date: April 14, 2020

**Target Platforms**
+  RPM-based Linux using glibc 2.12 or later, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64

The following issues and enhancements are addressed in 11.0.7.10.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
|  Update Corretto to 11.0.7.10.1.  |  All  |  Update Corretto 11 patch set to 11.0.7.10.1.  |   |
|  Increase default value of maxInlineLevel  |  All  |  Backport from tip of JDK, performance improvement  |  [JDK-8234863](https://bugs.openjdk.java.net/browse/JDK-8234863)  |
|  Fix missing Amazon cacerts in java.base.jmod  |  All  |  Ensures Corretto runtimes generated with jlink uses the proper cacerts instead of a partial set of certificates  |  [corretto/corretto-11#88](https://github.com/corretto/corretto-11/issues/88)  |
|  Add support for test image on linux  |  Linux  |  Build process generates additional artifacts to support native jtreg tests  |   |
|  Add support for test image on macOS  |  macOS  |  Build process generates additional artifacts to support native jtreg tests  |   |

The following CVE are addressed in 11.0.7.10.1.

| CVE | CVSS | Component |
| --- | --- | --- |
| CVE-2020-2803 | 8.3 | core-libs/java.nio |
| CVE-2020-2805 | 8.3 | core-libs/java.io |
| CVE-2020-2816 | 7.5 | security-libs/javax.net.ssl |
| CVE-2020-2781 | 5.3 | security-libs/java.security |
| CVE-2020-2830 | 5.3 | core-libs/java.util |
| CVE-2020-2767 | 4.8 | security-libs/javax.net.ssl |
| CVE-2020-2800 | 4.8 | core-libs/java.net |
| CVE-2020-2778 | 3.7 | security-libs/javax.net.ssl |
| CVE-2020-2754 | 3.7 | core-libs/javax.script |
| CVE-2020-2755 | 3.7 | core-libs/javax.script |
| CVE-2020-2773 | 3.7 | security-libs/javax.xml.crypto |
| CVE-2020-2756 | 3.7 | core-libs/java.io:serialization |
| CVE-2020-2757 | 3.7 | core-libs/java.io:serialization |


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
