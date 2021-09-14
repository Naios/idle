#!/bin/bash -e
cd $(dirname "$0")/..

IDLE_PACKAGE_CACHE=${IDLE_PACKAGE_CACHE}
IDLE_PACKAGE_CACHE_MOUNT=/home/cache

SRC_DIR=$(pwd -P)
BUILD_DIR=$SRC_DIR/build/docker

BUILD_TARGET=$1
BUILD_TYPE=Release

mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "- Mounting Source: '$SRC_DIR'"
echo "- Mounting Build: '$BUILD_DIR'"
echo "- Mounting Cache: '$IDLE_PACKAGE_CACHE'"

MSYS_NO_PATHCONV=1 docker run --rm -t --cap-add=SYS_PTRACE \
           -e BUILD_TARGET=$BUILD_TARGET \
           -e BUILD_TYPE=$BUILD_TYPE \
           -e IDLE_PACKAGE_CACHE=$IDLE_PACKAGE_CACHE_MOUNT \
           -v $SRC_DIR:/home/src \
           -v $BUILD_DIR:/home/persistent \
           -v $IDLE_PACKAGE_CACHE:$IDLE_PACKAGE_CACHE_MOUNT idle
