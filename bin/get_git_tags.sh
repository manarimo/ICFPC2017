#!/bin/bash

SCRIPT=$(readlink -f $0)
SCRIPT_DIR=`dirname $SCRIPT`
ROOT_DIR=`dirname $SCRIPT_DIR`
cd $ROOT_DIR
git tag -l | xargs git tag -d
git fetch origin --tags
for tag in $(git tag) ; do
    echo ${tag}
    echo $(git show-ref -s ${tag})
done
