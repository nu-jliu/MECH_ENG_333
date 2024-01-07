CFLAGS=-g -Werror -Wall
TARGETS=invest.exe ascii.exe bubble.exe
CC=gcc

all: $(TARGETS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

%.exe: %.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o *.exe $(TARGETS)