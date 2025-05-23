# Makefile for Multiplication Game
# Computer Architecture Term Project

# Compiler and flags - simulating instruction set architecture
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = -lncurses

# Target executable
TARGET = mult_game

# Source files
SOURCES = mult_game.c
HEADERS = mult_game.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete. Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) game_save.dat
	@echo "Clean complete."

# Install ncurses development library (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install libncurses5-dev libncursesw5-dev

# Install ncurses development library (CentOS/RHEL/Fedora)
install-deps-rpm:
	sudo yum install ncurses-devel || sudo dnf install ncurses-devel

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Check for memory leaks (requires valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Static analysis (requires cppcheck)
static-analysis:
	cppcheck --enable=all --std=c99 $(SOURCES)

# Format code (requires astyle)
format:
	astyle --style=kr --indent=spaces=4 *.c *.h

# Archive project
archive:
	tar -czf multiplication_game_$(shell date +%Y%m%d).tar.gz *.c *.h Makefile README.md

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the game (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  install-deps - Install ncurses library (Debian/Ubuntu)"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  run          - Build and run the game"
	@echo "  memcheck     - Run with valgrind memory checking"
	@echo "  format       - Format source code"
	@echo "  archive      - Create project archive"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all clean install-deps install-deps-rpm debug release run memcheck static-analysis format archive help