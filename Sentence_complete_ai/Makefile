# ==============================================================================
# Compiler Configuration
# ==============================================================================
CXX      := g++
CXXFLAGS := -O3 -march=native -std=c++17 -Wall -Wextra
INCLUDES := -I/usr/include/eigen3 -I/usr/include/opencv4

# Directories
TEST_DIR := tests
BIN_DIR  := bin

# Automatically detect all .cpp files in tests/
# Example: tests/test_eigen.cpp tests/test_math.cpp ...
SRCS := $(wildcard $(TEST_DIR)/*.cpp)

# Map each .cpp file to an executable in bin/
# Example: bin/test_eigen bin/test_math ...
BINS := $(patsubst $(TEST_DIR)/%.cpp, $(BIN_DIR)/%, $(SRCS))

# ==============================================================================
# Targets
# ==============================================================================

# Default: compile all tests into bin/, DO NOT RUN
all: $(BINS)

# Pattern rule: how to turn ANY tests/%.cpp into bin/%
$(BIN_DIR)/%: $(TEST_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	@echo "[CXX] Compiling $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

# Run ALL tests in bin/
test: $(BINS)
	@echo "=========================================="
	@echo "Running all tests..."
	@echo "=========================================="
	@for t in $(BINS); do \
		echo "--> Executing $$t"; \
		./$$t || exit 1; \
		echo ""; \
	done

# Run a SINGLE specific test
# Usage: make run TEST=test_eigen
run: $(BIN_DIR)/$(TEST)
	@echo "--> Running $(BIN_DIR)/$(TEST)..."
	@./$(BIN_DIR)/$(TEST)

# Clean all binaries
clean:
	@echo "Cleaning up $(BIN_DIR)..."
	rm -rf $(BIN_DIR)

.PHONY: all test run clean
