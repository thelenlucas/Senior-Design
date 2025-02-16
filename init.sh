#!/usr/bin/env bash

sudo apt install -y qtcreator qtbase5-dev qt5-qmake cmake cmake build-essential

git submodule init
git submodule update

mkdir build

cp example.db build/

cd build

cmake ..
make