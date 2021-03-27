CC := gcc
# add useful flags here
CFLAGS := -Wall
# change if you don't like the name
BUILD := ./bin
# change target to your preference
TARGET := alpha_compiler
# change if you don't like the hierarchy
OBJ_DIR := $(BUILD)/objects
INCLUDE := -Isrc/
# add any dir in src
SRC := $(wildcard src/*.c)
OBJS := $(SRC:%.c=$(OBJ_DIR)/%.o)

all: directories $(TARGET)

parsers:
	lex analyzer.l
	bison parser.y

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(BUILD)/$(TARGET) $^

.PHONY: all directories clean

directories:
	mkdir -p $(BUILD)
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BUILD)

run:
	$(BUILD)/$(TARGET)
