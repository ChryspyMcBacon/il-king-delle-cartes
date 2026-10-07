CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra
TARGET  = IlKingDelleCartes

all: $(TARGET)

$(TARGET): src/IlKingDelleCartes.c
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

docs:
	doxygen Doxyfile

clean:
	rm -f $(TARGET)

.PHONY: all run docs clean
