#!/bin/bash
set -e

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

STATUS=0
OUTPUT=$( { time g++ $INCLUDES $CFLAGS "${SOURCE_DIR}/loo/loo.cpp" -fuse-ld=mold -o "${BUILD_DIR}/loo" $LIBS; } 2>&1 ) || STATUS=$?

if [ $STATUS -eq 0 ]; then
    echo -e "==== BUILD SUCCESS ====\n"
else
    echo -e "==== BUILD FAILED ====\n"
fi

date
echo "author: Abdellah El Morabit"
echo "Compiling Loo"
echo
echo "Building ${BUILD_DIR}/loo..."
echo "$OUTPUT"

[ $STATUS -eq 0 ] || exit $STATUS
"$(dirname "$0")/reload.sh"
