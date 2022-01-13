# the compiler: gcc for C program
CC = gcc

CFLAGS = -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter -g -DDEBUG -std=c99

BUILD_DIR = ./build
SRC_DIR = ./src
LIB_DIR = ./lib
TESTS_DIR = ./tests

all : $(BUILD_DIR)/bits-test $(BUILD_DIR)/position-test $(BUILD_DIR)/moves-test $(BUILD_DIR)/zobrist-test

debug : $(BUILD_DIR)/bits-test $(BUILD_DIR)/position-test $(BUILD_DIR)/moves-test $(BUILD_DIR)/zobrist-test

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : $(TESTS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bits-test : $(BUILD_DIR)/bits-test.o $(BUILD_DIR)/bits.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/bits-test.o $(BUILD_DIR)/bits.o -o $(BUILD_DIR)/bits-test

$(BUILD_DIR)/position-test : $(BUILD_DIR)/position-test.o $(BUILD_DIR)/position.o 
	$(CC) $(CFLAGS) $(BUILD_DIR)/position-test.o $(BUILD_DIR)/position.o $(BUILD_DIR)/bits.o -o $(BUILD_DIR)/position-test

$(BUILD_DIR)/moves-test : $(BUILD_DIR)/moves-test.o $(BUILD_DIR)/moves.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/moves-test.o $(BUILD_DIR)/moves.o $(BUILD_DIR)/bits.o $(BUILD_DIR)/position.o -o $(BUILD_DIR)/moves-test

$(BUILD_DIR)/zobrist-test : $(BUILD_DIR)/zobrist-test.o $(BUILD_DIR)/zobrist.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/zobrist-test.o $(BUILD_DIR)/zobrist.o $(BUILD_DIR)/bits.o $(BUILD_DIR)/position.o $(BUILD_DIR)/moves.o -o $(BUILD_DIR)/zobrist-test
	
clean:
	rm -f $(BUILD_DIR)/*
