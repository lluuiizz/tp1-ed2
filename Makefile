##
# Trabalho  Prático I - Ed2
#
# @file
# @version 0.1

CC = gcc
CFLAGS = -Werror -Wall -Wextra -pedantic
TARGET = main
SOURCE_DIR = sources
SRCS = main.c $(SOURCE_DIR)/abin.c $(SOURCE_DIR)/arvb-star.c $(SOURCE_DIR)/arvb.c $(SOURCE_DIR)/asi.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
abin.o: $(SOURCE_DIR)/abin.c
arvb-star.o: $(SOURCE_DIR)/arvb-star.c
arvb.o: $(SOURCE_DIR)/arvb.c
asi.o: $(SOURCE_DIR)/asi.c

clean:
	rm -f $(OBJS) $(TARGET)

val:
	valgrind --leak-check=yes ./main

.PHONY: all clean run

# end
