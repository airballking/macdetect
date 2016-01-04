#!/bin/bash

CWDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${CWDIR}
PYTHONPATH=../../lib:$PYTHONPATH ./../lib/pymd/pymdDesktop.py $1
cd -
