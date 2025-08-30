CC = gcc
CFLAGS = -Wall -Wextra

TARGET = snake
SRCS = src/main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lSDL2

.PHONY: all run
run:
	./$(TARGET)
