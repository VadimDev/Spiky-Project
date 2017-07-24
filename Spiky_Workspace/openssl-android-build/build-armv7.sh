#!/bin/bash

rm -rf openssl-1.0.2k/
tar xzf openssl-1.0.2k.tar.gz
chmod a+x setenv-android-armv7.sh

. ./setenv-android-armv7.sh

cd openssl-1.0.2k/

perl -pi -e 's/install: all install_docs install_sw/install: install_docs install_sw/g' Makefile.org
./config shared no-ssl2 no-ssl3 no-hw  --openssldir=/usr/local/ssl/android19/armv7

make clean
make depend
make all

sudo -E make install CC=$ANDROID_TOOLCHAIN/arm-linux-androideabi-gcc RANLIB=$ANDROID_TOOLCHAIN/arm-linux-androideabi-ranlib

