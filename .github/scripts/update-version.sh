#!/usr/bin/env bash

set -xeuo pipefail

UPSTREAM_REMOTE=${1:?usage: $0 <upstream-remote-url>}

# Load the current OpenJDK version
source make/autoconf/version-numbers

# version.txt format: FEATURE.INTERIM.UPDATE.PATCH.BUILD
NEW_PREFIX="${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.${DEFAULT_VERSION_PATCH}"

# Upstream tags drop trailing zero elements: https://openjdk.org/jeps/322
VERSION_STR=${NEW_PREFIX}
while [[ "${VERSION_STR}" == *.0 ]]; do
  VERSION_STR=${VERSION_STR%.0}
done

BUILD_NUMBER=$(git ls-remote --tags "${UPSTREAM_REMOTE}" "refs/tags/jdk-${VERSION_STR}+*" \
  | awk -F+ '$NF ~ /^[0-9]+$/ { print $NF }' | sort -n | tail -1)
if [[ -z "${BUILD_NUMBER}" ]]; then
  # Expected right after upstream bumps version-numbers.conf, before it pushes jdk-<version>+0.
  echo "::warning::No jdk-${VERSION_STR}+<build> tag found on ${UPSTREAM_REMOTE}; using build number 0"
  BUILD_NUMBER=0
fi
NEW_VERSION="${NEW_PREFIX}.${BUILD_NUMBER}"

# Load the current Corretto version
CURRENT_VERSION=$(tr -d '[:space:]' < version.txt)
if ! [[ "${CURRENT_VERSION}" =~ ^([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\.([0-9]+)$ ]]; then
  echo "::error::version.txt contains '${CURRENT_VERSION}', expected FEATURE.INTERIM.UPDATE.PATCH.BUILD"
  exit 1
fi
CURRENT_PREFIX=${BASH_REMATCH[1]}
CURRENT_BUILD_NUMBER=${BASH_REMATCH[2]}

if [[ "${CURRENT_VERSION}" == "${NEW_VERSION}" ]]; then
  echo "Corretto version is current."
  exit 0
fi

# Within one FEATURE.INTERIM.UPDATE.PATCH line the build number must not go backwards.
# A changed prefix is a new version line, so the check is skipped.
if [[ "${CURRENT_PREFIX}" == "${NEW_PREFIX}" ]] && (( BUILD_NUMBER < CURRENT_BUILD_NUMBER )); then
  echo "::warning::Upstream build ${BUILD_NUMBER} is lower than ${CURRENT_VERSION} in version.txt; leaving it unchanged"
  exit 0
fi

echo "Updating Corretto version to ${NEW_VERSION}"
echo "${NEW_VERSION}" > version.txt
git commit -m "Update Corretto version to match upstream: ${NEW_VERSION}" version.txt
