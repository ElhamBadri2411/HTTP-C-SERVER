CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = server
OBJS = server.o utils.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(TARGET) $(OBJS)
