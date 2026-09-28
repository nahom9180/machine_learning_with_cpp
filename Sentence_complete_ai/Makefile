# 1. Compiler and Flags
CXX      := g++
CXXFLAGS := -O3 -march=native -std=c++17 -Wall
INCLUDES := -I/usr/include/eigen3

# 2. Directories
TEST_DIR := tests
BIN_DIR  := bin

# 3. Targets
TARGET   := $(BIN_DIR)/eigens
SRC      := $(TEST_DIR)/eigens_matrix.cpp

# Default target: build the executable
all: $(TARGET)

# Rule to build the binary
# $@ means 'target name' ($(TARGET))
# $< means 'first prerequisite' ($(SRC))
$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

# Convenience rule: compile and run immediately
run: $(TARGET)
	@echo "--- Running $(TARGET) ---"
	@./$(TARGET)

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

.PHONY: all run clean
