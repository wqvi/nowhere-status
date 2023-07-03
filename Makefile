BIN := nowhere-status

CFLAGS = -O2 -pedantic -Wall -Wextra -Wno-sign-compare -std=gnu99
LIBS = -lcurl -lm

BUILD_DIR := ./build
SRC_DIR := ./src

OBJS := build/main.o build/nowhere_alloc.o build/nowhere_battery.o build/nowhere_date.o build/nowhere_map.o build/nowhere_network.o build/nowhere_ram.o build/nowhere_read.o build/nowhere_swaybar.o build/nowhere_temperature.o build/nowhere_weather.o

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
