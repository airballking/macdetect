#!/bin/bash

VERSION=$1
COMMENT=$2

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# For good measure (and maybe we want to do additional operations
# later on), go into the base directory of the repository first.
cd ${CWDIR}/..

git tag -a "$1" -m "\"$2\""
git push --tags

cd -
