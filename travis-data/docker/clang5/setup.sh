#!/bin/sh
set -e
set -x


wget -q --no-check-certificate http://releases.llvm.org/5.0.2/${llvmtar}${tarext} \
    && tar xf ${llvmtar}${tarext} \
    && sudo cp -fR ${llvmtar}/* /usr \
    && rm -rf ${llvmtar} \
    && rm ${llvmtar}${tarext}

sudo apt-get --assume-yes install openmpi-bin libopenmpi-dev
