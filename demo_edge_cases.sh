#!/bin/bash

# Edge Case Demonstration Script for Paged Memory Simulator
# This script demonstrates key findings from the comprehensive testing

echo "=== Paged Memory Simulator - Edge Case Demonstrations ==="
echo

# Check if executable exists
if [ ! -f "./paged_memory" ]; then
    echo "Compiling program..."
    make
fi

echo "1. NORMAL OPERATION - Basic functionality works correctly"
echo "========================================================"
echo "1024
4

1
NormalJob
1500

3

5" | ./paged_memory

echo -e "\nPress Enter to continue..."
read

echo "2. MEMORY EXHAUSTION - Correctly handles full memory"
echo "===================================================="
echo "100
2

1
FillMemory
200

1
ShouldFail
50

5" | ./paged_memory

echo -e "\nPress Enter to continue..."
read

echo "3. ADDRESS RESOLUTION - Proper bounds checking"
echo "==============================================="
echo "1024
4

1
BoundaryJob
2500

2
1
0

2
1
2499

2
1
2500

5" | ./paged_memory

echo -e "\nPress Enter to continue..."
read

echo "4. BUG DEMONSTRATION - Negative job size accepted (should be rejected)"
echo "===================================================================="
echo "1024
4

1
NegativeJob
-100

3

5" | ./paged_memory

echo -e "\nPress Enter to continue..."
read

echo "5. INTERNAL FRAGMENTATION - Accurate calculation"
echo "================================================"
echo "1024
4

1
FragJob
1500

1
ExactJob
2048

3

5" | ./paged_memory

echo -e "\nPress Enter to continue..."
read

echo "6. JOB REMOVAL - Proper cleanup and memory management"
echo "===================================================="
echo "1024
6

1
KeepJob
1024

1
RemoveJob
2048

3

4
2

3

5" | ./paged_memory

echo -e "\nAll demonstrations completed!"
echo "See TEST_REPORT.md for comprehensive analysis."
