#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=$PYTHONPATH:../lib $1 ./scripts/pymdDesktop.py
cd -
