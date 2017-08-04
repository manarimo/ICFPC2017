#!/bin/bash

set -e

GIT_BRANCH=$1
GIT_COMMIT=$2

echo "Building ${GIT_COMMIT} from ${GIT_BRANCH}"

./install || (bin/run_python.sh araisan "AIのビルドに失敗したのだ。。。（コミット:${GIT_COMMIT},ブランチ:${GIT_BRANCH}）" && exit 1)

echo "AI build done."
bin/run_python.sh araisan "AIのビルドに成功したのだ！（コミット:${GIT_COMMIT},ブランチ:${GIT_BRANCH}）"

AI_DEST=/var/ai/${GIT_COMMIT}/
mkdir -p ${AI_DEST}

rsync -a --exclude='.*' . ${AI_DEST} || (bin/run_python.sh araisan "AIの保存に失敗したのだ。。。（コミット:${GIT_COMMIT},ブランチ:${GIT_BRANCH}）" && exit 1)

ls ${AI_DEST}
echo "AI saved at ${AI_DEST}"
bin/run_python.sh araisan "AIの保存に成功したのだ！（コミット:${GIT_COMMIT},ブランチ:${GIT_BRANCH}）"
