#!/bin/bash

set -x

UPSTREAM_REMOTE=upstream-$1
VERSION_BRANCH=$2

git config user.email "no-reply@amazon.com"
git config user.name "corretto-github-robot"

git checkout ${VERSION_BRANCH}

# Load the current OpenJDK version
source make/autoconf/version-numbers

BUILD_NUMBER=$(git ls-remote --tags ${UPSTREAM_REMOTE} |grep jdk-${DEFAULT_VERSION_FEATURE}+ | grep -vE "(-ga|{})$" | cut -d+ -f 2 |sort -n |tail -1)

# Load the current Corretto version
CURRENT_VERSION=$(cat version.txt)

if [[ ${CURRENT_VERSION} == ${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.${BUILD_NUMBER}.* ]]; then
    echo "Corretto version is current."
else
    echo "Updating Corretto version"
    NEW_VERSION="${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.${BUILD_NUMBER}.1"
    echo  "${NEW_VERSION}" > version.txt
    git commit -m "Update Corretto version to match upstream: ${NEW_VERSION}" version.txt
    #git push origin ${VERSION_BRANCH}
fi
