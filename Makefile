BIN := nowhere-status

BIN_DIR := ./bin
SRC_DIR := ./src
MODULES_DIR := ./src/modules
MODULES_BIN := ./$(BIN_DIR)/

CFLAGS = -I$(SRC_DIR) $(shell pkg-config --cflags playerctl) -O2 -pedantic -Wall -Wextra -Wno-sign-compare -std=gnu99
LIBS = -lcurl -lm $(shell pkg-config --libs playerctl)

OBJS := bin/main.o bin/node.o bin/swaybar.o \
	bin/date.o bin/network.o bin/ram.o bin/sysfs.o \
	bin/player.o

.DELETE_ON_ERROR:
$(BIN_DIR)/$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

.DELETE_ON_ERROR: 
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.DELETE_ON_ERROR:
$(BIN_DIR)/%.o: $(MODULES_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	rm -r $(BIN_DIR)
