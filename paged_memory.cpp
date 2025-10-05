/**
 * Paged Memory Allocation Simulator
 * 
 * This program simulates a paged memory allocation system demonstrating:
 * - Page allocation and deallocation
 * - Address translation (logical to physical)
 * - Internal fragmentation calculation
 * - Random frame allocation
 * - Memory management operations
 * 
 * Author: OS Lab Project
 * Version: 2.0 (Fixed)
 */

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <map>
#include <limits>  // For input validation

using namespace std;

/**
 * Structure to represent a job/process in the system
 * Contains job metadata and page assignments
 */
struct Job {
    int id;              // Unique job identifier
    string name;         // Human-readable job name
    int size;           // Job size in bytes
    vector<int> pages;  // Page numbers assigned to this job
};

/**
 * Structure to represent a logical page
 * Maps logical pages to physical frames
 */
struct Page {
    int pageNumber;   // Logical page number
    int frameNumber;  // Physical frame number
    bool isValid;     // Page validity flag
    int jobId;        // ID of job owning this page
    int offset;       // Offset within the page
};

/**
 * Structure to represent a physical page frame
 * Represents actual memory blocks in physical RAM
 */
struct PageFrame {
    int frameNumber;  // Physical frame number
    bool isOccupied;  // Whether frame is in use
    int jobId;        // ID of job using this frame
    int pageNumber;   // Logical page number stored here
    int size;         // Frame size in bytes (equal to page size)
};

/**
 * Paged Memory Manager Class
 * 
 * Manages the entire paged memory system including:
 * - Physical frame allocation
 * - Logical page management
 * - Address translation
 * - Job lifecycle management
 */
class PagedMemoryManager {
private:
    // System configuration
    int pageSize;        // Size of each page/frame in bytes
    int totalFrames;     // Total number of physical frames available
    
    // Data structures for memory management
    vector<PageFrame> frames;    // Physical memory frames
    vector<Page> pages;          // Logical pages
    vector<Job> jobs;            // Active jobs/processes
    map<int, int> pageTable;     // Page number -> Frame number mapping
    
    // ID generators for unique identification
    int nextJobId;       // Next available job ID
    int nextPageNumber;  // Next available page number
    
public:
    /**
     * Constructor - Initialize the paged memory manager
     * @param pageSize Size of each page/frame in bytes
     * @param totalFrames Total number of physical frames
     */
    PagedMemoryManager(int pageSize, int totalFrames) 
        : pageSize(pageSize), totalFrames(totalFrames), nextJobId(1), nextPageNumber(1) {
        
        // Validate input parameters
        if (pageSize <= 0 || totalFrames <= 0) {
            throw invalid_argument("Page size and frame count must be positive");
        }
        
        // Initialize all frames as free
        frames.resize(totalFrames);
        for (int i = 0; i < totalFrames; i++) {
            frames[i] = {i, false, -1, -1, pageSize};
        }
    }
    
