BIN := nowhere-status

BUILD_DIR := ./build
SRC_DIR := ./src

CFLAGS = -I$(SRC_DIR) -O2 -pedantic -Wall -Wextra -Wno-sign-compare -std=gnu99
LIBS = -lcurl -lm

OBJS := build/main.o build/map.o build/io.o build/swaybar.o \
	build/battery.o build/date.o build/network.o build/ram.o build/temperature.o build/weather.o

.DELETE_ON_ERROR:
$(BUILD_DIR)/$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

.DELETE_ON_ERROR: 
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.DELETE_ON_ERROR: 
$(BUILD_DIR)/%.o: $(SRC_DIR)/modules/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	rm -r $(BUILD_DIR)
