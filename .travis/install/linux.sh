#!/bin/bash
sudo apt-get install make pkg-config g++-4.9 libssl-dev
export CXX="g++-4.9" CC="gcc-4.9"
wget --no-check-certificate http://cmake.org/files/v3.1/cmake-3.1.3-Linux-x86_64.tar.gz && tar -xzf cmake-3.1.3-Linux-x86_64.tar.gz && sudo cp -fR cmake-3.1.3-Linux-x86_64/* /usr
cmake --version
sudo apt-get install qt55-meta-full
source /opt/qt55/bin/qt55-env.sh
git clone git://anongit.kde.org/qca
cd qca
cmake . -DBUILD_TESTS=OFF
make
sudo make install
cd ..