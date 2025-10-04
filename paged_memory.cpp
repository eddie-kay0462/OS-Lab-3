#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <map>

using namespace std;

// Structure to represent a job
struct Job {
    int id;
    string name;
    int size;  // Size in bytes
    vector<int> pages;  // Page numbers assigned to this job
};

// Structure to represent a page
struct Page {
    int pageNumber;
    int frameNumber;
    bool isValid;
    int jobId;
    int offset;
};

// Structure to represent a page frame
struct PageFrame {
    int frameNumber;
    bool isOccupied;
    int jobId;
    int pageNumber;
    int size;
};

class PagedMemoryManager {
private:
    int pageSize;
    int totalFrames;
    vector<PageFrame> frames;
    vector<Page> pages;
    vector<Job> jobs;
    map<int, int> pageTable;  // page number -> frame number mapping
    int nextJobId;
    int nextPageNumber;
    
public:
    PagedMemoryManager(int pageSize, int totalFrames) 
        : pageSize(pageSize), totalFrames(totalFrames), nextJobId(1), nextPageNumber(1) {
        // Initialize frames
        frames.resize(totalFrames);
        for (int i = 0; i < totalFrames; i++) {
            frames[i] = {i, false, -1, -1, pageSize};
        }
    }
    
    // Accept a new job and divide it into pages
    bool acceptJob(string jobName, int jobSize) {
        // Calculate number of pages needed
        int pagesNeeded = (jobSize + pageSize - 1) / pageSize;  // Ceiling division
        
        // Check if we have enough free frames
        int freeFrames = 0;
        for (const auto& frame : frames) {
            if (!frame.isOccupied) freeFrames++;
        }
        
        if (freeFrames < pagesNeeded) {
            cout << "Error: Not enough free frames. Need " << pagesNeeded 
                 << " frames, but only " << freeFrames << " are available." << endl;
            return false;
        }
        
        // Create a job
        Job newJob;
        newJob.id = nextJobId++;
        newJob.name = jobName;
        newJob.size = jobSize;
        
        // Calculate internal fragmentation for the last page of the job
        int internalFragmentation = 0;
        if (jobSize % pageSize != 0) {
            internalFragmentation = pageSize - (jobSize % pageSize);
        }
        
        // Allocate pages randomly to available frames
        vector<int> availableFrames;
        for (int i = 0; i < totalFrames; i++) {
            if (!frames[i].isOccupied) {
                availableFrames.push_back(i);
            }
        }
        
        // Shuffle available frames for random allocation of pages to frames
        random_device rd;
        mt19937 g(rd());
        shuffle(availableFrames.begin(), availableFrames.end(), g);
        
        // Assign pages to frames
        for (int i = 0; i < pagesNeeded; i++) {
            int frameNumber = availableFrames[i];
            
            // Create page
            Page newPage;
            newPage.pageNumber = nextPageNumber++;
            newPage.frameNumber = frameNumber;
            newPage.isValid = true;
            newPage.jobId = newJob.id;
            newPage.offset = i * pageSize;
            
            pages.push_back(newPage);
            newJob.pages.push_back(newPage.pageNumber);
            
            // Update frame
            frames[frameNumber].isOccupied = true;
            frames[frameNumber].jobId = newJob.id;
            frames[frameNumber].pageNumber = newPage.pageNumber;
            
            // Update page table
            pageTable[newPage.pageNumber] = frameNumber;
        }
        
        jobs.push_back(newJob);
        
        // Display job allocation information
        cout << "\n=== Job Allocated Successfully ===" << endl;
        cout << "Job ID: " << newJob.id << endl;
        cout << "Job Name: " << newJob.name << endl;
        cout << "Job Size: " << jobSize << " bytes" << endl;
        cout << "Pages Allocated: " << pagesNeeded << endl;
        if (internalFragmentation > 0) {
            cout << "Internal Fragmentation: " << internalFragmentation << " bytes" << endl;
        } else {
            cout << "Internal Fragmentation: 0 bytes" << endl;
        }
        cout << "Page Numbers: ";
        for (int pageNum : newJob.pages) {
            cout << pageNum << " ";
        }
        cout << endl;
        
        return true;
    }
    
