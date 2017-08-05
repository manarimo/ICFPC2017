#!/usr/bin/env bash

set -e
export GOPATH=$(dirname $(readlink -f $0))

cd "${GOPATH}"
go get alpaca
go build
sudo systemctl stop alpaca
cp alpaca ~/.local/bin
sudo systemctl start alpaca
