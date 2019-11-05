CC=gcc
TARGET=run
CFLAGS=-W -Wall
STRIP=strip
OBJS=rsa.o
SRCS=rsa.c

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	$(STRIP) $(TARGET)

rsa.o: rsa.c
	$(CC) -c rsa.c

clean:
	rm -f $(TARGET) $(OBJS)
