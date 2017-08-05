#!/usr/bin/env bash

set -e
export GOPATH=$(dirname $(readlink -f $0))

cd "${GOPATH}"
if [ -f ./alpaca.pid ]; then
  kill -9 $(cat ./alpaca.pid) || true
  rm ./alpaca.pid
fi
go build

nohup ./alpaca > /dev/null 2>&1 &
echo $! > ./alpaca.pid