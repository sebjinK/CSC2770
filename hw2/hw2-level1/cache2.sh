#!/bin/bash

g++ -fopenmp run3.cpp -o a3

OUTPUT_FILE="run1.txt"
echo "Number of Threads | Time Taken to Run (in Seconds)"
echo "------------------------------------------"
echo "Number of Threads","Time Taken to Run (in Seconds)" >> $OUTPUT_FILE
for((i=1;i<=8;i++)); do
    timeTaken=$(./a3 $i | grep "Time:" | awk '{print $2}')
    echo "$i                 | $timeTaken" # for debug
    echo $i,$timeTaken >> $OUTPUT_FILE
done
echo ""
echo "" >> $OUTPUT_FILE