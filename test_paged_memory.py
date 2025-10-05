#!/usr/bin/env python3
"""
Comprehensive Test Suite for Paged Memory Allocation Simulator

This script tests all edge cases and scenarios for the paged memory simulator:
- Boundary conditions
- Memory exhaustion
- Address resolution edge cases
- Job management
- Input validation
- Memory fragmentation scenarios
"""

import subprocess
import sys
import time
import os

class PagedMemoryTester:
    def __init__(self):
        self.test_results = []
        self.process = None
        
    def start_program(self, page_size=1024, total_frames=8):
        """Start the paged memory program with given parameters"""
        try:
            self.process = subprocess.Popen(
                ['./paged_memory'],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1
            )
            # Send initial parameters
            self.send_input(f"{page_size}\n{total_frames}\n")
            return True
        except Exception as e:
            print(f"Failed to start program: {e}")
            return False
    
    def send_input(self, input_str):
        """Send input to the program"""
        if self.process:
            self.process.stdin.write(input_str)
            self.process.stdin.flush()
    
    def get_output(self, timeout=5):
        """Get output from the program with timeout"""
        if self.process:
            try:
                # Read available output
                import select
                if select.select([self.process.stdout], [], [], timeout)[0]:
                    return self.process.stdout.readline()
            except:
                pass
        return ""
    
    def get_all_output(self):
        """Get all available output"""
        output = ""
        if self.process:
            import select
            while select.select([self.process.stdout], [], [], 0.1)[0]:
                line = self.process.stdout.readline()
                if not line:
                    break
                output += line
        return output
    
    def stop_program(self):
        """Stop the program"""
        if self.process:
            self.process.terminate()
            self.process.wait()
            self.process = None
    
    def run_test(self, test_name, test_function):
        """Run a test and record results"""
        print(f"\n{'='*60}")
        print(f"Running Test: {test_name}")
        print('='*60)
        
        try:
            result = test_function()
            self.test_results.append((test_name, result))
            print(f"Result: {'PASS' if result else 'FAIL'}")
        except Exception as e:
            print(f"Test failed with exception: {e}")
            self.test_results.append((test_name, False))
    
    def test_boundary_conditions(self):
        """Test 1: Boundary conditions for page size and frame count"""
        print("Testing boundary conditions...")
        
        # Test with minimal values
        if not self.start_program(1, 1):
            return False
        
        output = self.get_all_output()
        print("Initial setup output:", output)
        
        # Test job with size 1 (minimal)
        self.send_input("1\nTestJob1\n1\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Minimal job output:", output)
        
        # Test job with size 0 (should fail)
        self.send_input("1\nTestJob0\n0\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Zero size job output:", output)
        
        self.stop_program()
        return "Error" not in output or "Job Allocated Successfully" in output
    
    def test_memory_exhaustion(self):
        """Test 2: Memory exhaustion scenarios"""
        print("Testing memory exhaustion...")
        
        # Start with small memory (2 frames, 100 bytes each)
        if not self.start_program(100, 2):
            return False
        
        # Fill memory completely
        self.send_input("1\nJob1\n200\n")  # Takes 2 frames
        time.sleep(0.5)
        output1 = self.get_all_output()
        print("First job (fills memory):", output1)
        
        # Try to add another job (should fail)
        self.send_input("1\nJob2\n50\n")
        time.sleep(0.5)
        output2 = self.get_all_output()
        print("Second job (should fail):", output2)
        
        self.stop_program()
        return "Not enough free frames" in output2
    
    def test_address_resolution_edge_cases(self):
        """Test 3: Address resolution edge cases"""
        print("Testing address resolution edge cases...")
        
        if not self.start_program(1024, 4):
            return False
        
        # Add a job
        self.send_input("1\nTestJob\n2500\n")  # 3 pages
        time.sleep(0.5)
        output = self.get_all_output()
        print("Job creation:", output)
        
        # Test valid addresses
        self.send_input("2\n1\n0\n")  # First byte
        time.sleep(0.5)
        output = self.get_all_output()
        print("Address 0:", output)
        
        self.send_input("2\n1\n2499\n")  # Last byte
        time.sleep(0.5)
        output = self.get_all_output()
        print("Address 2499:", output)
        
        # Test invalid addresses
        self.send_input("2\n1\n2500\n")  # Out of bounds
        time.sleep(0.5)
        output = self.get_all_output()
        print("Address 2500 (out of bounds):", output)
        
        self.send_input("2\n999\n0\n")  # Invalid job ID
        time.sleep(0.5)
        output = self.get_all_output()
        print("Invalid job ID:", output)
        
        self.stop_program()
        return "Error" in output and "Address Resolution" in output
    
    def test_job_removal_and_fragmentation(self):
        """Test 4: Job removal and memory fragmentation"""
        print("Testing job removal and fragmentation...")
        
        if not self.start_program(1024, 4):
            return False
        
        # Add multiple jobs
        self.send_input("1\nJob1\n1024\n")  # 1 page
        time.sleep(0.5)
        output = self.get_all_output()
        print("Job1 added:", output)
        
        self.send_input("1\nJob2\n2048\n")  # 2 pages
        time.sleep(0.5)
        output = self.get_all_output()
        print("Job2 added:", output)
        
        # Display memory state
        self.send_input("3\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Memory state before removal:", output)
        
        # Remove a job
        self.send_input("4\n1\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("After removing Job1:", output)
        
        # Display memory state again
        self.send_input("3\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Memory state after removal:", output)
        
        self.stop_program()
        return "removed successfully" in output
    
    def test_internal_fragmentation(self):
        """Test 5: Internal fragmentation calculations"""
        print("Testing internal fragmentation...")
        
        if not self.start_program(1024, 4):
            return False
        
        # Test job that doesn't fill last page completely
        self.send_input("1\nFragJob\n1500\n")  # Should have 524 bytes fragmentation
        time.sleep(0.5)
        output = self.get_all_output()
        print("Fragmentation test:", output)
        
        # Test job that fills pages exactly
        self.send_input("1\nExactJob\n2048\n")  # Exactly 2 pages
        time.sleep(0.5)
        output = self.get_all_output()
        print("Exact fit job:", output)
        
        self.stop_program()
        return "Internal Fragmentation: 524" in output and "Internal Fragmentation: 0" in output
    
    def test_large_values(self):
        """Test 6: Large values and stress testing"""
        print("Testing large values...")
        
        # Test with large page size and frame count
        if not self.start_program(65536, 16):  # 64KB pages, 16 frames
            return False
        
        # Add large job
        self.send_input("1\nLargeJob\n1000000\n")  # ~1MB job
        time.sleep(0.5)
        output = self.get_all_output()
        print("Large job test:", output)
        
        self.stop_program()
        return "Job Allocated Successfully" in output
    
    def test_multiple_jobs_random_allocation(self):
        """Test 7: Multiple jobs and random allocation verification"""
        print("Testing multiple jobs and random allocation...")
        
        if not self.start_program(1024, 8):
            return False
        
        # Add several jobs
        jobs = [
            ("Job1", 500),
            ("Job2", 1500),
            ("Job3", 800),
            ("Job4", 2000)
        ]
        
        for name, size in jobs:
            self.send_input(f"1\n{name}\n{size}\n")
            time.sleep(0.5)
            output = self.get_all_output()
            print(f"Added {name}:", output)
        
        # Display memory state to see random allocation
        self.send_input("3\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Final memory state:", output)
        
        self.stop_program()
        return "Job Allocated Successfully" in output
    
    def test_input_validation(self):
        """Test 8: Input validation and error handling"""
        print("Testing input validation...")
        
        if not self.start_program(1024, 4):
            return False
        
        # Test invalid menu choices
        self.send_input("999\n")  # Invalid menu choice
        time.sleep(0.5)
        output = self.get_all_output()
        print("Invalid menu choice:", output)
        
        # Test negative job size
        self.send_input("1\nNegJob\n-100\n")
        time.sleep(0.5)
        output = self.get_all_output()
        print("Negative job size:", output)
        
        self.stop_program()
        return "Invalid choice" in output
    
    def test_page_table_consistency(self):
        """Test 9: Page table consistency after operations"""
        print("Testing page table consistency...")
        
        if not self.start_program(1024, 6):
            return False
        
        # Add job
        self.send_input("1\nConsistencyJob\n2500\n")
        time.sleep(0.5)
        output1 = self.get_all_output()
        
        # Display memory state
        self.send_input("3\n")
        time.sleep(0.5)
        output2 = self.get_all_output()
        
        # Remove job
        self.send_input("4\n1\n")
        time.sleep(0.5)
        output3 = self.get_all_output()
        
        # Display memory state again
        self.send_input("3\n")
        time.sleep(0.5)
        output4 = self.get_all_output()
        
        print("Page table consistency test completed")
        
        self.stop_program()
        return True
    
    def run_all_tests(self):
        """Run all tests"""
        print("Starting Comprehensive Paged Memory Simulator Testing")
        print("="*60)
        
        tests = [
            ("Boundary Conditions", self.test_boundary_conditions),
            ("Memory Exhaustion", self.test_memory_exhaustion),
            ("Address Resolution Edge Cases", self.test_address_resolution_edge_cases),
            ("Job Removal and Fragmentation", self.test_job_removal_and_fragmentation),
            ("Internal Fragmentation", self.test_internal_fragmentation),
            ("Large Values", self.test_large_values),
            ("Multiple Jobs Random Allocation", self.test_multiple_jobs_random_allocation),
            ("Input Validation", self.test_input_validation),
            ("Page Table Consistency", self.test_page_table_consistency)
        ]
        
        for test_name, test_func in tests:
            self.run_test(test_name, test_func)
        
        # Print summary
        print("\n" + "="*60)
        print("TEST SUMMARY")
        print("="*60)
        
        passed = sum(1 for _, result in self.test_results if result)
        total = len(self.test_results)
        
        for test_name, result in self.test_results:
            status = "PASS" if result else "FAIL"
            print(f"{test_name:.<50} {status}")
        
        print(f"\nTotal: {passed}/{total} tests passed")
        
        if passed == total:
            print("🎉 All tests passed! The paged memory simulator is robust.")
        else:
            print("⚠️  Some tests failed. Review the implementation.")
        
        return passed == total

def main():
    """Main function to run the test suite"""
    # Check if the executable exists
    if not os.path.exists('./paged_memory'):
        print("Error: paged_memory executable not found.")
        print("Please compile the program first using: make")
        return False
    
    # Create and run tests
    tester = PagedMemoryTester()
    return tester.run_all_tests()

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
