#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=$PYTHONPATH:../lib nosetests --with-coverage --cover-inclusive -v -w "scripts" -w "tests" -e "gi\.*"
cd -
