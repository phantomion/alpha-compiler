CC := gcc
# add useful flags here
CFLAGS := -Wall -g3 -Og -lm
# change if you don't like the name
BUILD := ./bin
# change target to your preference
TARGET := avm
# change if you don't like the hierarchy
OBJ_DIR := $(BUILD)/objects
# add any dir in src
SRC := $(wildcard *.c) \
	../vm_structs.c
OBJS := $(SRC:%.c=$(OBJ_DIR)/%.o)

all: directories $(TARGET)

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
	$(BUILD)/$(TARGET) $(ARGS)
