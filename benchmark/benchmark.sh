#!/bin/bash
script="./cache_test.lua"

for script in './cache_test.lua' './cache_test_search.lua'
do
    for i in 1 2 5 10
    do
        echo "Script: $script, threads: $i, connections: $i"
        wrk -d 10 -t $i -c $i --latency -s $script http://localhost:8082/
    done
done