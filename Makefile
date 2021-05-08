CC := gcc
# add useful flags here
CFLAGS := -Wall -g3 -Og
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

steps:
	make parsers
	make all

all: directories $(TARGET)

parsers:
	flex analyzer.l
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
	rm src/analyzer.*
	rm src/parser.*

test:
	make run ARGS=tests/arith_quads.alpha
	make run ARGS=tests/func_quads.alpha
	make run ARGS=tests/tableitem_quads.alpha
	make run ARGS=tests/ifstmts.alpha
	make run ARGS=tests/whilestmts.alpha

run:
	$(BUILD)/$(TARGET) $(ARGS)
