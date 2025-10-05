#!/bin/bash

# Manual Test Script for Paged Memory Simulator
# This script runs specific edge case tests manually

echo "=== Paged Memory Simulator - Manual Edge Case Testing ==="
echo

# Check if executable exists
if [ ! -f "./paged_memory" ]; then
    echo "Error: paged_memory executable not found. Compiling..."
    make
    if [ $? -ne 0 ]; then
        echo "Compilation failed!"
        exit 1
    fi
fi

echo "Executable found. Starting tests..."
echo

# Test 1: Minimal configuration
echo "=== Test 1: Minimal Configuration (1 frame, 1024 bytes) ==="
echo "1024
1

1
MinimalJob
1

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 2: Memory exhaustion
echo "=== Test 2: Memory Exhaustion ==="
echo "100
2

1
FillMemory
200

1
ShouldFail
50

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 3: Zero and negative values
echo "=== Test 3: Zero and Negative Values ==="
echo "1024
4

1
ZeroJob
0

1
NegativeJob
-100

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 4: Address resolution boundaries
echo "=== Test 4: Address Resolution Boundaries ==="
echo "1024
4

1
BoundaryJob
1024

2
1
0

2
1
1023

2
1
1024

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 5: Large system
echo "=== Test 5: Large System (64KB pages, 16 frames) ==="
echo "65536
16

1
LargeJob
1000000

3

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 6: Fragmentation
echo "=== Test 6: Internal Fragmentation ==="
echo "1024
8

1
FragJob1
1500

1
FragJob2
2048

3

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 7: Multiple jobs
echo "=== Test 7: Multiple Jobs ==="
echo "1024
8

1
Job1
500
1
Job2
1500
1
Job3
800
1
Job4
2000

3

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 8: Job removal
echo "=== Test 8: Job Removal ==="
echo "1024
6

1
KeepJob
1000
1
RemoveJob
2000
1
AnotherJob
1500

3

4
2

3

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 9: Invalid inputs
echo "=== Test 9: Invalid Inputs ==="
echo "1024
4

999
1
Job With Spaces
1000
1
Special@#Job
500

5" | ./paged_memory

echo -e "\nPress Enter to continue to next test..."
read

# Test 10: Page boundaries
echo "=== Test 10: Page Boundaries ==="
echo "512
4

1
ExactPage
512
1
CrossPage
513
1
MultiPage
1536

3

5" | ./paged_memory

echo -e "\nAll tests completed!"
