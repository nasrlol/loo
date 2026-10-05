#!/bin/bash
set -e

echo
date
echo "author: Abdellah El Morabit"
echo

BUILD_DIR=build
SOURCE_DIR=source
INCLUDES="-I${SOURCE_DIR}/loo -I${SOURCE_DIR} -I${SOURCE_DIR}/base"
CFLAGS="-g -fno-rtti -fno-exceptions -Wall -Werror -fanalyzer -no-pie -Wno-unused-function -Wno-unused-variable -O0"
LIBS=""

VULKAN=1
WAYLAND=1
X11=0

if [ $WAYLAND -eq 1 ]; then
    LIBS+=" $(pkg-config --libs wayland-client)"
    CFLAGS+=" $(pkg-config --cflags wayland-client) -DWAYLAND=1 -DX11=0"
elif [ $X11 -eq 1 ]; then
    LIBS+=" -lX11"
    CFLAGS+=" -DX11=1 -DWAYLAND=0"
fi

if [ $VULKAN -eq 1 ]; then
    LIBS+=" -lvulkan"
    CFLAGS+=" -DVULKAN=1"
fi

mkdir -p "$BUILD_DIR"

echo "Building ${BUILD_DIR}/loo..."
g++ $INCLUDES $CFLAGS "${SOURCE_DIR}/loo/loo.cpp" -o "${BUILD_DIR}/loo" $LIBS