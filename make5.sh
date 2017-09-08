#! /bin/bash

set -o nounset                              # Treat unset variables as an error

export PATH=/home/boundarydevices/buildroot/output/host/bin:$PATH

mkdir -p target-qt5/
# rm -r target-qt5/*
cd target-qt5

qmake ..

make -j4
