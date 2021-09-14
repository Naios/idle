#!/bin/bash -e
cd $(dirname "$0")/..

IDLE_PACKAGE_CACHE=${IDLE_PACKAGE_CACHE}
IDLE_PACKAGE_CACHE_MOUNT=/home/cache

SRC_DIR=$(pwd -P)
BUILD_DIR=$SRC_DIR/build/docker

mkdir -p $BUILD_DIR/bin
cd $BUILD_DIR

echo "Mounting Source: '$SRC_DIR'"
echo "Mounting Build: '$BUILD_DIR'"
echo "Mounting Cache: '$IDLE_PACKAGE_CACHE'"


if [ "$#" -gt 0 ]
  then
	IDLE_EXEC_COMMAND="-c '$@'"
fi

MSYS_NO_PATHCONV=1 docker run --rm -it --cap-add=SYS_PTRACE \
           -e IDLE_PACKAGE_CACHE=$IDLE_PACKAGE_CACHE_MOUNT \
           -v $SRC_DIR:/home/src \
           -v $BUILD_DIR:/home/persistent \
           -v $IDLE_PACKAGE_CACHE:$IDLE_PACKAGE_CACHE_MOUNT idle fish -C 'cd /home/persistent/build/bin/' $IDLE_EXEC_COMMAND
