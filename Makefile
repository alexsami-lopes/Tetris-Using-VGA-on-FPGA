# Variables
CC = gcc
CFLAGS = -Wall -std=gnu99
LIBS = -lintelfpgaup
SRCS = src/tetris.c src/game_logic.c src/input.c src/graphics.c
OBJS = $(SRCS:.c=.o)

# Executable
TARGET = tetris

# Compilation rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files
src/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)
