#!/usr/bin/env bash

set -e
export GOPATH=$(dirname $(readlink -f $0))

cd "${GOPATH}"
go build
sudo systemctl restart alpaca
