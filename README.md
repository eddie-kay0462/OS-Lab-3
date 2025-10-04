# Paged Memory Allocation Simulator

This C++ program simulates a paged memory allocation system with the following features:

## Features

- **Accept Jobs**: Input job name and size, automatically divide into pages
- **Page Division**: Jobs are divided into pages based on specified page size
- **Internal Fragmentation Detection**: Calculates and displays internal fragmentation
- **Random Page Frame Allocation**: Pages are allocated to frames randomly
- **Address Resolution**: Converts logical addresses to physical addresses
- **Memory State Display**: Shows current frame allocation and page table
- **Job Management**: Add and remove jobs dynamically

## How to Compile and Run

### Using Make (Recommended)
```bash
make
./paged_memory
```

### Manual Compilation
```bash
g++ -std=c++11 -Wall -Wextra -O2 -o paged_memory paged_memory.cpp
./paged_memory
```

### Clean Build
```bash
make clean
```

## Program Usage

1. **Initial Setup**: Enter page size and total number of page frames
2. **Menu Options**:
   - **Accept Job**: Add a new job with name and size
   - **Resolve Address**: Convert logical address to physical address
   - **Display Memory State**: View current memory allocation
   - **Remove Job**: Remove an existing job and free its frames
   - **Exit**: Quit the program

## Example Session

```
=== Paged Memory Allocation Simulator ===
Enter page size (bytes): 1024
Enter total number of page frames: 8

=== Menu ===
1. Accept a job
2. Resolve address
3. Display memory state
4. Remove a job
5. Exit
Enter your choice: 1
Enter job name: Process1
Enter job size (bytes): 2500

=== Job Allocated Successfully ===
Job ID: 1
Job Name: Process1
Job Size: 2500 bytes
Pages Allocated: 3
Internal Fragmentation: 524 bytes
Page Numbers: 1 2 3
```

## Key Concepts Demonstrated

- **Paging**: Division of jobs into fixed-size pages
- **Page Frames**: Physical memory blocks that hold pages
- **Page Table**: Mapping between logical pages and physical frames
- **Internal Fragmentation**: Wasted space within the last page of a job
- **Address Translation**: Converting logical addresses to physical addresses
- **Random Allocation**: Non-contiguous allocation of pages to frames

## Technical Details

- Uses C++11 features for better code organization
- Implements proper data structures for jobs, pages, and frames
- Includes input validation and error handling
- Provides detailed output for educational purposes
- Supports dynamic job management (add/remove)
