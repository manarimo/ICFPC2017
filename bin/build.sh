#!/bin/bash

GIT_COMMIT=$1
GIT_BRANCH=$2
GIT_PREVIOUS_SUCCESSFUL_COMMIT=$3

echo "Building ${GIT_COMMIT} from ${GIT_BRANCH}"
echo "Last build on this branch: ${GIT_PREVIOUS_SUCCESSFUL_COMMIT}"