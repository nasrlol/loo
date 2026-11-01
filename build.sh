#!/bin/bash
set -e

date
echo "author: Abdellah El Morabit"
echo "Compiling Loo"
echo

BUILD_DIR=build
SOURCE_DIR=source
INCLUDES="-I${SOURCE_DIR}/loo -I${SOURCE_DIR} -I${SOURCE_DIR}/base -I${SOURCE_DIR}/render -I${SOURCE_DIR}/platform"
CFLAGS="-fno-rtti -fno-exceptions -Wall -Wundef -Werror -fanalyzer -no-pie -Wno-unused-function -Wno-unused-variable -O0 "
LIBS="-pthread "

DEBUG=1

VULKAN=0
WAYLAND=1
X11=0

if [ $WAYLAND -eq 1 ]; then
     CFLAGS+=" -DWAYLAND=1 -DX11=0"
     LIBS+="-lwayland-client"

elif [ $X11 -eq 1 ]; then
    LIBS+=" -lX11"
    CFLAGS+=" -DX11=1 -DWAYLAND=0"
fi

if [ $VULKAN -eq 1 ]; then
    LIBS+=" -lvulkan"
    CFLAGS+=" -DVULKAN=1"
fi

if [ $DEBUG -eq 1 ]; then
	CFLAGS+=" -g -DDEBUG=1"
fi

mkdir -p "$BUILD_DIR"

echo "Building ${BUILD_DIR}/loo..."
g++ $INCLUDES $CFLAGS "${SOURCE_DIR}/loo/loo.cpp" -fuse-ld=mold -o "${BUILD_DIR}/loo" $LIBS
"$(dirname "$0")/reload.sh"
