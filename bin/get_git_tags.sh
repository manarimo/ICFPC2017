#!/bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
ROOT_DIR=`dirname $SCRIPT_DIR`
cd $ROOT_DIR
git tag -l | xargs git tag -d > /dev/null
git fetch origin --tags 2> /dev/null
for tag in $(git tag) ; do
    echo ${tag}
    echo $(git show-ref -s ${tag})
done
