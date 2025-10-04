# Makefile for Paged Memory Allocation Simulator

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = paged_memory
SOURCE = paged_memory.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