    /**
     * Accept a new job and allocate memory pages for it
     * @param jobName Name of the job/process
     * @param jobSize Size of the job in bytes
     * @return true if job was successfully allocated, false otherwise
     */
    bool acceptJob(string jobName, int jobSize) {
        
        // Input validation - reject negative or zero job sizes
        if (jobSize <= 0) {
            cout << "Error: Job size must be positive. Got: " << jobSize << " bytes" << endl;
            return false;
        }
        
        // Input validation - check for reasonable job size limits
        const int MAX_JOB_SIZE = 100 * 1024 * 1024; // 100MB limit
        if (jobSize > MAX_JOB_SIZE) {
            cout << "Error: Job size too large. Maximum allowed: " << MAX_JOB_SIZE << " bytes" << endl;
            return false;
        }
        
        // Calculate number of pages needed using ceiling division
        int pagesNeeded = (jobSize + pageSize - 1) / pageSize;
        
        // Check if we have enough free frames for this job
        int freeFrames = 0;
        for (const auto& frame : frames) {
            if (!frame.isOccupied) freeFrames++;
        }
        
        if (freeFrames < pagesNeeded) {
            cout << "Error: Not enough free frames. Need " << pagesNeeded 
                 << " frames, but only " << freeFrames << " are available." << endl;
            return false;
        }
        
        // Create new job object with validated parameters
        Job newJob;
        newJob.id = nextJobId++;
        newJob.name = jobName;
        newJob.size = jobSize;
        
        // Calculate internal fragmentation (wasted space in last page)
        int internalFragmentation = 0;
        if (jobSize % pageSize != 0) {
            // If job doesn't fill the last page completely, there's fragmentation
            internalFragmentation = pageSize - (jobSize % pageSize);
        }
        
        // Collect all available (free) frames
        vector<int> availableFrames;
        for (int i = 0; i < totalFrames; i++) {
            if (!frames[i].isOccupied) {
                availableFrames.push_back(i);
            }
        }
        
        // Randomize frame selection to prevent clustering and demonstrate
        // non-contiguous memory allocation (key feature of paging)
        random_device rd;
        mt19937 g(rd());
        shuffle(availableFrames.begin(), availableFrames.end(), g);
        
        // Allocate pages to randomly selected frames
        for (int i = 0; i < pagesNeeded; i++) {
            int frameNumber = availableFrames[i];
            
            // Create new logical page
            Page newPage;
            newPage.pageNumber = nextPageNumber++;
            newPage.frameNumber = frameNumber;
            newPage.isValid = true;
            newPage.jobId = newJob.id;
            newPage.offset = i * pageSize;
            
            // Add page to system
            pages.push_back(newPage);
            newJob.pages.push_back(newPage.pageNumber);
            
            // Mark frame as occupied and update frame metadata
            frames[frameNumber].isOccupied = true;
            frames[frameNumber].jobId = newJob.id;
            frames[frameNumber].pageNumber = newPage.pageNumber;
            
            // Update page table for address translation
            pageTable[newPage.pageNumber] = frameNumber;
        }
        
        // Add job to active jobs list
        jobs.push_back(newJob);
        
        // Display comprehensive job allocation information
        cout << "\n=== Job Allocated Successfully ===" << endl;
        cout << "Job ID: " << newJob.id << endl;
        cout << "Job Name: " << newJob.name << endl;
        cout << "Job Size: " << jobSize << " bytes" << endl;
        cout << "Pages Allocated: " << pagesNeeded << endl;
        
        // Display fragmentation information
        if (internalFragmentation > 0) {
            cout << "Internal Fragmentation: " << internalFragmentation << " bytes" << endl;
            cout << "Fragmentation Percentage: " << fixed << setprecision(2) 
                 << (double)internalFragmentation / jobSize * 100 << "%" << endl;
        } else {
            cout << "Internal Fragmentation: 0 bytes (perfect fit)" << endl;
        }
        
        cout << "Page Numbers: ";
        for (int pageNum : newJob.pages) {
            cout << pageNum << " ";
        }
        cout << endl;
        
        return true;
    }
    
    /**
     * Perform address resolution from logical to physical address
     * @param jobId ID of the job
     * @param logicalAddress Logical address to resolve
     * @return true if resolution successful, false otherwise
     */
    bool resolveAddress(int jobId, int logicalAddress) {
        
        // Input validation - reject negative addresses
        if (logicalAddress < 0) {
            cout << "Error: Logical address cannot be negative. Got: " << logicalAddress << endl;
            return false;
        }
        
        // Find the job by ID
        Job* job = nullptr;
        for (auto& j : jobs) {
            if (j.id == jobId) {
                job = &j;
                break;
            }
        }
        
        if (!job) {
            cout << "Error: Job ID " << jobId << " not found." << endl;
            return false;
        }
        
        // Check if logical address is within job bounds
        if (logicalAddress >= job->size) {
            cout << "Error: Logical address " << logicalAddress 
                 << " is out of bounds for job " << jobId << " (size: " << job->size << ")" << endl;
            return false;
        }
        
        // Perform address translation (logical -> physical)
        // Step 1: Extract page number and offset from logical address
        int pageNumber = logicalAddress / pageSize;
        int offset = logicalAddress % pageSize;
        
        // Step 2: Validate page number is within job's allocated pages
        if (pageNumber >= static_cast<int>(job->pages.size())) {
            cout << "Error: Page number " << pageNumber << " is out of bounds." << endl;
            return false;
        }
        
        // Step 3: Get actual page number from job's page list
        int actualPageNumber = job->pages[pageNumber];
        
        // Step 4: Look up frame number in page table
        auto it = pageTable.find(actualPageNumber);
        if (it == pageTable.end()) {
            cout << "Error: Page " << actualPageNumber << " not found in page table." << endl;
            return false;
        }
        
        // Step 5: Calculate physical address
        int frameNumber = it->second;
        int physicalAddress = frameNumber * pageSize + offset;
        
        // Display comprehensive address resolution information
        cout << "\n=== Address Resolution ===" << endl;
        cout << "Job ID: " << jobId << endl;
        cout << "Logical Address: " << logicalAddress << endl;
        cout << "Page Number: " << pageNumber << endl;
        cout << "Page Offset: " << offset << endl;
        cout << "Actual Page Number: " << actualPageNumber << endl;
        cout << "Frame Number: " << frameNumber << endl;
        cout << "Physical Address: " << physicalAddress << endl;
        
        // Verify the translation is correct
        cout << "Verification: Frame " << frameNumber << " * " << pageSize 
             << " + " << offset << " = " << physicalAddress << endl;
        
        return true;
    }
    
