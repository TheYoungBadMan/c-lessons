CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
CPPFLAGS = -I$(INC_DIR) -MMD -MP -MF $(DEP_DIR)/$*.d
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)

TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(DEP_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR) $(DEP_DIR) $(BIN_DIR):
	@mkdir -p $@

-include $(DEPS)

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(TARGET)
	@echo "Running..."
	@$(TARGET)

valrun: $(TARGET)
	@echo "Running with valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s -q $(TARGET)

debug: $(TARGET)
	@echo "Debugging..."
	@gdb -q $(TARGET)

.PHONY: all clean run valrun debug