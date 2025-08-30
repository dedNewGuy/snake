CXX = g++
CXX_FLAGS = -Wall -Wextra

TARGET = snake
SRCS = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(SRCS)

.PHONY: all run
run:
	./$(TARGET)
