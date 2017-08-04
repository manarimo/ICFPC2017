#!/bin/bash

set -e

GIT_COMMIT=$1
GIT_BRANCH=$2

echo "Building ${GIT_COMMIT} from ${GIT_BRANCH}"

./install || (bin/run_python.sh araisan "AIのビルドに失敗したのだ。。。(コミット: ${GIT_COMMIT}, ブランチ: ${GIT_BRANCH})" && exit 1)

echo "Done"
bin/run_python.sh araisan "AIのビルドに成功したのだ！(コミット: ${GIT_COMMIT}, ブランチ: ${GIT_BRANCH})"
