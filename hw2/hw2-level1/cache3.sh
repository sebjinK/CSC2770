#!/bin/bash

g++ -fopenmp run4.cpp -o a4

OUTPUT_FILE="run1.txt"
echo "Spacing Size | Time Taken to Run (in Seconds)"
echo "---------------------------------------------"
echo "Spacing Size","Time Taken to Run (in Seconds)" >> $OUTPUT_FILE
for((i=1;i<=20;i++)); do
    timeTaken=$(./a4 2 $i | grep "Time:" | awk '{print $2}')
    echo "$i            | $timeTaken" 
    echo $i,$timeTaken >> $OUTPUT_FILE
done
echo ""
echo "" >> $OUTPUT_FILE