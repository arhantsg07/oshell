CC := gcc
CFLAGS := -std=gnu11 -Wall -Wextra -pedantic -g
TARGET := unix_shell
SRCS := unix_shell.c history.c parser.c
OBJS := $(SRCS:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c history.h parser.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)