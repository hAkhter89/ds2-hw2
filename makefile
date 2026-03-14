CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
CPPFLAGS := -Iinclude
LDFLAGS  := 

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
EXPERIMENTS_DIR := experiments

SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET   := $(BIN_DIR)/hashing_test

.PHONY: all clean run directories

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS) $(EXPERIMENTS_DIR)/experiment.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

run: all
	@echo "Running tests..."
	@./$(TARGET)

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

help:
	@echo "Available targets:"
	@echo "  all         Build the experiments executable (default)"
	@echo "  run         Build and run the experiments"
	@echo "  clean       Remove build artifacts"
	@echo "  directories Create build directory structure"
