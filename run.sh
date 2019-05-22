#!/bin/sh
rm -f output.txt

echo $'run\tnodes\tthroughput' >> output.txt
for run in 1 2 3 4 5 6 7 8 9 10; do
    for nodes in 1 2 3 4 5 6 7 8 9 10 15 20 25 30 35 40 45 50; do
        cmake-build-debug/DcfSimulator $nodes 10 $run >> output.txt
    done
done