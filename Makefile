# Define compiler
CC = gcc

# Define compiler flags
CFLAGS := -std=c17 -Wall -Wextra -I. -O2 -DNDEBUG

# Define source files
SRCS := main.c database.c records.c sort.c summary.c

# Define object files
OBJECTS = $(patsubst %.c,build/%.o,$(SRCS))

# Define output executable
TARGET := cms_P5-4

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

build:
	mkdir -p build

# Clean up build files
.PHONY: all clean
all: $(TARGET)

clean:
	rm -rf build $(TARGET)




