#! /bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
PY_BASE=`dirname $SCRIPT_DIR`/src/python
REQUIREMENTS=$PY_BASE/requirements.frozen
PY_MODULE=$PY_BASE/$1
shift

pip3 install -r $REQUIREMENTS 1>&2

python3 $PY_MODULE "$@"