    // Perform address resolution
    bool resolveAddress(int jobId, int logicalAddress) {
        // Find the job
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
        
        if (logicalAddress >= job->size) {
            cout << "Error: Logical address " << logicalAddress 
                 << " is out of bounds for job " << jobId << " (size: " << job->size << ")" << endl;
            return false;
        }
        
        // Calculate page number and offset
        int pageNumber = logicalAddress / pageSize;
        int offset = logicalAddress % pageSize;
        
        // Find the actual page number for this job
        if (pageNumber >= job->pages.size()) {
            cout << "Error: Page number " << pageNumber << " is out of bounds." << endl;
            return false;
        }
        
        int actualPageNumber = job->pages[pageNumber];
        
        // Look up frame number in page table
        auto it = pageTable.find(actualPageNumber);
        if (it == pageTable.end()) {
            cout << "Error: Page " << actualPageNumber << " not found in page table." << endl;
            return false;
        }
        
        int frameNumber = it->second;
        int physicalAddress = frameNumber * pageSize + offset;
        
        // Display address resolution
        cout << "\n=== Address Resolution ===" << endl;
        cout << "Job ID: " << jobId << endl;
        cout << "Logical Address: " << logicalAddress << endl;
        cout << "Page Number: " << pageNumber << endl;
        cout << "Page Offset: " << offset << endl;
        cout << "Actual Page Number: " << actualPageNumber << endl;
        cout << "Frame Number: " << frameNumber << endl;
        cout << "Physical Address: " << physicalAddress << endl;
        
        return true;
    }
    
    // Display current memory state
    void displayMemoryState() {
        cout << "\n=== Memory State ===" << endl;
        cout << "Page Size: " << pageSize << " bytes" << endl;
        cout << "Total Frames: " << totalFrames << endl;
        cout << "Used Frames: ";
        
        int usedFrames = 0;
        for (const auto& frame : frames) {
            if (frame.isOccupied) {
                usedFrames++;
            }
        }
        cout << usedFrames << " / " << totalFrames << endl;
        
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
    
    // Remove a job and free its frames
    bool removeJob(int jobId) {
        auto it = find_if(jobs.begin(), jobs.end(), 
                         [jobId](const Job& job) { return job.id == jobId; });
        
        if (it == jobs.end()) {
            cout << "Error: Job ID " << jobId << " not found." << endl;
            return false;
        }
        
        Job job = *it;
        
        // Free all frames used by this job
        for (int pageNumber : job.pages) {
            auto pageTableIt = pageTable.find(pageNumber);
            if (pageTableIt != pageTable.end()) {
                int frameNumber = pageTableIt->second;
                frames[frameNumber].isOccupied = false;
                frames[frameNumber].jobId = -1;
                frames[frameNumber].pageNumber = -1;
                pageTable.erase(pageTableIt);
            }
        }
        
        // Remove pages from pages vector
        pages.erase(remove_if(pages.begin(), pages.end(),
                            [jobId](const Page& page) { return page.jobId == jobId; }),
                   pages.end());
        
        // Remove job
        jobs.erase(it);
        
        cout << "Job " << jobId << " removed successfully." << endl;
        return true;
    }
};

int main() {
    cout << "=== Paged Memory Allocation Simulator ===" << endl;
    
    int pageSize, totalFrames;
    cout << "Enter page size (bytes): ";
    cin >> pageSize;
    cout << "Enter total number of page frames: ";
    cin >> totalFrames;
    
    PagedMemoryManager manager(pageSize, totalFrames);
    
    int choice;
    do {
        cout << "\n=== Menu ===" << endl;
        cout << "1. Accept a job" << endl;
        cout << "2. Resolve address" << endl;
        cout << "3. Display memory state" << endl;
        cout << "4. Remove a job" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1: {
                string jobName;
                int jobSize;
                cout << "Enter job name: ";
                cin >> jobName;
                cout << "Enter job size (bytes): ";
                cin >> jobSize;
                manager.acceptJob(jobName, jobSize);
                break;
            }
            case 2: {
                int jobId, logicalAddress;
                cout << "Enter job ID: ";
                cin >> jobId;
                cout << "Enter logical address: ";
                cin >> logicalAddress;
                manager.resolveAddress(jobId, logicalAddress);
                break;
            }
            case 3: {
                manager.displayMemoryState();
                break;
            }
            case 4: {
                int jobId;
                cout << "Enter job ID to remove: ";
                cin >> jobId;
                manager.removeJob(jobId);
                break;
            }
            case 5: {
                cout << "Exiting..." << endl;
                break;
            }
            default: {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
    } while (choice != 5);
    
    return 0;
}