    /**
     * Display comprehensive memory state information
     * Shows frame allocation, page table, and job information
     */
    void displayMemoryState() {
        cout << "\n=== Memory State ===" << endl;
        cout << "Page Size: " << pageSize << " bytes" << endl;
        cout << "Total Frames: " << totalFrames << endl;
        cout << "Memory Efficiency: ";
        
        // Calculate memory utilization statistics
        int usedFrames = 0;
        for (const auto& frame : frames) {
            if (frame.isOccupied) {
                usedFrames++;
            }
        }
        
        double utilization = (double)usedFrames / totalFrames * 100;
        cout << usedFrames << " / " << totalFrames << " (" << fixed << setprecision(1) 
             << utilization << "% used)" << endl;
        
        cout << "\nFrame Allocation:" << endl;
        cout << setw(8) << "Frame" << setw(10) << "Job ID" << setw(12) << "Page #" << setw(8) << "Status" << endl;
        cout << string(40, '-') << endl;
        
        for (const auto& frame : frames) {
            cout << setw(8) << frame.frameNumber 
                 << setw(10) << (frame.isOccupied ? to_string(frame.jobId) : "-")
                 << setw(12) << (frame.isOccupied ? to_string(frame.pageNumber) : "-")
                 << setw(8) << (frame.isOccupied ? "Used" : "Free") << endl;
        }
        
        cout << "\nPage Table:" << endl;
        cout << setw(10) << "Page #" << setw(12) << "Frame #" << endl;
        cout << string(25, '-') << endl;
        for (const auto& entry : pageTable) {
            cout << setw(10) << entry.first << setw(12) << entry.second << endl;
        }
        
        cout << "\nJobs:" << endl;
        cout << setw(8) << "Job ID" << setw(15) << "Job Name" << setw(10) << "Size" 
             << setw(15) << "Pages" << endl;
        cout << string(50, '-') << endl;
        for (const auto& job : jobs) {
            cout << setw(8) << job.id 
                 << setw(15) << job.name 
                 << setw(10) << job.size
                 << setw(15) << job.pages.size() << endl;
        }
    }
    
    /**
     * Remove a job and free all its allocated frames
     * @param jobId ID of the job to remove
     * @return true if job was successfully removed, false otherwise
     */
    bool removeJob(int jobId) {
        // Input validation - reject negative job IDs
        if (jobId <= 0) {
            cout << "Error: Job ID must be positive. Got: " << jobId << endl;
            return false;
        }
        
        // Find the job to remove
        auto it = find_if(jobs.begin(), jobs.end(), 
                         [jobId](const Job& job) { return job.id == jobId; });
        
        if (it == jobs.end()) {
            cout << "Error: Job ID " << jobId << " not found." << endl;
            return false;
        }
        
        Job job = *it;
        
        // Free all frames used by this job
        cout << "Freeing " << job.pages.size() << " frames for job " << jobId << "..." << endl;
        
        for (int pageNumber : job.pages) {
            auto pageTableIt = pageTable.find(pageNumber);
            if (pageTableIt != pageTable.end()) {
                int frameNumber = pageTableIt->second;
                
                // Mark frame as free
                frames[frameNumber].isOccupied = false;
                frames[frameNumber].jobId = -1;
                frames[frameNumber].pageNumber = -1;
                
                // Remove from page table
                pageTable.erase(pageTableIt);
            }
        }
        
        // Remove all pages belonging to this job
        pages.erase(remove_if(pages.begin(), pages.end(),
                            [jobId](const Page& page) { return page.jobId == jobId; }),
                   pages.end());
        
        // Remove job from active jobs list
        jobs.erase(it);
        
        cout << "Job " << jobId << " ('" << job.name << "') removed successfully." << endl;
        cout << "Freed " << job.pages.size() << " pages and " << job.pages.size() << " frames." << endl;
        
        return true;
    }
};

