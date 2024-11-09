# Variables
CC = gcc
CFLAGS = -Wall -std=gnu99
LIBS = -lintelfpgaup
SRCS = src/tetris.c src/game_logic.c src/input.c src/graphics.c #src/graphics_fpga.c  # Arquivos .c
ASM_SRCS = src/graphics_fpga.s  # Arquivo assembly

OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.s=.o)

# Executable
TARGET = tetris

# Compilation rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile C source files
src/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly source file
src/%.o: src/%.s
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)


