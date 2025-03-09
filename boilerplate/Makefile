# Simple Makefile for EER projects

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./include -I../include

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
TEST_DIR = test

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# EER library path (adjust as needed)
EER_LIB = ../build/libeer.a

# Main targets
.PHONY: all clean test

all: $(BUILD_DIR) $(BUILD_DIR)/app

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/components

$(BUILD_DIR)/app: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(EER_LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test targets
test: $(BUILD_DIR) $(BUILD_DIR)/test_runner

$(BUILD_DIR)/test_runner: $(TEST_DIR)/*.c
	$(CC) $(CFLAGS) -o $@ $^ $(EER_LIB)
	./$(BUILD_DIR)/test_runner

# Clean
clean:
	rm -rf $(BUILD_DIR)
