CFLAGS=-g -Werror -Wall

TARGET=HelloWorld.exe

all: $(TARGET)

# Object files
%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)

# Executable files
HelloWorld.exe: HelloWorld.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o $(TARGET)