#!/bin/bash

BINUTILS="binutils-2.24"
GCC="gcc-4.8.2"
GMP="gmp-5.1.3"
MPFR="mpfr-3.1.2"
MPC="mpc-1.0.1"
LIBICONV="libiconv-1.14"

rm -rf gcc binutils
mkdir gcc binutils


if [[ ! -f /usr/local/bin/gcc ]]
then
	echo "[ERROR]: Please install GCC.*"
	exit 1
fi

export CC=/usr/local/bin/gcc
export CXX=/usr/local/bin/g++
export CPP=/usr/local/bin/cpp
export LD=/usr/local/bin/gcc
export CFLAGS="${CFLAGS} -Wno-error=deprecated-declarations -Wno-error"

export PREFIX="/opt/cross"
export TARGET=x86_64-pc-elf
export PATH="$PREFIX/bin:$PATH"

pushd binutils
curl http://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.gz -o ${BINUTILS}.tar.gz

if [[ ! -f "${BINUTILS}.tar.gz" ]]
then
	echo "[ERROR]: Could not download binutils"
	exit 1
fi

tar -xzf ${BINUTILS}.tar.gz
mkdir build-binutils
pushd build-binutils
../${BINUTILS}/configure -v --target=$TARGET --prefix="$PREFIX" --disable-nls
make
make install
popd
popd

pushd gcc
curl http://gcc.parentingamerica.com/releases/${GCC}/${GCC}.tar.gz -o ${GCC}.tar.gz

if [[ ! -f ${GCC}.tar.gz ]]
then
        echo "[ERROR]: Could not download GCC"
        exit 1
fi

curl https://ftp.gnu.org/gnu/gmp/${GMP}.tar.bz2 -o ${GMP}.tar.bz2
curl http://www.mpfr.org/mpfr-current/${MPFR}.tar.gz -o ${MPFR}.tar.gz
curl http://multiprecision.org/mpc/download/${MPC}.tar.gz -o ${MPC}.tar.gz
curl http://ftp.gnu.org/pub/gnu/libiconv/${LIBICONV}.tar.gz -o ${LIBICONV}.tar.gz

tar -xzf ${GCC}.tar.gz
tar -xjf ${GMP}.tar.bz2
tar -xzf ${MPFR}.tar.gz
tar -xzf ${MPC}.tar.gz
tar -xzf ${LIBICONV}.tar.gz

mv ${GMP} ${GCC}/gmp
mv ${MPFR} ${GCC}/mpfr
mv ${MPC} ${GCC}/mpc
mv ${LIBICONV} ${GCC}/libiconv

mkdir build-gcc
pushd build-gcc
../${GCC}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
