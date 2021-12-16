#!/bin/sh
make clean
make -j8
rm -rf result.csv
rm *.root # 删除root文件