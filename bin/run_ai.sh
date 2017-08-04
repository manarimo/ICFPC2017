#!/bin/bash

# usage run_ai.sh [commit id for ai] [ai args...]

GIT_COMMIT=$1
shift
AI_DIRECTORY=/var/ai/${GIT_COMMIT}/

cd ${AI_DIRECTORY}
./punter $@

