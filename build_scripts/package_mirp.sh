#!/bin/bash

set -eu

MYDIR="$(cd "$(dirname "$0")" && pwd)"
MIRP_DIR="$(dirname ${MYDIR})"

MIRP_VER=`cat ${MIRP_DIR}/VERSION`

ARCH=$1

CC=gcc
CXX=g++

CURDIR="$(pwd)"

FULL_DEPS_DIR="${DEPS_DIR}/mirp_deps_${ARCH}"
PREFIX="${CURDIR}/mirp_v${MIRP_VER}_${ARCH}"

# Copy the dependencies to the install prefix
cp -R ${FULL_DEPS_DIR} ${PREFIX}

BUILD_DIR="$(mktemp -d -p /tmp)"
cd "${BUILD_DIR}"
cmake -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} \
      -DCMAKE_BUILD_TYPE=Release \
      -DMIRP_OPENMP=True \
      -DMIRP_STATIC=True \
      -DCMAKE_PREFIX_PATH=${PREFIX} \
      -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
      ${MIRP_DIR}

make
#ctest
make install

cd "${CURDIR}"
rm -Rf "${BUILD_DIR}"

# Fix the rpaths (if we have patchelf)
if [[ $(command -v patchelf 2>&1) ]]
then
    for I in ${PREFIX}/bin/*
    do
        if [[ ! -L "$I" ]]
        then 
            RP1=`patchelf --print-rpath "$I"`
            patchelf --set-rpath '$ORIGIN' "$I"
            RP2=`patchelf --print-rpath "$I"`
            echo "${I}: RPATH changed from \"${RP1}\" to \"${RP2}\""
        fi
    done
    for I in ${PREFIX}/bin/*
    do
        if [[ ! -L "$I" ]]
        then 
            RP1=`patchelf --print-rpath "$I"`
            patchelf --set-rpath '$ORIGIN/../lib64' "$I"
            RP2=`patchelf --print-rpath "$I"`
            echo "${I}: RPATH changed from \"${RP1}\" to \"${RP2}\""
        fi
    done
else
    echo
    echo "!!! Patchelf not installed. Skipping fixing RPATHS !!!"
    echo
fi

mv ${PREFIX}/README ${PREFIX}/README.dependencies

# Create the readme file
GCC_VER=$(gcc --version | head -n 1 | cut -d" " -f 3)
BUILD_DATE=$(date -I)
cp "${MIRP_DIR}/LICENSE"               "${PREFIX}"
cp "${MYDIR}/mirp_README.in"           "${PREFIX}/README"
sed -i "s/MIRP_VER/${MIRP_VER}/g"      "${PREFIX}/README"
sed -i "s/GCC_VER/${GCC_VER}/g"        "${PREFIX}/README"
sed -i "s/ARCH/${ARCH}/g"              "${PREFIX}/README"
sed -i "s/BUILD_DATE/${BUILD_DATE}/g"  "${PREFIX}/README"