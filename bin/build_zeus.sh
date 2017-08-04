#!/bin/bash

set -e

echo "Building zeus (local simulator)"
cd zeus
ant
rsync -a --exclude='.*' . /var/icfpc/zeus/


