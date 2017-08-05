#!/bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
ROOT_DIR=`dirname $SCRIPT_DIR`
cd $ROOT_DIR
for tag in $(git tag) ; do
    echo ${tag}
    echo $(git show-ref -s ${tag})
done
