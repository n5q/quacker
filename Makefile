# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17
INCLUDES := -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := $(BUILD_DIR)/quacker

# Source files and objects
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(BIN) clean

# Build the executable
$(BIN): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up all build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o

# Phony targets
.PHONY: all clean