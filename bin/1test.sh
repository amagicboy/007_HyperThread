#!/bin/bash
cd ../build
rm -rf *
rm ../lib/* -rf
cmake ..
make 
cd -
echo pykuse62.com|sudo -S ./main ./test1
