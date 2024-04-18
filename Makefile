CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = bin/server
OBJS = bin/server.o bin/utils.o bin/file.o bin/routes_handler.o bin/json.o
BUILD_DIR = bin

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

$(BUILD_DIR)/server.o: server.c server.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c server.c -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/utils.o: utils.c utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils.c -o $(BUILD_DIR)/utils.o

$(BUILD_DIR)/file.o: file.c file.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c file.c -o $(BUILD_DIR)/file.o

$(BUILD_DIR)/routes_handler.o: routes_handler.c routes_handler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c routes_handler.c -o $(BUILD_DIR)/routes_handler.o

$(BUILD_DIR)/json.o: json.c json.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c json.c -o $(BUILD_DIR)/json.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
