CFLAGS=-g -Werror -Wall

all: HelloWorld.exe

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)

%.exe: %.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o *.exe