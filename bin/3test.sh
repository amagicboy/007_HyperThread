#!/bin/bash
cd ../build
rm -rf *
rm ../lib/* -rf
cmake ..
make 
cd -
echo pykuse62.com|sudo -S ./main  /data/cqy/memtier_benchmark/memtier_benchmark/memtier_benchmark  -s 10.16.0.62 -p 11212 --threads=4 --clients=8 --data-size-range=32-512 --data-size-pattern=R --test-time=10 --ratio=1:10 --protocol=memcache_text --multi-key-get=100 --randomize --hide-histogram --key-minimum=200 --key-maximum=400 --key-pattern=G:G --key-stddev=10 --key-median=300
