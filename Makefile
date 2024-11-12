# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17
INCLUDES := -Iinclude
LDFLAGS := -lsqlite3

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN := $(BUILD_DIR)/quacker

# Source files and objects
SRC := $(wildcard $(SRC_DIR)/*.cc)
OBJ := $(SRC:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

# Default target
all: $(BIN) clean

# Build the executable
$(BIN): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ $(LDFLAGS)

# Clean up all build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o

# Phony targets
.PHONY: all clean