#!/usr/bin/env bash

tools=(
    jaotc
    jar
    jarsigner
    java
    javac
    javadoc
    javap
    jcmd
    jconsole
    jdb
    jdeprscan
    jdeps
    jhsdb
    jimage
    jinfo
    jjs
    jlink
    jmap
    jmod
    jps
    jrunscript
    jshell
    jstack
    jstat
    jstatd
    keytool
    pack200
    rmic
    rmid
    rmiregistry
    serialver
    unpack200
)

manuals=(
    jar
    jarsigner
    java
    javac
    javadoc
    javap
    jcmd
    jconsole
    jdb
    jdeps
    jinfo
    jjs
    jmap
    jps
    jrunscript
    jstack
    jstat
    jstatd
    keytool
    pack200
    rmic
    rmid
    rmiregistry
    serialver
    unpack200
)

#
# Test 1: Ensure that alternatives installs tools and man pages.
#
for b in ${tools[@]}; do
    if [ "/usr/bin/$b" -ef "/usr/lib/jvm/java-11-amazon-corretto.x86_64/bin/$b" ]; then
        echo "$b - OK"
    else
        echo "$b - Error. Not on path."
        exit
    fi
done

for m in ${manuals[@]}; do
    if [ "$(man -w $m)" -ef "/usr/lib/jvm/java-11-amazon-corretto.x86_64/man/man1/$m.1" ]; then
        echo "$m - OK"
    else
        echo "$m - Error. $m not installed."
        exit
    fi
done

#
# Test 2: Remove the RPM and ensure that tools and man pages are removed.
#
yum remove -y java-11-amazon-corretto-headless

for b in ${tools[@]}; do
    if [ ! -f "/usr/bin/$b" ]; then
        echo "$b - OK"
    else
        echo "$b - Error. $b still installed."
        exit
    fi
done

for m in ${manuals[@]}; do
    if [ ! -f "/usr/share/man/man1/$m.1" ]; then
        echo "$m - OK"
    else
        echo "$m - Error. $m still installed."
        exit
    fi
done
