BIN := status

CFLAGS = -O2 -pedantic -Wall -Wextra -Wno-sign-compare -std=c99
LIBS = -lcurl -lm

BUILD_DIR := ./build
SRC_DIR := ./src

OBJS := build/bat.o build/bat0.o build/cpu_temp.o build/date.o build/net.o build/ram.o build/slurp.o build/status.o build/weather.o

.DELETE_ON_ERROR:
$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

.DELETE_ON_ERROR: 
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	rm -r $(BUILD_DIR)
