#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=../../lib:$PYTHONPATH $1 ./../lib/pymd/pymdDesktop.py
cd -
