#!/usr/bin/env bash

set -xeuo pipefail

UPSTREAM_REMOTE=upstream-$1
VERSION_BRANCH=$2

git config user.email "no-reply@amazon.com"
git config user.name "corretto-github-robot"

git checkout ${VERSION_BRANCH}

# Load the current OpenJDK version
source make/autoconf/version-numbers

# strip trailing zeroes: https://openjdk.org/jeps/322
ELEMENTS=("${DEFAULT_VERSION_FEATURE}" "${DEFAULT_VERSION_INTERIM}" "${DEFAULT_VERSION_UPDATE}" "${DEFAULT_VERSION_PATCH}")
LAST=${#ELEMENTS[@]}
while (( LAST > 1 )) && [[ "${ELEMENTS[$((LAST-1))]}" == "0" ]]; do
  ((LAST--))
done
VERSION_STR=$(IFS=.; echo "${ELEMENTS[*]:0:$LAST}")

BUILD_NUMBER=$(git ls-remote --tags ${UPSTREAM_REMOTE} |grep "jdk-${VERSION_STR}+" | grep -vE "(-ga|{})$" | cut -d+ -f 2 | sort -n | tail -1)

# Load the current Corretto version
CURRENT_VERSION=$(cat version.txt)

if [[ "${CURRENT_VERSION}" == "${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.${DEFAULT_VERSION_PATCH}.${BUILD_NUMBER}" ]]; then
  echo "Corretto version is current."
else
  echo "Updating Corretto version"
  NEW_VERSION="${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.${DEFAULT_VERSION_PATCH}.${BUILD_NUMBER}"
  echo "${NEW_VERSION}" > version.txt
  git commit -m "Update Corretto version to match upstream: ${NEW_VERSION}" version.txt
fi
