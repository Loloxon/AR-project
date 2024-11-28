#!/bin/bash

# Step 1: Navigate to the solution directory and compile the C program
echo "Compiling example.c..."
cd solution || { echo "Directory 'solution' not found! Exiting."; exit 1; }
gcc -o example example.c
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful."

# Step 2: Run the compiled C program
echo "Running example.c..."
./example
if [ $? -ne 0 ]; then
    echo "Execution of example failed. Exiting."
    exit 1
fi
echo "example.c finished execution."

# Step 3: Navigate to the visualization directory and run the Python script
echo "Running visualization.py..."
cd ../visualization || { echo "Directory 'visualization' not found! Exiting."; exit 1; }
python3 visualization.py
if [ $? -ne 0 ]; then
    echo "Visualization failed. Exiting."
    exit 1
fi
echo "Visualization completed successfully."
