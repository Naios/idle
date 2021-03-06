#!/bin/bash -e
cd $(dirname "$0")/..

IDLE_PACKAGE_CACHE=${IDLE_PACKAGE_CACHE}
IDLE_PACKAGE_CACHE_MOUNT=/home/cache

BUILD_TARGET=$1
BUILD_TYPE=${2:-Release}
CC=${3:-clang-12}
CXX=${4:-clang++-12}

# IDLE_WITH_ASAN=
# IDLE_WITH_TSAN=
# IDLE_WITH_MSAN=
# IDLE_WITH_UBSAN=

SRC_DIR=$(pwd -P)
BUILD_DIR=$SRC_DIR/build/docker/cc-$CC-cxx-$CXX

mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "- Mounting Source: '$SRC_DIR'"
echo "- Mounting Build: '$BUILD_DIR'"
echo "- Mounting Cache: '$IDLE_PACKAGE_CACHE'"
echo "- BUILD_TARGET: $BUILD_TARGET"
echo "- BUILD_TYPE: $BUILD_TYPE"
echo "- CC: $CC"
echo "- CXX: $CXX"
echo "- IDLE_WITH_ASAN: $IDLE_WITH_ASAN"
echo "- IDLE_WITH_TSAN: $IDLE_WITH_TSAN"
echo "- IDLE_WITH_MSAN: $IDLE_WITH_MSAN"
echo "- IDLE_WITH_UBSAN: $IDLE_WITH_UBSAN"

MSYS_NO_PATHCONV=1 docker run --rm -t --cap-add=SYS_PTRACE \
           -e BUILD_TARGET=$BUILD_TARGET \
           -e BUILD_TYPE=$BUILD_TYPE \
           -e CC=$CC \
           -e CXX=$CXX \
           -e IDLE_PACKAGE_CACHE=$IDLE_PACKAGE_CACHE_MOUNT \
           -e IDLE_WITH_ASAN=$IDLE_WITH_ASAN \
           -e IDLE_WITH_TSAN=$IDLE_WITH_TSAN \
           -e IDLE_WITH_MSAN=$IDLE_WITH_MSAN \
           -e IDLE_WITH_UBSAN=$IDLE_WITH_UBSAN \
           -v $SRC_DIR:/home/src \
           -v $BUILD_DIR:/home/persistent \
           -v $IDLE_PACKAGE_CACHE:$IDLE_PACKAGE_CACHE_MOUNT idle
