#!/bin/bash

set -e

rm -rf `pwd`/build/*
cd `pwd`/build && cmake .. && make
cd ..
cp -r `pwd`/src/include `pwd`/lib

# 把头文件拷贝到 /usr/include/mymuduo  so库拷贝到 /usr/lib    PATH
if [ ! -d /usr/include/rpc ]; then 
    mkdir /usr/include/rpc
fi

cd `pwd`/src/include

# 拷贝hpp文件
for header in `ls *.hpp`
do
    cp $header /usr/include/rpc
done

cd ..
cd ..
cp `pwd`/lib/librpc.a /usr/lib

ldconfig