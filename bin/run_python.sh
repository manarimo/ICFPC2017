#! /bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
PY_BASE=`dirname $SCRIPT_DIR`/src/python
REQUIREMENTS=$PY_BASE/requirements.frozen
PY_MODULE=$PY_BASE/$1
shift

pip install -r $REQUIREMENTS

python3 $PY_MODULE $@
