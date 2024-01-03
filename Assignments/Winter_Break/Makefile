CFLAGS=-g -Werror -Wall

all: HelloWorld.exe

# Object files
%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)

# Executable files
%.exe: %.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o *.exe