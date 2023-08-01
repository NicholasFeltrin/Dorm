CC := gcc
CFLAGS := -Wall -lzip

SRC := main.c

TARGET := dorm

.PHONY: all debug clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

debug: $(SRC)
	$(CC) $(CFLAGS) -g -o $@ $^

clean:
	rm -f $(TARGET) debug
