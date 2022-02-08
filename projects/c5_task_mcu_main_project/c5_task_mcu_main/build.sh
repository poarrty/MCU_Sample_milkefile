#!/bin/sh
#此脚本将需要生成的固件都编译拷贝到编译目录（编译目录必须为工程目录下的 /build 目录）
#CI 会将工程目录下的 build/* 加上构建号后推送到 artifactory

if [ 1 -eq $# ]
then
    CI_BUILD_NUM_=$1
else
    CI_BUILD_NUM_="0"
fi

echo "CI Build id $CI_BUILD_NUM_\r\n"

cd ./hal/core/

make clean

echo "\r\nBuild images[c5_task_mcu_main]:\r\n"

make iap CI_BUILD_NUM=$CI_BUILD_NUM_ -j32
