CC=gcc
TARGET=exe
CFLAGS=-W -Wall
STRIP=strip
OBJS=test_AES128.o AES128.o
SRCS=$(OBJS:.o=.c)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	$(STRIP) $(TARGET)

test_AES128.o: test_AES128.c
	$(CC) -c test_AES128.c

AES128.o: AES128.c AES128.h
	$(CC) -c AES128.c

clean:
	rm -f $(TARGET) $(OBJS)