/**
 * Main function - Entry point of the program
 * Handles user interaction and menu system
 */
int main() {
    cout << "=== Paged Memory Allocation Simulator v2.0 ===" << endl;
    cout << "Fixed version with comprehensive input validation and error handling" << endl;
    cout << endl;
    
    // Get system configuration with input validation
    int pageSize, totalFrames;
    
    // Get and validate page size
    do {
        cout << "Enter page size (bytes): ";
        if (!(cin >> pageSize)) {
            cout << "Error: Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if (pageSize <= 0) {
            cout << "Error: Page size must be positive. Got: " << pageSize << endl;
        } else if (pageSize > 1024 * 1024) { // 1MB limit
            cout << "Error: Page size too large. Maximum: 1MB" << endl;
        } else {
            break;
        }
    } while (true);
    
    // Get and validate frame count
    do {
        cout << "Enter total number of page frames: ";
        if (!(cin >> totalFrames)) {
            cout << "Error: Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        if (totalFrames <= 0) {
            cout << "Error: Frame count must be positive. Got: " << totalFrames << endl;
        } else if (totalFrames > 1000) { // Reasonable limit
            cout << "Error: Too many frames. Maximum: 1000" << endl;
        } else {
            break;
        }
    } while (true);
    
    // Initialize memory manager with validated parameters
    PagedMemoryManager manager(pageSize, totalFrames);
    
    cout << "\nSystem initialized successfully!" << endl;
    cout << "Total memory: " << (pageSize * totalFrames) << " bytes" << endl;
    
    // Main program loop
    int choice;
    do {
        cout << "\n=== Menu ===" << endl;
        cout << "1. Accept a job" << endl;
        cout << "2. Resolve address" << endl;
        cout << "3. Display memory state" << endl;
        cout << "4. Remove a job" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        
        // Validate menu choice input
        if (!(cin >> choice)) {
            cout << "Error: Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        // Process user menu choice
        switch (choice) {
            case 1: {
                // Accept new job
                string jobName;
                int jobSize;
                
                cout << "Enter job name: ";
                cin.ignore(); // Clear input buffer
                getline(cin, jobName); // Allow spaces in job names
                
                cout << "Enter job size (bytes): ";
                if (!(cin >> jobSize)) {
                    cout << "Error: Invalid job size. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                
                manager.acceptJob(jobName, jobSize);
                break;
            }
            case 2: {
                // Resolve logical address to physical address
                int jobId, logicalAddress;
                
                cout << "Enter job ID: ";
                if (!(cin >> jobId)) {
                    cout << "Error: Invalid job ID. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                
                cout << "Enter logical address: ";
                if (!(cin >> logicalAddress)) {
                    cout << "Error: Invalid logical address. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                
                manager.resolveAddress(jobId, logicalAddress);
                break;
            }
            case 3: {
                manager.displayMemoryState();
                break;
            }
            case 4: {
                // Remove existing job
                int jobId;
                
                cout << "Enter job ID to remove: ";
                if (!(cin >> jobId)) {
                    cout << "Error: Invalid job ID. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                
                manager.removeJob(jobId);
                break;
            }
            case 5: {
                cout << "Exiting..." << endl;
                break;
            }
            default: {
                cout << "Invalid choice. Please enter a number between 1 and 5." << endl;
            }
        }
    } while (choice != 5);
    
    cout << "\nThank you for using the Paged Memory Allocation Simulator!" << endl;
    cout << "Program terminated successfully." << endl;
    
    return 0;
}
