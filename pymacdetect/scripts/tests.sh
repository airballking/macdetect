#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=../../lib:$PYTHONPATH nosetests --with-coverage --cover-package=pymdLib,pymdGui -v -e "gi\.*"
cd -
