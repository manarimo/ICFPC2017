#!/bin/bash

# usage run_ai.sh [commit id for ai] [ai args...]

GIT_COMMIT=$1
shift
AI_DIRECTORY=/var/ai/${GIT_COMMIT}/

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
ROOT_DIR=`dirname $SCRIPT_DIR`

# replace sandstar with latest version
cp $ROOT_DIR/bin/sandstar.rb $AI_DIRECTORY/bin/sandstar

cd ${AI_DIRECTORY}
./punter $@

