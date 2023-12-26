#!/bin/bash

#sudo LD_PRELOAD=./lib/libnumap.so  ../mem-ovd-test/kmeans

#if fails, then change to another .so file
sudo LD_PRELOAD=/data/cqy/1010/1018/profilingtool-cqy/lib/libnumap.so  ../mem-ovd-test/kmeans
