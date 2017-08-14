SHELL = /bin/sh
CC    = gcc
CFLAGS = -fPIC -pedantic -Wall -Wextra -march=native -shared
LIBS = -ldl

TARGET  = libvilloc.so

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET)

clean : 
	rm libvilloc.so
