#!/bin/bash

GIT_COMMIT=$2
GIT_BRANCH=$1

echo "Building ${GIT_COMMIT} from ${GIT_BRANCH}"

./install || (bin/run_python.sh araisan "AIのビルドに失敗したのだ。。。(コミット: ${GIT_COMMIT}, ブランチ: ${GIT_BRANCH})" && exit 1)

echo "Done"
bin/run_python.sh araisan "AIのビルドに成功したのだ！(コミット: ${GIT_COMMIT}, ブランチ: ${GIT_BRANCH})"
