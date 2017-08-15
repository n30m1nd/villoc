CC    = gcc
CFLAGS = -fPIC -shared -Wall
LIBS = -ldl
SOURCES = libvilloc.c
TARGET  = libvilloc.so

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(TARGET)

clean : 
	rm libvilloc.so
