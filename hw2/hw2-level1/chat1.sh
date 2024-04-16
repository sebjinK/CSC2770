#!/bin/bash

# Compile the C++ code
g++ run1.cpp

# Output file
OUTPUT_FILE="run1.txt"

# Run the code for different array sizes
for ((SIZE=20000; SIZE<=40000; SIZE+=1000)); do
    total_time=0
    for ((i=1; i<=20; i++)); do
        # Run the C++ code and capture the time taken
        time_taken=$(./a $SIZE | grep "Time:" | awk '{print $2}')
        total_time=$(echo "$total_time + $time_taken" | bc)
    done
    # Calculate the average time
    avg_time=$(echo "scale=5; $total_time / 20" | bc)
    # Output array size and average time to the file
    echo "$SIZE $avg_time"
    echo "$SIZE,$avg_time" >> $OUTPUT_FILE
done
run1.txt > run1.xlsx