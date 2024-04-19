CFLAGS = -Isrc $(shell pkg-config --cflags playerctl) -O2 -pedantic -Wall -Wextra -Wno-sign-compare -std=gnu99
LIBS = -lm $(shell pkg-config --libs playerctl)

OBJS := bin/main.o bin/node.o bin/swaybar.o \
	bin/date.o bin/network.o bin/ram.o bin/sysfs.o \
	bin/player.o

TESTS_OBJS := bin/tests/modules.o bin/tests/llist.o \
	bin/tests/player.o bin/tests/node.o

all: bin/nowhere-status bin/tests/check

tests: bin/tests/check
	bin/tests/check

.PHONY:
clean:
	rm -r bin 

# Compile binary rules
.DELETE_ON_ERROR:
bin/nowhere-status: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

.DELETE_ON_ERROR:
bin/tests/check: $(TESTS_OBJS)
	$(CC) $(CFLAGS) $(TESTS_OBJS) $(LIBS) -lcheck -o $@

# Compile object rules
.DELETE_ON_ERROR: 
bin/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.DELETE_ON_ERROR:
bin/%.o: src/modules/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.DELETE_ON_ERROR:
bin/tests/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) -Isrc -std=gnu99 -Wall -DDEBUG -c $< -o $@

.DELETE_ON_ERROR:
bin/tests/%.o: src/modules/%.c
	mkdir -p $(dir $@)
	$(CC) -Isrc -std=gnu99 -Wall $(shell pkg-config --cflags playerctl) -DDEBUG -c $< -o $@

.DELETE_ON_ERROR:
bin/tests/%.o: tests/%.c
	mkdir -p $(dir $@)
	$(CC) -Isrc -std=gnu99 -Wall -DDEBUG -c $< -o $@
