CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = bin/server
OBJS = bin/server.o bin/utils.o
BUILD_DIR = bin

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BUILD_DIR)/server.o: server.c server.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c server.c -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/utils.o: utils.c utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils.c -o $(BUILD_DIR)/utils.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
