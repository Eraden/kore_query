#!/usr/bin/env bash

echo "$(lsb_release -a)"

sudo apt-get --yes install check
sudo apt-get --yes remove clang
sudo apt-get --yes install clang-3.8
sudo apt-get --yes install libyajl-dev
sudo apt-get --yes install libyajl2
sudo apt-get --yes install libpq-dev
sudo apt-get --yes install postgresql-server-dev-9.4
sudo apt-get --yes install kore

root="$PWD"
echo "$(cmake --version)"

mkdir -p build
mkdir -p tmp

if [[ "$(cmake --version | grep -E '[0-9]+.[0-9]+.[0-9]+' | sed 's/[a-z ]//gi')" == "3.6.1" ]]
then
  echo "Valid cmake"
else
  if [[ ! -f ./tmp/cmake-3.6.1 ]]
  then
    cd tmp
    wget https://cmake.org/files/v3.6/cmake-3.6.1.tar.gz
    tar -xvf cmake-3.6.1.tar.gz &> /dev/null
    mv ./tmp/cmake-3.6.1 ./tmp/cmake
    cd cmake
    echo "Configure cmake..."
    ./configure &> /dev/null
    echo "Building cmake..."
    make -j 20 &> /dev/null
    cd ${root}
  fi
  mv ./tmp/cmake-3.6.1 ./tmp/cmake
  cd tmp/cmake
  echo "Installing cmake..."
  sudo make install &> /dev/null
  echo "  done"
  cd ${root}
fi
./tmp/cmake-3.6.1
export PATH="/usr/local/bin:$PATH"
export PATH="/usr/lib/llvm-3.8/bin:$PATH"

version="$(cmake --version | grep -E '[0-9]+.[0-9]+.[0-9]+' | sed 's/[a-z ]//gi')"
echo "cmake version: '$version'"

if [[ "$version" == "3.6.1" ]]
then
  echo "CMake is valid"
else
  echo "Failed to install cmake"
  exit 1
fi
