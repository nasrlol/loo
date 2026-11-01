
#!/bin/bash
set -e

date
echo "author: Abdellah El Morabit"
echo "Reloading Loo"
echo

BUILD_DIR=build
SOURCE_DIR=source
INCLUDES="-I${SOURCE_DIR}/loo -I${SOURCE_DIR} -I${SOURCE_DIR}/base -I${SOURCE_DIR}/render"
CFLAGS="-fno-rtti -fno-exceptions -Wall -Wundef -Werror -fanalyzer -no-pie -Wno-unused-function -Wno-unused-variable -O0 "

SOFLAGS="-shared -fPIC"

DEBUG=1

if [ $DEBUG -eq 1 ]; then
	CFLAGS+=" -g -DDEBUG=1"
fi

# NOTE: the game is built without -DWAYLAND / -DX11, it never sees a window backend

mkdir -p "$BUILD_DIR"

# NOTE: loo skips reloading while this exists, so it never loads a half written state.so
LOCK_FILE="${BUILD_DIR}/state.lock"
touch "$LOCK_FILE"
trap 'rm -f "$LOCK_FILE"' EXIT

echo "Building ${BUILD_DIR}/state.so..."
time g++ $INCLUDES $CFLAGS "${SOURCE_DIR}/loo/loo_state.cpp" $SOFLAGS -fuse-ld=mold -o "${BUILD_DIR}/loo_state.so"
