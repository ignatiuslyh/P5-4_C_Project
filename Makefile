# Define compiler
CC = gcc

# Define compiler flags
CFLAGS = -I./src

# Define source files
SRCDIR = ./src
SOURCES = $(wildcard $(SRCDIR)/**/*.c) $(SRCDIR)/main.c

# Define object files
OBJECTS = $(SOURCES:.c=.o)

# Define output executable
TARGET = myprogram

# Default target
all: $(TARGET)

# Link object files into the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Compile each .c file into .o
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean