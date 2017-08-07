#! /bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
PY_BASE=`dirname $SCRIPT_DIR`/src/python
REQUIREMENTS=$PY_BASE/requirements.frozen
PY_MODULE=$PY_BASE/jaguar

PYTHONPATH=$PY_BASE python3 $PY_MODULE
