#!/bin/sh
cd ..
rm -rf build/*
cd build
cmake ..
make -j8
./exampleB1
