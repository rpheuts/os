#!/bin/bash

BINUTILS="binutils-2.24"
GCC="gcc-4.8.2"
GMP="gmp-5.1.3"
MPFR="mpfr-3.1.2"
MPC="mpc-1.0.1"
LIBICONV="libiconv-1.14"

rm -rf gcc binutils
mkdir gcc binutils


if [[ ! -f /usr/local/bin/gcc-4.8 ]]
then
	echo "[ERROR]: Please install GCC 4.8.*"
	exit 1
fi

export CC=/usr/local/bin/gcc-4.8
export CXX=/usr/local/bin/g++-4.8
export CPP=/usr/local/bin/cpp-4.8
export LD=/usr/local/bin/gcc-4.8
export CFLAGS="${CFLAGS} -Wno-error=deprecated-declarations"

export PREFIX="$HOME/opt/cross"
export TARGET=i586-elf
export PATH="$PREFIX/bin:$PATH"

pushd binutils
curl http://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.gz -o ${BINUTILS}.tar.gz

if [[ ! -f "${BINUTILS}.tar.gz" ]]
then
	echo "[ERROR]: Could not download binutils"
	exit 1
fi

tar -xjf ${BINUTILS}.tar.gz
mkdir build-binutils
pushd build-binutils
../${BINUTILS}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls
#make
#make install
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

tar -xjf ${GCC}.tar.gz
tar -xzf ${GMP}.tar.gz
tar -xjf ${MPFR}.tar.gz
tar -xjf ${MPC}.tar.gz
tar -xjf ${LIBICONV}.tar.gz

mv ${GMP} ${GCC}/gmp
mv ${MPFR} ${GCC}/mpfr
mv ${MPC} ${GCC}gcc/
mv ${LIBICONV} ${GCC}/libiconv

mkdir build-gcc
pushd build-gcc
../${GCC}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
