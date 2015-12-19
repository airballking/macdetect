#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=$PYTHONPATH:../../lib nosetests --with-coverage --cover-package=pymdLib,pymdGui -v -e "gi\.*"
cd -
